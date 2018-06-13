import sys
import os
import ConfigParser
import subprocess
import atexit

def cleanup():
    global g_transport
    print('exiting...')
    if g_transport == 'adb':
        subprocess.call('adb forward --remove tcp:8082', shell=True)

cfg = ConfigParser.ConfigParser()
cfg.read('config.ini')
if 'config' in cfg.sections():
    g_username = cfg.get('config', 'username')
    g_password = cfg.get('config', 'password')
    g_frontdoor_path = cfg.get('config', 'frontdoor_path')
    g_transport = cfg.get('config', 'transport')
else:
    g_username = raw_input('username: ')
    g_password = raw_input('password: ')
    g_frontdoor_path = raw_input('frontdoor library path (i.e. /scratch/bose)')
    g_transport = raw_input('transport [adb|tcp]: ')
    save = raw_input('save to file [y|n]?: ')
    if (save == 'y') or (save == 'Y'):
        cfg.add_section('config')
        cfg.set('config', 'username', g_username)
        cfg.set('config', 'password', g_password)
        cfg.set('config', 'frontdoor_path', g_frontdoor_path)
        cfg.set('config', 'transport', g_transport)
        with open('config.ini', 'wb') as cfg_file:
            cfg.write(cfg_file)
    
sys.path.append(os.path.abspath(g_frontdoor_path))
from CastleTestUtils.CastleTestUtils.FrontDoorAPI import FrontDoorAPI

if g_transport == 'adb':
    subprocess.call('adb forward tcp:8082 tcp:8084', shell=True)
    g_username = None
    g_password = None
    atexit.register(cleanup)


