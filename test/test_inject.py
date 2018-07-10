#!/usr/bin/python
import json
import pytest
import argparse
from common import FrontDoorAPI

"""
  Send a GET request
"""
def do_get(fd):
    r = fd.send('GET', '/injectKey')
    j = json.loads(r)
    print('GET reply is {}'.format(r))

def main():
    argparser = argparse.ArgumentParser('key injection test client')
    argparser.add_argument('--ip', dest='ip_addr', required=True,
        help='IP address')
    argparser.add_argument('--key', dest='inject_key', required=True,
        help='key to inject')
    argparser.add_argument('--type', dest='inject_type', required=True,
        choices=['press', 'release', 'duration'],
        help='type of inject')
    argparser.add_argument('--duration', dest='inject_duration', default=500,
        help='duration (for type==duration) (default=500)')
    args = argparser.parse_args()

    print('connecting to {}'.format(args.ip_addr))
    fd = FrontDoorAPI.FrontDoorAPI(args.ip_addr)

    if args.inject_type == 'duration':
        j = json.dumps({'key' : args.inject_key.upper(), 'duration' : int(args.inject_duration)}, indent=4)
    else:
        j = json.dumps({'key' : args.inject_key.upper(), 'keyState' : args.inject_type.upper()}, indent=4)

    print('sending {}'.format(j))
    r = fd.send('PUT', '/injectKey', j)
    print('PUT (press) reply is {}'.format(r))




if __name__ == '__main__':
  main()

