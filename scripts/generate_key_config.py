#!/usr/bin/python
import sys
import io
import json
import re
from pycparser import c_parser, c_ast, parse_file
import argparse
from collections import OrderedDict

"""
Generate a default key file from the RivieraLPM_KeyValues.h (or similar)
header file
"""

def lookfor(node, target):
  ret = None

  for n0 in node.children():
    n = n0[1]
    if n.__class__.__name__ == target:
      return n
    else:
      ret = lookfor(n, target)

  return ret

def main():
  argparser = argparse.ArgumentParser('generate key config')
  argparser.add_argument('lpm_key_file', help='LPM key value header')
  argparser.add_argument('key_config_file', help='Key Config output')
  argparser.add_argument('key_events_file', help='Key Events header output')
  args = argparser.parse_args()

  ast = parse_file(args.lpm_key_file, use_cpp=True)

  event_id = 0
  cur_val = 0
  keymap = {'keyTable' : []}
  header_string = '//\n'
  header_string += '// DO NOT EDIT - AUTOGENERATED BY generate_key_config.py\n'
  header_string += '//\n'
  header_string += '#pragma once\n\n'
  header_string += 'enum {\n'
  

  for n0 in ast.children():
    n1 = n0[1]
    if n1.__class__.__name__ ==  'Typedef' and n1.name == 'KEY_VALUE':
      # walk down to the enumerator list
      enum_list = lookfor(n1, 'EnumeratorList')
      for e0 in enum_list.children():
        e = e0[1]
        if e.value != None:
          v = e.value
          if v.__class__.__name__ == 'Constant':
            cur_val = int(v.value)
            val = cur_val
          elif v.__class__.__name__ == 'ID':
            val = cur_val
        else:
          val = cur_val

        if re.compile(r'NUM_BOSE_KEYS').match(e.name):
          continue

        if re.compile(r'BOSE_INVALID_KEY').match(e.name):
          continue

        pat = re.compile(r'^BOSE_')
        event_name = pat.sub('KEY_EVENT_', e.name)
        pat = re.compile(r'^KEY_VAL_')
        event_name = pat.sub('KEY_EVENT_', event_name)
        
        keymap['keyTable'].append(OrderedDict([
          ('Keys', [val]),
          ('TimeOut', 300),  
          ('Repeat', False), 
          ('ActionOnTimeout', False), 
          ('Event', event_id),
          ('EventName', event_name),
          ('KeyName', e.name),
        ]))
        header_string += '    {} = {},\n'.format(event_name, event_id)

        cur_val += 1
        event_id += 1

  header_string += '};\n'

#TODO : clean up "Keys" output (collapse to a single line)
  s = json.dumps(keymap, indent=4)
  with io.FileIO(args.key_config_file, "w") as file:
    file.write(s)
  
  with io.FileIO(args.key_events_file, "w") as file:
    file.write(header_string)

if __name__ == '__main__':
  main()

