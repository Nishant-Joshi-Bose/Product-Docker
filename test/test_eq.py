#!/usr/bin/python
import json
import pytest
import argparse
from common import FrontDoorAPI

"""
  Send a GET request
"""
def do_get(fd):
  r = fd.send('GET', '/audio/eqSelect')
  j = json.loads(r)

  print('GET reply is {}'.format(r))

"""
  Send a PUT request with "mode" parameter set
"""
def do_put(fd):
  eq_modes = ['EQ_OFF', 'EQ_AIQ_A', 'EQ_AIQ_B', 'EQ_RETAIL_A', 'EQ_RETAIL_B', 'EQ_RETAIL_C']
  while 1:
    for i, m in enumerate(eq_modes):
      print('{}: {}'.format(i, m))
    cmd = raw_input('> ')
    cmd = int(cmd, 10) 
    if(cmd < len(eq_modes)):
      mode = eq_modes[cmd]
    else:
      print('invalid mode')
      continue
      
    j = json.dumps({'mode' : mode}, indent=4)
    r = fd.send('PUT', '/audio/eqSelect', j)
    print('PUT reply is {}'.format(r))
    break

def main():
  argparser = argparse.ArgumentParser('EQ test client')
  argparser.add_argument('--ip', dest='ip_addr', required=True,
    help='IP address')
  args = argparser.parse_args()

  print('connecting to {}'.format(args.ip_addr))
  fd = FrontDoorAPI.FrontDoorAPI(args.ip_addr)

  while 1:
    print('===== EQ test client =====')
    print('p: PUT')
    print('g: perform GET')
    print('q: quit')
    cmd = raw_input('cmd> ')

    if(cmd == 'p'):
      do_put(fd)
    elif(cmd == 'g'):
      do_get(fd)
    elif(cmd == 'q'):
      print('bye!')
      exit()
    else:
      print('unknown command {}'.format(cmd))

if __name__ == '__main__':
  main()

