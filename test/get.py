#!/usr/bin/python
import json
import pytest
import argparse
import sys
import common
import os
from common import FrontDoorAPI, g_username, g_password

"""
  Send a GET request
"""
def do_get(fd, endpoint):
  r = fd.send('GET', endpoint)
  print('GET reply is {}'.format(r))


def main():
  argparser = argparse.ArgumentParser('FrontDoor test client')
  argparser.add_argument('--ip', dest='ip_addr', required=True,
    help='IP address')
  argparser.add_argument('--ep', dest='endpoint', required=True,
    help='endpoint name')
  args = argparser.parse_args()

  print('connecting to {}'.format(args.ip_addr))
  fd = FrontDoorAPI.FrontDoorAPI(args.ip_addr, email=g_username, password=g_password)

  do_get(fd, args.endpoint)

if __name__ == '__main__':
  main()

