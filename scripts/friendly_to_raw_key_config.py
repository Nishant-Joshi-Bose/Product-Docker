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

def lookfor(node, target):
  ret = None

  for n0 in node.children():
    n = n0[1]
    if n.__class__.__name__ == target:
      return n
    else:
      ret = lookfor(n, target)

  return ret

def build_enum_map_from_proto(pb, name):
  ret = {}
  e = getattr(pb, '_' + name.upper())
  for v in e.values:
    ret[v.name] = v.number

  return ret

def build_enum_map(ast, name):
  ret = {}
  cur_val = 0
  for n0 in ast.children():
    n = n0[1]
    if (n.__class__.__name__ ==  'Typedef') and (n.name == name):
      # walk down to the enumerator list
      enum_list = lookfor(n, 'EnumeratorList')
      for e0 in enum_list.children():
        e = e0[1]

        # if it has a value, use it, otherwise use current value
        if e.value != None:
          v = e.value
          if v.__class__.__name__ == 'Constant':
            cur_val = int(v.value)
            val = cur_val
          elif v.__class__.__name__ == 'ID':
            val = cur_val
        else:
          val = cur_val

        ret[e.name] = val 
        cur_val += 1
  return ret

def main():
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
      ast_keys.append(parse_file(f, use_cpp=True))
    else:
      ast_keys.append(None)

  if re.match(r'.*\.pyc$', args.actions_file):
    pb = imp.load_compiled('p', os.path.abspath(args.actions_file))
    action_map = build_enum_map_from_proto(pb, 'KEY_ACTION')
  else:
    ast_actions = parse_file(args.actions_file, use_cpp=True)
    # build enum map from events
    action_map = build_enum_map(ast_actions, 'KEY_ACTION')

  # build enum maps from ASTs
  key_maps = []
  for a in ast_keys:
    if a is not None:
      key_maps.append(build_enum_map(a, 'KEY_VALUE'))
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
    if not origin_name in ORIGIN_NAMES:
      print('Entry {}, Unknown origin {}, skipping'.format(i, origin_name))
      continue
    if not event_name in EVENT_NAMES:
      print('Entry {}, Unknown event {}, skipping'.format(i, event_name))
      continue
    if not action_name in action_map:
      print('Entry {}, Unknown action {}, skipping'.format(i, action_name))
      continue

    # replace with numeric values
    origin = ORIGIN_NAMES[origin_name]
    event = EVENT_NAMES[event_name]
    e['Origin'] = origin
    e['KeyEvent'] = event
    e['Action'] = action_map[action_name]
 
    key_map = key_maps[origin]
    if key_map is None:
      print('Entry {}, No key file supplied for origin {}, skipping'.format(i, origin_name))
      continue

    for k in range(len(e['KeyList'])):
      key = e['KeyList'][k]
      if not key in key_map:
        print('Entry {} / {}, Unknown key {}, skipping entry'.format(i, k, key))
        discard = 1
        break
      else:
        e['KeyList'][k] = key_map[key]
  
    if discard == 0:
      keymap['KeyTable'].append(e)

  s = json.dumps(keymap, indent=4)
  with io.FileIO(args.outputcfg, "w") as file:
    file.write(s)
  
if __name__ == '__main__':
  main()

