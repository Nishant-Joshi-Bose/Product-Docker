import sys
import io
import json
import re
import argparse
import os
import copy
import pprint
import imp

"""
Given a key list enumeration file, an actions enumeration file, and a 
"friendly" configuration file, generate a key configuration file suitable 
for parsing by the CastleKeyHandler component
"""

ORIGIN_NAMES = {
    "CONSOLE_BUTTON": 0,
    "CAPSENSE": 1,
    "IR": 2,
    "RF": 3,
    "CEC": 4,
    "NETWORK": 5,
    "TAP": 6
}

EVENT_NAMES = {
    "PRESS": 0,
    "RELEASE": 1,
    "PRESS_HOLD": 2,
    "PRESS_HOLD_REPEAT": 3,
    "RELEASE_BURST": 4,
    "RELEASE_ALWAYS": 5
}

ORIGIN_NAMES_REV = {
    0: "CONSOLE_BUTTON",
    1: "CAPSENSE",
    2: "IR",
    3: "RF",
    4: "CEC",
    5: "NETWORK",
    6: "TAP"
}

EVENT_NAMES_REV = {
    0: "PRESS",
    1: "RELEASE",
    2: "PRESS_HOLD",
    3: "PRESS_HOLD_REPEAT",
    4: "RELEASE_BURST",
    5: "RELEASE_ALWAYS"
}

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
    common_intents = load_source('CommonIntents', args.common_intents_file)
    custom_intents = load_source('CustomIntents', args.custom_intents_file)
    key_defs = load_source('BoseKeys', args.key_file)

    key_enum = build_enum_map_from_proto(key_defs._KEY_VALUE, False)
    common_intents_enum = build_enum_map_from_proto(
        common_intents._ACTIONCOMMON_T_ACTIONS, False)
    # custom is allowed to be empty
    custom_intents_enum = {}
    if custom_intents is not None:
        custom_intents_enum = build_enum_map_from_proto(
            custom_intents._ACTIONCUSTOM_T_ACTIONS, False)

    # merge intents
    action_enum = {}
    action_enum.update(common_intents_enum)
    action_enum.update(custom_intents_enum)

    # merge user config files
    j = {}
    j['keyTable'] = []
    for f in args.inputcfgs:
        ifile = open(f).read()
        jtmp = json.loads(ifile)
        j['keyTable'] += jtmp['keyTable']

    print(action_enum)
    # transmogrify the key table
    keymap = {'keyTable': []}

    for i, e in enumerate(j['keyTable']):
        discard = 0
        # sanity-check entry
        origin_name = e['origin']
        event_name = e['keyEvent']
        action_name = e['action']

        # origin is a list now
        for origin_name in e['origin']:
            if not origin_name in ORIGIN_NAMES:
                print('Entry {}, Unknown origin {}, skipping'.format(i, origin_name))
                continue
            if not event_name in EVENT_NAMES:
                print('Entry {}, Unknown event {}, skipping'.format(i, event_name))
                continue
            if not action_name in action_enum:
                print('Entry {}, Unknown action {}, skipping'.format(i, action_name))
                continue

            oe = copy.deepcopy(e)

            # replace with numeric values
            origin = ORIGIN_NAMES[origin_name]
            event = EVENT_NAMES[event_name]
            oe['origin'] = origin
            oe['keyEvent'] = event
            oe['action'] = action_enum[action_name]
            # filter being in the output shouldn't hurt anything, but remove it for readability
            oe.pop('filter', None)

            for k in range(len(e['keyList'])):
                key = e['keyList'][k]
                if not key in key_enum:
                    print('Entry {} / {}, Unknown key {}, skipping entry ({}, {})'.format(i,
                                                                                          k, key, origin_name, origin))
                    discard = 1
                    break
                else:
                    print('Entry {} / {}, key {}, do ({}, {})'.format(i,
                                                                      k, key, origin_name, origin))
                    oe['keyList'][k] = key_enum[key]

            if discard == 0:
                keymap['keyTable'].append(oe)

    s = json.dumps(keymap, indent=4)
    with io.FileIO(args.outputcfg, "w") as file:
        file.write(s)


"""
real main
"""


def main():
    argparser = argparse.ArgumentParser('generate key config')
    # friendly input file
    argparser.add_argument('--inputcfgs', dest='inputcfgs', required=True, nargs='+',
                           help='\"Friendly\" json config file')
    # input header file with action enumeration
    argparser.add_argument('--common', dest='common_intents_file', required=True,
                           help='Python protobuf output containing common intent definitions')
    argparser.add_argument('--custom', dest='custom_intents_file',
                           help='Python protobuf output containing custom intent definitions')
    # key definitions
    argparser.add_argument('--keys', dest='key_file', required=True,
                           help='Python protobuf output containint key definitions')
    # output configuration to generate
    argparser.add_argument('--outputcfg', dest='outputcfg', required=True,
                           help='Key config json output')
    generate_raw_config(argparser.parse_args())


"""
main main
"""
if __name__ == '__main__':
    main()
