#!/usr/bin/python
import sys
import io
import json
import re
from pycparser import c_parser, c_ast, parse_file
import argparse

"""
Given a key list enumeration file, an events enumeration file, and a 
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

def build_events_table():
  pass

def main():
  argparser = argparse.ArgumentParser('generate key config')
  argparser.add_argument('config_file', help='\"friendly\" json config file')
  argparser.add_argument('lpm_key_file', help='LPM key values header')
  argparser.add_argument('events_file', help='Event values output')
  argparser.add_argument('key_config_file', help='Key config json output')
  args = argparser.parse_args()

  # build ASTs from the header files
  ast_keys = parse_file(args.lpm_key_file, use_cpp=True)
  ast_events = parse_file(args.events_file, use_cpp=True)

  # harvest the useful information from the ASTs
  key_map = build_enum_map(ast_keys, 'KEY_VALUE')
  event_map = build_enum_map(ast_events, 'KEY_EVENT')

  ifile = open(args.config_file).read()
  j = json.loads(ifile)

  # transmogrify the key table
  keymap = {'KeyTable' : []}

  for e in j['KeyTable']:
    e['Origin'] = ORIGIN_NAMES[e['Origin']]
    e['KeyEvent'] = EVENT_NAMES[e['KeyEvent']]
    e['Action'] = event_map[e['Action']]
  
    for k in range(len(e['KeyList'])):
      e['KeyList'][k] = key_map[e['KeyList'][k]] 

    keymap['KeyTable'].append(e)

  s = json.dumps(keymap, indent=4)
  with io.FileIO(args.key_config_file, "w") as file:
    file.write(s)
  
if __name__ == '__main__':
  main()

