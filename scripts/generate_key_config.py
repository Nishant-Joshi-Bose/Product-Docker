#!/usr/bin/python
import sys
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
  argparser.add_argument('filename', help='LPM key value header')
  args = argparser.parse_args()

  ast = parse_file(args.filename, use_cpp=True)

  cur_val = 0
  keymap = {'keyTable' : []}

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

        pat = re.compile(r'KEY_VAL_AVAIL_')
        match = pat.match(e.name)
        if match:
          event = 255
        else:
          event = val
        
        keymap['keyTable'].append(OrderedDict([
          ('Keys', [val]),
          ('TimeOut', 300),  
          ('Repeat', False), 
          ('ActionOnTimeout', False), 
          ('Event', event),
          ('Description', e.name),
        ]))

        cur_val += 1

#TODO : clean up "Keys" output (collapse to a single line)
  s = json.dumps(keymap, indent=4)
  print('{}'.format(s))
  

if __name__ == '__main__':
  main()

