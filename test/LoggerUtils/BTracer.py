#!/usr/bin/python2.7
# BTracer.py
#
# :Organization:  BOSE CORPORATION
# 
# :Copyright:  COPYRIGHT 2015 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168
"""
Bose Tracer

This decorator class needs to be imported AFTER the logging class was imported
and the logger is created.  The usage is as follows:

    from BLogger import *
    blogger = BLogger('dog_tester', '/tmp/dogs.log')
    logger = blogger.configure_logger()
    logger.setLevel( logging.INFO )
    from BTracer import BTracer

    @BTracer(logger)
    def f():
    
The reason for this is that referencing imported decorator instantiates it, and 
resultant encapsulating code expects all the variables to exist at that time.
It is also impossible to modify the created code to supply a different logger.
"""
from functools import wraps

class BTracer(object):
    """
    This is a decorator class used for tracing test functions.  
    It is using INFO level.
    """
    def __init__(self, logger):
        """
        We expect that at this point the logger object exists and is initialized
        """
        self.logger = logger
    
    def __call__(self, func):
        """
        When decorator is encountered (@BTracer), this function is called
        and it returns wrapper traced_func(), which does the pre- and post-
        logging around a call to the target function func().
        traced_func() is returned and assigned to the name of the decorated
        function.
        """
        @wraps(func)
        def traced_func( *args, **kwargs ):
            self.logger.info( "entering >> {}{}".format(func.__name__, args[1:]) )
            ok = func( *args, **kwargs )
            self.logger.info( "PASS" if ok else "FAIL" )
            self.logger.info( "exiting << %s" % (func.__name__) )
            return ok  
        return traced_func

