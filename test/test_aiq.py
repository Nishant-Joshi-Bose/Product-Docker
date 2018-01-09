#!/usr/bin/python
import json
import pytest
import argparse
from FrontDoorAPI import FrontDoorAPI

"""
  Send a GET request
"""
def do_get(fd):
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

"""
  Send a PUT request with "mode" parameter set
"""
def do_put_mode(fd):
  while 1:
    print('1: Enabled Normal')
    print('2: Enabled Retail')
    print('3: Disabled')
    cmd = raw_input('> ')
    if(cmd == '1'):
      mode = 'Enabled Normal'
    elif(cmd == '2'):
      mode = 'Enanbled Retail'
    elif(cmd == '3'):
      mode = 'Disabled'
    else:
      print('invalid mode')
      continue
      
    j = json.dumps({'mode' : mode}, indent=4)
    r = fd.send('PUT', '/adaptiq', j)
    print('PUT reply is {}'.format(r))
    break

"""
  Send a PUT request with "action" parameter set
"""
def do_put_action(fd):
  while 1:
    print('1: enter')
    print('2: cancel')
    print('3: advance')
    cmd = raw_input('> ')
    if(cmd == '1'):
      action = 'enter'
    elif(cmd == '2'):
      action = 'cancel'
    elif(cmd == '3'):
      action = 'advance'
    else:
      print('invalid action')
      continue
      
    j = json.dumps({'action' : action}, indent=4)
    print('sending {}'.format(j))
    r = fd.send('PUT', '/adaptiq', j)
    print('PUT reply is {}'.format(r))
    break

  pass


def main():
  argparser = argparse.ArgumentParser('AIQ test client')
  argparser.add_argument('--ip', dest='ip_addr', required=True,
    help='IP address')
  args = argparser.parse_args()

  print('connecting to {}'.format(args.ip_addr))
  fd = FrontDoorAPI.FrontDoorAPI(args.ip_addr)

  while 1:
    print('===== AIQ test client =====')
    print('a: PUT action')
    print('m: PUT mode')
    print('g: perform GET')
    print('q: quit')
    cmd = raw_input('cmd> ')

    if(cmd == 'a'):
      do_put_action(fd)
    elif(cmd == 'm'):
      do_put_mode(fd)
    elif(cmd == 'g'):
      do_get(fd)
    elif(cmd == 'q'):
      print('bye!')
      exit()
    else:
      print('unknown command {}'.format(cmd))

if __name__ == '__main__':
  main()

