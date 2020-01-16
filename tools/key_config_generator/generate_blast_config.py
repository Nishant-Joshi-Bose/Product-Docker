import sys
import io
import json
import re
import argparse
import os
import copy
import imp

"""
convert protobuf enum to a dict
"""


def build_enum_map_from_proto(e, swap):
    ret = {}
    for v in e.values:
        if swap:
            ret[v.number] = v.name
        else:
            ret[v.name] = v.number

    return ret


"""
Load a python source file
"""


def load_source(name, filename):
    try:
        print('loading {}'.format(filename))
        return imp.load_source(name, filename)
    except:
        print('couldn''t load file {}'.format(filename))
        return None


"""
Friendly config -> raw config
"""


def generate_raw_config(args):
    key_defs = load_source('BoseKeys', args.key_file)
    key_map = build_enum_map_from_proto(key_defs._KEY_VALUE, False)

    j = {}
    j['blastTable'] = []
    ifile = open(args.inputcfg).read()
    j = json.loads(ifile)

    # transmogrify the blast table
    blastmap = {'blastTable': []}

    for i, e in enumerate(j['blastTable']):
        oe = copy.deepcopy(e)
        oe['keyList'] = []
        key_list = e['keyList']
        for key in key_list:
            if not key in key_map:
                print('Entry {}, Unknown key {}, skipping entry'.format(i, key))
                continue
            else:
                print('Entry {}, key {}'.format(i, key))
                oe['keyList'].append(key_map[key])

        blastmap['blastTable'].append(oe)

    s = json.dumps(blastmap, indent=4)
    with open(args.outputcfg, "w") as file:
        file.write(s)


def main():
    argparser = argparse.ArgumentParser('generate key blasting config')
    # friendly input file
    argparser.add_argument('--inputcfg', dest='inputcfg', required=True,
                           help='\"Friendly\" json config file')
    # key definitions
    argparser.add_argument('--keys', dest='key_file', required=True,
                           help='Python protobuf output containint key definitions')
    # output configuration to generate
    argparser.add_argument('--outputcfg', dest='outputcfg', required=True,
                           help='Key config json output')
    args = argparser.parse_args()
    generate_raw_config(args)


if __name__ == '__main__':
    main()
