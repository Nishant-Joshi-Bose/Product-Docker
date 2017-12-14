import pexpect
from time import sleep

RAW_KEY_CLI = 'raw_key'

#Eddie console has origin value of 0
ORIGIN = '0'

PRESS = '1'
RELEASE = '0'

#Eddie console key ids
MFB = '1'

MFB_press = RAW_KEY_CLI + ' ' + ORIGIN + ' ' + MFB + ' ' + PRESS
MFB_release = RAW_KEY_CLI + ' ' + ORIGIN + ' ' + MFB + ' ' + RELEASE


def mfb_playpause(  ip_addr='localhost' , con='adb' ):
    tap = None
    if con == "adb":
        tap = pexpect.spawn("adb shell")
        tap.expect("#")
        tap.sendline("telnet %s 17000" % ip_addr)
    else:
        tap = pexpect.spawn("telnet %s 17000" % ip_addr )
    
    tap.expect("->")
    tap.sendline( MFB_press )
    tap.expect("OK")
    tap.expect("->")
    print "MFB press sent"
    sleep (0.05) # sleep for 50 ms 
    
    tap.sendline( MFB_release )
    tap.expect("OK")
    print "MFB release sent"
    tap.terminate(True)
    tap = None
    sleep( 0.5 )

def mfb_nw_standby( ip_addr='localhost' , con='adb' ):
    tap = None
    if con == "adb":
        tap = pexpect.spawn("adb shell")
        tap.expect("#")
        tap.sendline("telnet %s 17000" % ip_addr)
    else:
        tap = pexpect.spawn("telnet %s 17000" % ip_addr )
    
    tap.expect("->")
    tap.sendline( MFB_press )
    tap.expect("OK")
    tap.expect("->")
    print "MFB press sent"
    sleep (2) # sleep for 2 sec
    
    tap.sendline( MFB_release )
    tap.expect("OK")
    print "MFB release sent"
    tap.terminate(True)
    tap = None
    sleep( 0.5 )

def mfb_lp_standby( ip_addr='localhost' , con='adb' ):
    tap = None
    if con == "adb":
        tap = pexpect.spawn("adb shell")
        tap.expect("#")
        tap.sendline("telnet %s 17000" % ip_addr)
    else:
        tap = pexpect.spawn("telnet %s 17000" % ip_addr )
    
    tap.expect("->")
    tap.sendline( MFB_press )
    tap.expect("OK")
    tap.expect("->")
    print "MFB press sent"
    sleep (15) # sleep for 15 sec
    
    tap.sendline( MFB_release )
    tap.expect("OK")
    print "MFB release sent"
    tap.terminate(True)
    tap = None
    sleep( 0.5 )

def mfb_next_track( ip_addr='localhost' , con='adb' ):
    tap = None
    if con == "adb":
        tap = pexpect.spawn("adb shell")
        tap.expect("#")
        tap.sendline("telnet %s 17000" % ip_addr)
    else:
        tap = pexpect.spawn("telnet %s 17000" % ip_addr )
    
    tap.expect("->")
    tap.sendline( MFB_press )
    tap.expect("OK")
    tap.expect("->")
    print "MFB press sent"
    sleep (0.05) # sleep for 50 ms 
    
    tap.sendline( MFB_release )
    tap.expect("OK")
    print "MFB release sent"
    sleep (0.05) # sleep for 50 ms 


    tap.expect("->")
    tap.sendline( MFB_press )
    tap.expect("OK")
    tap.expect("->")
    print "MFB press sent"
    sleep (0.05) # sleep for 50 ms 
    
    tap.sendline( MFB_release )
    tap.expect("OK")
    print "MFB release sent"

    tap.terminate(True)
    tap = None
    sleep( 0.5 )

def mfb_prev_track( ip_addr='localhost' , con='adb' ):
    tap = None
    if con == "adb":
        tap = pexpect.spawn("adb shell")
        tap.expect("#")
        tap.sendline("telnet %s 17000" % ip_addr)
    else:
        tap = pexpect.spawn("telnet %s 17000" % ip_addr )
    
    tap.expect("->")
    tap.sendline( MFB_press )
    tap.expect("OK")
    tap.expect("->")
    print "MFB press sent"
    sleep (0.05) # sleep for 50 ms 
    
    tap.sendline( MFB_release )
    tap.expect("OK")
    print "MFB release sent"
    sleep (0.05) # sleep for 50 ms 


    tap.expect("->")
    tap.sendline( MFB_press )
    tap.expect("OK")
    tap.expect("->")
    print "MFB press sent"
    sleep (0.05) # sleep for 50 ms 
    
    tap.sendline( MFB_release )
    tap.expect("OK")
    print "MFB release sent"
    sleep (0.05) # sleep for 50 ms 


    tap.expect("->")
    tap.sendline( MFB_press )
    tap.expect("OK")
    tap.expect("->")
    print "MFB press sent"
    sleep (0.05) # sleep for 50 ms 
    
    tap.sendline( MFB_release )
    tap.expect("OK")
    print "MFB release sent"

    tap.terminate(True)
    tap = None
    sleep( 0.5 )
