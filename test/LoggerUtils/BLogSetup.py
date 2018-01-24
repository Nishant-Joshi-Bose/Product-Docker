#!/usr/bin/python2.7
# BLogSetup.py
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
:Abstract: Setup the python logger for automated tests

    Individual python scripts that import this module can set an individual
    loglevel using the logger that is returned by get_logger (see python logging)
    but that should not be checked in. This setting is expected to be the global
    setting for the test station.

    Guidelines on log levels:
    CRITICAL: fatal exception or error that would cause a crash if not handled - printColor('magenta', "message")
    ERROR: something unexpected that usually prevents continuing - printColor('red', "message")
    WARNING: something unexpected, but continuing - printColor('yellow', "message")
    INFO: messages that are useful to demonstrate what is happening at a high level - printColor('blue', "message")
    DEBUG: verbose messages that are useful only to the writer of the program - printColor('grey', "message")
    Success: something expected happened - printColor('green', "message")
"""

__version__ = "$Revision$"
__docformat__ = "restructuredtext en"

import logging
import datetime
import time
import sys
import os
import re

# ------------------------------------------------------------------------------
# global function get_logger
# ------------------------------------------------------------------------------
def get_logger(modulename):
    """
    Get a logger from the python logging module and return to the caller
    :param modulename: string that names the module, prefix to the log file, and string in every log line
    :return: a logger obtained from the python logging module
    >>> the_logger = get_logger( "theModuleName" )
    >>> the_logger.debug('debug message')
    >>> the_logger.info('debug message')
    >>> the_logger.warn('debug message')
    >>> the_logger.error('debug message')
    >>> the_logger.critical('debug message')
    """
    logger = logging.getLogger(modulename)

    return logger

# ------------------------------------------------------------------------------
# Non-importable Internal class _LogStream
# ------------------------------------------------------------------------------
class _LogStream(object):
    """
    Internal class to log setup module. It is a standard python stream.
    1.  Redirect stdout and stderr to itself
    2.  In it's own write method, write to stdout and to a file created with hardcoded name
    Purpose is to be installed as output stream of the logger. This combines print
    and logs in a single output.
    """
    def __init__(self, dir = '.', prefix = 'log'):
        self._logfile_name = None
        self.log_path_filename = None
        self._stderr = None
        self._stdout = None
        self.open(dir)
   
    # FIX ME: close() and del() implementations
    def __del__(self):
        self._stdout = None
        self._stderr = None

        if self._file != None:
            self._file.close()
            self._file = None

    def __enter__(self):
        pass

    def __exit__(self, *args):
        pass

    def isatty(self):
        pass

    def open(self, dir = '.', prefix = 'log'):
        # save the system stdout and stderr so it can be restored on destruction
        self._stdout = sys.stdout
        self._stderr = sys.stderr

        # Open file to write logs into
        self._logfile_name = datetime.datetime.strftime(datetime.datetime.now(), prefix + '-%Y-%m-%d-%H_%M_%S_%f.txt')
        dir = os.path.normpath(dir)
        self.log_path_filename = dir + os.sep + self._logfile_name
        self._file = open(self.log_path_filename, "w")
        time.sleep(1) #delay for windows systems to handle the creation of the file

        # Replace the system stdout and stderr
        sys.stdout = self
        sys.stderr = self

    def write(self, message):
        self._stdout.write(message)
        # keep the termcolor ansi codes out of the log file
        message = re.sub('\\x1b\[\d+m', '', message)
        # remove the null character before writing to the file,
        # since this can stop the message from being printed on linux
        message = re.sub('[\\x00]', '', message)
        self._file.write(message)

    def flush(self):
        if self._stdout is not None:
            self._stdout.flush()
        if self._file is not None:
            self._file.flush()
            os.fsync(self._file.fileno())

    def close(self):
        if self._stdout != None:
            sys.stdout = self._stdout
            sys.stderr = self._stderr
            self._stdout = None
            self._stderr = None

        if self._file != None:
            self._file.close()
            self._file = None

# ------------------------------------------------------------------------------
# Code that gets run when this module is imported - module setup code
# ------------------------------------------------------------------------------
# Construct logstream to redirect stdout and stderr, and give to logging
_log_stream = _LogStream(dir=os.environ.get("BLOGSETUP_DIR", "."))
_log_format = '%(asctime)s [%(name)s:%(lineno)d]: %(levelname)-5s: %(message)s'
logging.basicConfig( format=_log_format, stream=_log_stream, level=logging.DEBUG )

"""
Flush the logger can call at critial locations in the code so that something is
in the log before a extern event kills/terminates your process
"""
def flush_logger():
    _log_stream.flush()


"""
Reinitialize the logger. Basically closes the output stream and re-opens it. The idea is
to allow the test runner to close one log session and open a new one, resulting in a
different log output file.

The reason this is needed is that the infinite while loop in python that re-runs
all tests on a 24/7 station runs in the same virtual machine instance as the previous
run. The global "logging" setting does not therefore automatically get reset.

Originally, without this, the assumption was made that the global logger gets re-initialized
because the whole virtual machine gets re-started between test runs. But clearly this
is not the case.

:param: none
:return: none
>>> TestBLogSetup.reinit_logger()
"""
def reinit_logger(dir = '.', prefix = 'log'):
    _log_stream.close()
    _log_stream.open(dir, prefix)


def get_log_path_filename():
    """
    Tests need the logfile name if they wish to include it in an email, as LogStream is implemented to be
    private, we need a 'getter' for the filename
    :return: the log filename and path
    """
    return _log_stream.log_path_filename

# # ------------------------------------------------------------------------------
# # Unit Test.
# # TODO: use unittest framework to actually verify the results of the log
# # ------------------------------------------------------------------------------
# import time
# class TestBLogSetup( unittest.TestCase ):
#     def testLog(self):
#         print( "SOMETHING NICE")
#         the_logger = get_logger( self.testLog.__name__ )
#         the_logger.debug('debug message')
#         the_logger.info('info message')
#         the_logger.warn('warning message')
#         the_logger.error('error message')
#         the_logger.critical('critical message')
#         self.assertEqual('foo'.upper(), 'FOO') # success
#
#         time.sleep(1) # ensure time changes so logfile changes
#         reinit_logger()
#         print( "SOMETHING NICE")
#         the_logger = get_logger( self.testLog.__name__ )
#         the_logger.debug('debug message')
#         the_logger.info('info message')
#         the_logger.warn('warning message')
#         the_logger.error('error message')
#         the_logger.critical('critical message')
#         self.assertEqual('foo'.upper(), 'FOO') # success
#         self.assertEqual('foo'.upper(), 'FOO1') # fail
#
#
# if __name__ == '__main__':
#     unittest.main()
