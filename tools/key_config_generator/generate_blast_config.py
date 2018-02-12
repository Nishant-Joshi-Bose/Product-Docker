import sys
import io
import json
import re
import argparse
import os
import copy
import clang.cindex


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
def build_enum_map_from_ast(cursor, name, swap):
  enum = lookfor_enum(cursor, name)

  if enum is None:
    return None 

  ret = {}
  for c in enum.get_children():
    if (c.kind == clang.cindex.CursorKind.ENUM_CONSTANT_DECL):
      if swap:
        ret[c.enum_value] = c.spelling
      else:
        ret[c.spelling] = c.enum_value

  return ret

def verify_clang_tu(tu):
  for d in tu.diagnostics:
    if d.severity >= 3:
      print('severity: {}'.format(d.severity))
      print('location: {}'.format(d.location))
      print('spelling: {}'.format(d.spelling))
      print('option: {}'.format(d.option))
      raise Exception('clang encountered an error')


"""
Friendly config -> raw config
"""
def generate_raw_config(clang_args, index, args):
  # build AST from the key header files
  tu = index.parse(args.keyfile, clang_args)
  verify_clang_tu(tu)
  key_map = build_enum_map_from_ast(tu.cursor, 'KEY_VALUE', False)

  j = {}
  j['blastTable'] = []
  ifile = open(args.inputcfg).read()
  j = json.loads(ifile)

  # transmogrify the blast table
  blastmap = {'blastTable' : []}

  for i, e in enumerate(j['blastTable']):
    oe = copy.deepcopy(e)
    key = e['key']
    if not key in key_map:
      print('Entry {}, Unknown key {}, skipping entry'.format(i, key))
      continue 
    else:
      print('Entry {}, key {}'.format(i, key))
      oe['key'] = key_map[key]
    
    blastmap['blastTable'].append(oe)
 
  s = json.dumps(blastmap, indent=4)
  with io.FileIO(args.outputcfg, "w") as file:
    file.write(s)

def main():
  argparser = argparse.ArgumentParser('generate key blasting config')
  # friendly input file
  argparser.add_argument('--inputcfg', dest='inputcfg', required = True,
    help='\"Friendly\" json config file')
  argparser.add_argument('--keyfile', dest='keyfile', required = True,
    help='Header file with BOSE_* key definitions')
  # output configuration to generate
  argparser.add_argument('--outputcfg', dest='outputcfg', required = True,
    help='Key config json output')
  argparser.add_argument('--incdirs', dest='inc_dirs', nargs='+',
    help='Path(s) for include files')
  args = argparser.parse_args()

  clang_args = ['-x', 'c++']
  if args.inc_dirs is not None:
    for inc in args.inc_dirs:
      clang_args.append('-I{}'.format(inc))
  index = clang.cindex.Index.create()

  generate_raw_config(clang_args, index, args)
  
if __name__ == '__main__':  
  main()

