#!/usr/bin/python2.7
# -*- coding: utf-8 -*-
#
# :Organization: BOSE CORPORATION
#
# :Copyright: COPYRIGHT 2015 BOSE CORPORATION ALL RIGHTS RESERVED.
# This program may not be reproduced, in whole or in part in any
# form or any means whatsoever without the written permission of:
# BOSE CORPORATION
# The Mountain,
# Framingham, MA 01701-9168
#

author = 'kb1010257'
docformat = 'restructuredtext en'

import sys
import re
import os
import glob
import shutil
import argparse
import tempfile
import time

from datetime import datetime
from collections import defaultdict


# dict to hold ran/failed/error values in lists
log_matches = defaultdict(list)
# another to hold logs that may be kept and compressed
log_status = defaultdict(list)


def log_type_filter(logs):
    """
     filter the logs types from archive processing

     TODO:
     add regex to param list

    :param logs: tuple (log_type, [list of log files])
    :return: True only if log type matches test
    """
    if re.search('failed|missing|bad|success', logs[0]):
        return True


def build_log_list(log_dir, log_pattern, sort_desc=False):
    """
    here we use the built-in sorted method on the list generated
    and reference the tuple element right within the list comprehension

    :param log_dir: where to look for logs
    :param log_pattern: the pattern in the log file NAME of which to search for. So if log_pattern==*log*
    then any file found in log_dir with 'log' anywhere in its name will be on the list.
    :param sort_desc: set to True, not 'True' to return logs newest first
    :return: sorted list of absolute path log files with names including log_pattern
    """
    abs_log_dir = os.path.abspath(log_dir)
    if os.path.exists(abs_log_dir):
        log_path_pattern = os.path.join(abs_log_dir, log_pattern)
        if isinstance(sort_desc, bool):
            r_log_files = [x[1] for x in
                           sorted([(os.stat(i).st_mtime, i) for i in glob.glob(log_path_pattern)], reverse=sort_desc)]
        else:
            raise TypeError('sort_desc is not boolean type')
    else:
        raise IOError('log_dir does not exist')

    return r_log_files

def remove_logs_over_time(log_list, days_old=14):
    """
    removes the logs from log_list that are days_old and older
    This will REMOVE the logs. If you want to archive use: archive_logs
    :param log_list: list of logs as returned from build_log_list which is the absolute path and file name.
    :param days_old: logs this number of days old and older will be on the returned list
    :return:
    """
    now = time.time()
    for log in log_list:
        if os.stat(log).st_mtime < (now - days_old * 86400):
            try:
                print("Removing: {}".format(log))
                os.remove(log)
            except Exception as e:
                print("Unable to remove {0} because: {1}".format(log, e))
                continue


def build_log_statuses(p_log_files, success_keep=1, fails_keep=5):
    """

    parse each log file and assign it a status based on rules

    TODO:
    add regex pattern as param
    once log status rules are clearly defined refactor logic into own method


    :param p_log_files: list of log files to process
    :param fails_keep: how many recent logs with failures to keep
    :return: dictionary { log_type: [log_files] }
    """
    # handy shortcut true/false testing. empty list will eval to False
    # see https://docs.python.org/2/library/stdtypes.html#truth-value-testing
    if not p_log_files:
        print 'File folder specified does not contain log*.txt files.'
    else:
        # read each file into memory, then read each line of file in reverse
        # to find metrics faster. metrics are stored in log_matches dictionary
        # for example log_matches['ran'] will store list of all ran metrics
        # encountered in the log file. same for errors and failures
        for l_file in p_log_files:
            print 'Processing Log File: {}'.format(l_file)
            with open(l_file, 'r') as log_file:
                log_lines = log_file.readlines()
                for line_index in xrange(len(log_lines) - 1, 0, -1):
                    # capture all we need from line in named groups
                    # TODO see comment todo! Before using this method it needs a more generic search option
                    log_match = \
                        re.search('ran:\s(?P<ran>\d+).+failed:\s(?P<failed>\d+).+errors:\s(?P<errors>\d+)',
                                  log_lines[line_index], re.IGNORECASE)
                    if log_match:
                        # when we find a match, iterate over matched keys/values
                        # and add values to aggregate dictionary
                        for k, v in log_match.groupdict().items():
                            log_matches[k].append(int(v))
                    # move onto next file if we have already found unit/grand total metrics
                    if len(log_matches['ran']) == 2:
                        break

            # file is closed  here, but still have access to data in log_lines
            # log status rules begin here. as rules become better understood
            # they can be refactored into own method so that the rules are
            # applied to each log. eventually we will be able to apply rules
            # to logs dynamically.
            # if not log_matches['ran'] and not log_matches['failed'] and not log_matches['errors']:
            if not log_matches['ran']:
                print 'Error: {} has no Test Metrics, File will be archived'.format(l_file)
                log_status['bad'].append(l_file)
                continue

            if log_matches.has_key('ran') and log_matches.has_key('failed') and log_matches.has_key('errors'):
                if len(log_matches['ran']) != 2 or \
                        len(log_matches['failed']) != 2 or \
                        len(log_matches['errors']) != 2:
                    print \
                        'Error: {}: has missing Test Metrics. Check File. It will be archived.'.format(l_file)
                    log_status['missing'].append(l_file)
                else:
                    # use the unittest runs metrics reading file backwards in memory
                    log_errors = log_matches['errors'][1]
                    log_failures = log_matches['failed'][1]

                    if log_errors + log_failures == 0:
                        print 'Found 100% success! for {}. Keeping file...'.format(l_file)
                        log_status['success'].append(l_file)

                    if log_errors + log_failures != 0:
                        print ' Log: {} has failures. It will be archived'.format(l_file)
                        log_status['failed'].append(l_file)

            # must empty aggregate match dictionary after each file processed
            log_matches.clear()

        print '\nKeeping {} most recent logs with Failures\n'.format(fails_keep)
        for log_fail in log_status['failed'][0:fails_keep]:
            print log_fail
        # todo log status line fix
        log_status['success'] = log_status['success'][int(success_keep):]

    return log_status


