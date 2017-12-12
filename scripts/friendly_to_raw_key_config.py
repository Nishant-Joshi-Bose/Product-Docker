#!/usr/bin/python
import sys
import io
import json
import re
from pycparser import c_parser, c_ast, parse_file
import argparse
import imp
import os
import inspect
import copy
import clang.cindex
from pprint import pprint
import yaml
import jsonpickle

"""
Given a key list enumeration file, an actions enumeration file, and a 
"friendly" configuration file, generate a key configuration file suitable 
for parsing by the CastleKeyHandler component
"""

ORIGIN_NAMES = {
  "CONSOLE_BUTTON" : 0, 
  "CAPSENSE" : 1, 
  "IR" : 2, 
  "RF" : 3, 
  "CEC" : 4, 
  "NETWORK" : 5,
  "TAP" : 6
}

EVENT_NAMES = {
  "PRESS" : 0,  
  "RELEASE" : 1, 
  "PRESS_HOLD" : 2, 
  "PRESS_HOLD_REPEAT" : 3,
  "RELEASE_BURST" : 4
}

def build_enum_map_from_proto(pb, name):
  ret = {}
  e = getattr(pb, '_' + name.upper())
  for v in e.values:
    ret[v.name] = v.number

  return ret


#        if n.kind == clang.cindex.CursorKind.ENUM_DECL:
#          print('this is {} - {}'.format(n.kind, n.type.spelling))
#          for c in n.get_children():
#            #print('   c is {}'.format(c.kind))
#            pass

"""
Recursively search the AST for enum <name>; return the cursor (or None if not found)
"""
def lookfor_enum(cursor, name):
  ret = None

  for c in cursor.get_children():
    if (c.kind == clang.cindex.CursorKind.ENUM_DECL) and (c.type.spelling == name):
      return c
    else:
      ret = lookfor_enum(c, name)

  return ret

def build_enum_map_from_ast(cursor, name):
  ret = {}
  enum = lookfor_enum(cursor, name)

  # nothing found, just return an empty dict
  if enum is None:
    return ret

  for c in enum.get_children():
    if (c.kind == clang.cindex.CursorKind.ENUM_CONSTANT_DECL):
      ret[c.spelling] = c.enum_value

  return ret

def main():
  index = clang.cindex.Index.create()
  argparser = argparse.ArgumentParser('generate key config')
  # friendly input file
  argparser.add_argument('--inputcfg', dest='inputcfg', required = True,
    help='\"Friendly\" json config file')
  # input header file with action enumeration
  argparser.add_argument('--actions', dest='actions_file', required = True,
    help='Event values output header file')
  # key definitions for different origins, all optional
  argparser.add_argument('--console', dest='console_file',
    help='Console key values header')
  argparser.add_argument('--cap', dest='cap_file',
    help='Capsense key values header')
  argparser.add_argument('--ir', dest='ir_file',
    help='IR key values header')
  argparser.add_argument('--rf', dest='rf_file',
    help='RF key values header')
  argparser.add_argument('--cec', dest='cec_file',
    help='CEC key values header')
  argparser.add_argument('--net', dest='net_file',
    help='Net key values header')
  argparser.add_argument('--tap', dest='tap_file',
    help='TAP key values header')
  # output configuration to generate
  argparser.add_argument('--outputcfg', dest='outputcfg', required = True,
    help='Key config json output')
  args = argparser.parse_args()

  key_files = (args.console_file, args.cap_file, args.ir_file, args.rf_file,
      args.cec_file, args.net_file, args.tap_file)

  # build ASTs from the header files
  ast_keys = []
  for f in key_files:
    if f is not None:
      print('parsing {}'.format(f))
      ast_keys.append(index.parse(f).cursor)
    else:
      ast_keys.append(None)

  if re.match(r'.*\.pyc$', args.actions_file):
    pb = imp.load_compiled('p', os.path.abspath(args.actions_file))
    action_map = build_enum_map_from_proto(pb, 'KEY_ACTION')
  else:
    ast_actions = index.parse(args.actions_file).cursor
    # build enum map from events
    action_map = build_enum_map_from_ast(ast_actions, 'KEY_ACTION')

  # build enum maps from ASTs
  key_maps = []
  for a in ast_keys:
    if a is not None:
      key_maps.append(build_enum_map_from_ast(a, 'KEY_VALUE'))
    else:
      key_maps.append(None)


  ifile = open(args.inputcfg).read()
  j = json.loads(ifile)

  # transmogrify the key table
  keymap = {'KeyTable' : []}

  for i, e in enumerate(j['KeyTable']):
    discard = 0
    # sanity-check entry
    origin_name = e['Origin']
    event_name = e['KeyEvent']
    action_name = e['Action']

    # origin is a list now
    for origin_name in e['Origin']:
      if not origin_name in ORIGIN_NAMES:
        print('Entry {}, Unknown origin {}, skipping'.format(i, origin_name))
        continue
      if not event_name in EVENT_NAMES:
        print('Entry {}, Unknown event {}, skipping'.format(i, event_name))
        continue
      if not action_name in action_map:
        print('Entry {}, Unknown action {}, skipping'.format(i, action_name))
        continue

      oe = copy.deepcopy(e);
  
      # replace with numeric values
      origin = ORIGIN_NAMES[origin_name]
      event = EVENT_NAMES[event_name]
      oe['Origin'] = origin
      oe['KeyEvent'] = event
      oe['Action'] = action_map[action_name]
   
      key_map = key_maps[origin]
      if key_map is None:
        print('Entry {}, No key file supplied for origin {}, skipping'.format(i, origin_name))
        continue
  
      for k in range(len(e['KeyList'])):
        key = e['KeyList'][k]
        if not key in key_map:
          print('Entry {} / {}, Unknown key {}, skipping entry ({}, {})'.format(i, k, key, origin_name, origin))
          discard = 1
          break
        else:
          print('Entry {} / {}, key {}, do ({}, {})'.format(i, k, key, origin_name, origin))
          oe['KeyList'][k] = key_map[key]
    
      if discard == 0:
        keymap['KeyTable'].append(oe)
 
  s = json.dumps(keymap, indent=4)
  with io.FileIO(args.outputcfg, "w") as file:
    file.write(s)
  
if __name__ == '__main__':  
  main()

