#!/usr/bin/python
import sys
import io
import json
import re
import argparse
import imp
import os
import inspect
import copy
import clang.cindex
from pprint import pprint

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

"""
Recursively search the AST for enum <name>; return the cursor (or None if not found)
"""
def lookfor_enum(cursor, name): 
  ret = None

  for c in cursor.get_children():
    bare_name = re.sub(r'.*::', '', c.type.spelling)
    if (c.kind == clang.cindex.CursorKind.ENUM_DECL) and (bare_name == name):
      # found it!
      return c
    else:
      ret = lookfor_enum(c, name)
      if ret is not None:
        break

  return ret

"""
Build a dict from enum <name>.  cursor is a node in a clang AST
"""
def build_enum_map_from_ast(cursor, name):
  enum = lookfor_enum(cursor, name)

  if enum is None:
    return None 

  ret = {}
  for c in enum.get_children():
    if (c.kind == clang.cindex.CursorKind.ENUM_CONSTANT_DECL):
      ret[c.spelling] = c.enum_value

  return ret

def main():
  clang_args = ['-x', 'c++']
  index = clang.cindex.Index.create()
  argparser = argparse.ArgumentParser('generate key config')
  # friendly input file
  argparser.add_argument('--inputcfgs', dest='inputcfgs', required = True, nargs='+',
    help='\"Friendly\" json config file')
  # input header file with action enumeration
  argparser.add_argument('--actions', dest='actions_files', required = True, nargs='+',
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
      ast_keys.append(index.parse(f, clang_args).cursor)
    else:
      ast_keys.append(None)

  # merge action files ASTs (python or c/c++ headers) to action map
  action_map = {}
  for f in args.actions_files:
    if re.match(r'.*\.pyc$', f):
      pb = imp.load_compiled('p', os.path.abspath(f))
      a = build_enum_map_from_proto(pb, 'KEY_ACTION')
      if a is not None:
        action_map.update(a)
    else:
      ast_actions = index.parse(f, clang_args).cursor
      # build enum map from events
      a = build_enum_map_from_ast(ast_actions, 'KEY_ACTION')
      if a:
        action_map.update(a)
      else:
        a = build_enum_map_from_ast(ast_actions, 'ActionCommon_t')
        if a:
          action_map.update(a)

  # build enum maps from ASTs
  key_maps = []
  for a in ast_keys:
    if a is not None:
      key_maps.append(build_enum_map_from_ast(a, 'KEY_VALUE'))
    else:
      key_maps.append(None)

  # merge user config files
  j = {}
  j['KeyTable'] = []
  for f in args.inputcfgs:
    ifile = open(f).read()
    jtmp = json.loads(ifile)
    j['KeyTable'] += jtmp['KeyTable']

  print(action_map)
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

