"""
Organization: BOSE CORPORATION
Copyright: COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
This program may not be reproduced, in whole or in part in any
form or any means whatsoever without the written permission of:
BOSE CORPORATION
The Mountain,
Framingham, MA 01701-9168

This file is to test Shephed Processes
"""

import time
import logging
import ConfigParser
import sys, errno
import pytest
import pytest_dependency

from CastleTestUtils.RivieraUtils.rivieraUtils import RivieraUtils
from CastleTestUtils.SoftwareUpdateUtils.BonjourUpdateScripts.bonjourUpdate import BonjourUpdateUtils
from CastleTestUtils.RivieraUtils.rivieraCommunication import ADBCommunication
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

logger = get_logger(__name__, "ShepherdProcess.log", level=logging.INFO, fileLoglevel=logging.DEBUG)
cfg = ConfigParser.SafeConfigParser()
cfg.read("conf_Shepherd.ini")

@pytest.mark.usefixtures('passport_user_with_device', 'user_details', 'environment', 'save_speaker_log')
@pytest.mark.dependency()
def test_bonjour_update(request, passport_user_with_device, user_details, environment, save_speaker_log):
    """
    This to perform Bonjour update twice to validate it install continuos build
    """
    device = request.config.getoption("--device-id")
    zip_file = request.config.getoption("--zipfile")
    bonjour_update_loop = request.config.getoption("--updatecnt")
    adb = ADBCommunication()
    adb.setCommunicationDetail(device)
    adb.rebootDevice_adb()
    #Need to put sleep to up all services after reboot
    time.sleep(30)
    BonjourCnt = 0
    while BonjourCnt < int(bonjour_update_loop):
        BonjourCnt = BonjourCnt + 1
        result = PerformBonjourUpdate(adb, zip_file, device, user_details["email"], user_details["password"], environment)
        assert result, "Bonjour Update Failed. Please see logs for more details"
        time.sleep(60)

@pytest.mark.dependency(depends=["test_bonjour_update"])
def test_shepherd_process(request, save_speaker_log):
    """
    This is to validate all processes running after Bonjour Update
    We are continuously validating each process for 5 minutes
    Fetching All Processes list from Eddie : cat /var/run/shepherd/Shepherd*.xml
    Validate Process is Running from Eddie : cat /var/run/shepherd/pids
    """
    device = request.config.getoption("--device-id")
    rivierapull = RivieraUtils('LOCAL')
    processes = rivierapull.getShepherdProcesses(device=device)
    logger.info("Actual Processes : " + str(processes))
    process_died_list = []
    startTime = time.time()
    timeout = int(cfg.get('Settings', 'TIME_OUT'))
    riviera = RivieraUtils('ADB', device=device)
    while (time.time() - startTime) <= timeout:
        runningprocess = riviera.communication.executeCommand("cat pids", cwd="/var/run/shepherd")
        logger.info("Running Processes : " + str(runningprocess))
        for process in processes:
            if process not in runningprocess:
                logger.info("------------Process died : " + str(process) + "---------------")
                if process not in process_died_list:
                    process_died_list.append(process)
        time.sleep(2)
    assert process_died_list == [], "The following processes died after installation: {}".format(process_died_list)
    
    #Added Product boot status
    res = riviera.communication.executeCommand('echo product boot_status | nc 0 17000')
    logger.info("Product boot_status : %s", res)
    if 'false' in res:
        assert False, "Device is in booting state"

def PerformBonjourUpdate(adb, zip_file, device, email, password, environment):
    """
    Perform Bonjour Update twice on same build to validate software update
    """
    starttime = time.time()
    _timeout = 60
    try:
        while(time.time() - starttime) < _timeout:
            try:
                device_ip_address = adb.getIPAddress()
                logger.info("Device IP : " + device_ip_address)
                break
            except Exception as error:
                logger.info("Getting IP Address.... " + str(error))
                continue
        bonjour_util = BonjourUpdateUtils(device=device, email=email, password=password, environment=environment)
        try:
            bonjour_util.upload_zipfile(zip_file, device_ip_address)
        except SystemExit as error:
            logger.info("System Exit Exception in Bonjour Update .... " + str(error))
            if str(error) == str(errno.ETIMEDOUT):
                logger.info("System Exit Exception in Bonjour Update for TimeOut ...")
                return False
            elif str(error) == str(errno.ENOPKG):
                logger.info("Exception in Bonjour Update and Device is in critical error state ...")
                return False
            elif str(error) == str(errno.EBADF):
                logger.info("Exception in Bonjour Update and Unexpected State - Exiting Bonjour Update ...")
                return False
            return True
    except Exception as error:
        logger.info("Exception in Bonjour Update .... " + str(error))
        return False
