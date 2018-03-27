#!/usr/bin/python
import json
import pytest
import argparse
import copy
from FrontDoorAPI import FrontDoorAPI

def main():
    argparser = argparse.ArgumentParser('FrontDoor test client')
    argparser.add_argument('--ip', dest='ip_addr', required=True,
        help='IP address')
    args = argparser.parse_args()

    print('connecting to {}'.format(args.ip_addr))
    fd = FrontDoorAPI.FrontDoorAPI(args.ip_addr)

    req = {}
    req['sources'] = [
        {
            "details" : {
                "deviceType" : "DEVICE_TYPE_CBL_SAT",
                "ciCode"        : "Y0199",
                "activationKey"     : "ACTIVATION_KEY_CBL_SAT"
            },
            "displayName"       : "SLOT_0",
            "local"             : True,
            "multiroom"         : True,
            "sourceAccountName" : "SLOT_0",
            "sourceName"        : "PRODUCT",
            "status"            : "ready",
            "visible"           : "true",
        },
        {
            "details" : {
                "deviceType" : "DEVICE_TYPE_GAME",
                "ciCode"        : "T2778",
                "activationKey"     : "ACTIVATION_KEY_GAME"
            },
            "displayName"       : "SLOT_2",
            "local"             : True,
            "multiroom"         : True,
            "sourceAccountName" : "SLOT_2",
            "sourceName"        : "PRODUCT",
            "status"            : "ready",
            "visible"           : "true",
        },
        {
            "details" : {
                "deviceType"    : "DEVICE_TYPE_TV",
                "ciCode"        : "T2778",
            },
            "displayName": "TV", 
            "local"             : True, 
            "multiroom"         : True, 
            "sourceAccountName" : "TV", 
            "sourceName"        : "PRODUCT", 
            "status"            : "ready"
        }
    ]


    r = fd.send('PUT', '/system/sources', json.dumps(req, indent=4))
    print('reply is {}'.format(r))

if __name__ == '__main__':
  main()

