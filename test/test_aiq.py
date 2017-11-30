#!/usr/bin/python
import json
import pytest
import argparse
from FrontDoorAPI import FrontDoorAPI

def main():
  argparser = argparse.ArgumentParser('AIQ test client')
  argparser.add_argument('--ip', dest='ip_addr', required=True,
    help='IP address')
  args = argparser.parse_args()

  print('connecting to {}'.format(args.ip_addr))
  fd = FrontDoorAPI.FrontDoorAPI(args.ip_addr)

  # start off with a simple test; do a "GET" and make sure we get back the list of supported actions/modes
  r = fd.send('GET', '/adaptiq')
  j = json.loads(r)

  print('GET reply is {}'.format(r))

  print('Supported actions:')
  for i, a in enumerate(j['body']['properties']['supportedactions']):
    print('   {}: {}'.format(i, a))

  print('Supported modes:')
  for i, m in enumerate(j['body']['properties']['supportedmodes']):
    print('   {}: {}'.format(i, m))

if __name__ == '__main__':
  main()