def archive_logs(p_log_status):

    """
    pythonic way to flatten list of lists
    If filter returns True, then a tuple of (type, [files])
    is added to list built by list comprehension.
    Then we extract just files from tuple ' f for (t, f)'
    but since filter is called on each dictionary key, we end up  with
    a list of lists with files passing the filter test.
    The 'for f in f' is run for each returned file sublist and the
    end result is a single list of files.

    Simple Example:
    note how the z value is different after each list comprehension statement.

    >>> a = [[1,2],[3,4]]
    >>> a
    [[1, 2], [3, 4]]
    >>> [z for z in a]
    [[1, 2], [3, 4]]
    >>> z
    [3, 4]
    >>> [z for z in a for z in z]
    [1, 2, 3, 4]
    >>> z
    4

    :param p_log_status: dictionary of log files and their statuses
                         {'status': [list of logs] }
    :return:  nothing
    """

    if not p_log_status['failed'] and not p_log_status['success']:
        print 'No files to Archive'
        sys.exit(1)

    temp_dir = tempfile.mkdtemp()
    (t_file_handle, t_file_path) = tempfile.mkstemp(dir=temp_dir)

    temp_files = [f for (t, f) in filter(log_type_filter, p_log_status.items()) for f in f]
    for t_file in temp_files:
        shutil.move(os.path.abspath(t_file), temp_dir)

    try:
        print '\nMaking Archive in: {}'.format(temp_dir)
        shutil.make_archive(t_file_path, 'bztar', root_dir=temp_dir)

        archive_file = t_file_path + '.tar.bz2'
        archive_file_copy = 'Logs_' + os.path.basename(os.getcwd() + '_' + datetime.fromtimestamp(
            os.stat(t_file_path + '.tar.bz2')[-1]).strftime('%Y_%m_%d_%H_%M') + '.tar.bz2')

        print 'Copying Archive to current directory: {}'.format(os.getcwd())
        shutil.copy(os.path.abspath(archive_file), archive_file_copy)

        print '\nArchive: {} created. \nAll logs were moved to: {}'.\
            format(os.path.abspath(archive_file_copy), temp_dir)
    except IOError as ioe:
        print ioe.message


if __name__ == '__main__':

    parser = argparse.ArgumentParser()

    parser.add_argument(
        '-ldir', nargs='?',
        default=os.getcwd(),
        const=os.getcwd(),
        help='path where logs are located. use current directory if not provided')

    parser.add_argument(
        '-sk', nargs='?',
        default=0,
        const=1,
        help='how many success logs to keep. use 1 if not provided')

    parser.add_argument(
        '-fk', nargs='?',
        default=0,
        const=5,
        help='how many failure logs to keep. use 5 if not provided (use "all" to save all failure logs)')

    args = parser.parse_args()

    log_files = build_log_list(args.ldir, '*log*.txt', True)
    log_status = build_log_statuses(log_files, success_keep=args.sk, fails_keep=args.fk)
    remove_logs_over_time(log_files, 0)
    #archive_logs(log_status)

