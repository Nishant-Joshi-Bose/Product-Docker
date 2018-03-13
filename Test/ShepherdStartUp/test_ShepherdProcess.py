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
import pytest
import pytest_dependency
from CastleTestUtils.RivieraUtils.rivieraUtils import RivieraUtils
from CastleTestUtils.SoftwareUpdateUtils.BonjourUpdateScripts.bonjourUpdate import BonjourUpdateUtils
from CastleTestUtils.SoftwareUpdateUtils.BonjourUpdate.bonjourUpdateSupport import BonjourUpdateSupport
from CastleTestUtils.RivieraUtils.rivieraCommunication import ADBCommunication
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

logger = get_logger(__name__, "ShepherdProcess.log", level=logging.INFO, fileLoglevel=logging.DEBUG)
cfg = ConfigParser.SafeConfigParser()
cfg.read("conf_Shepherd.ini")

@pytest.mark.dependency()
def test_bonjour_update(request):
    """
    This to perform Bonjour update twice to validate it install continuos build
    """
    BonjourCnt = 0
    while BonjourCnt < int(cfg.get('Settings', 'BONJOUR_UPDATE_LOOP')):
        BonjourCnt = BonjourCnt + 1
        result = PerformBonjourUpdateTest(request)
        assert result, "Bonjour Update Failed. Please see logs for more details"
        time.sleep(120)
        #bonjourUpdateSupport = BonjourUpdateSupport(device=device, logger=logger)
        #bonjourUpdateSupport.confirm_installation_versions()
        

@pytest.mark.dependency(depends=["test_bonjour_update"])
def test_shepherd_process(request):
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
    result = True
    riviera = RivieraUtils('ADB', device=device)
    while (time.time() - startTime) <= timeout:
        runningprocess = riviera.communication.executeCommand("cat pids", cwd="/var/run/shepherd")
        logger.info("Running Processes : " + str(runningprocess))
        for process in processes:
            if process not in runningprocess:
                logger.info("------------Process died : " + str(process) + "---------------")
                if process not in process_died_list:
                    process_died_list.append(process)
                    result = False
        time.sleep(2)
    assert process_died_list == [], "The following processes died after installation: {}".format(process_died_list)

def PerformBonjourUpdate(request):
    """
    Perform Bonjour Update twice on same build to validate software update
    """
    device = request.config.getoption("--device-id")
    zip_file = request.config.getoption("--zipfile")
    adb = ADBCommunication()
    adb.setCommunicationDetail(device)
    bonjourUpdateSupport = BonjourUpdateSupport(device=device, logger=logger)
    starttime = time.time()
    _timeout = 60
    try:
        while(time.time() - starttime < _timeout):
            try:
                deviceIP = adb.getIPAddress()
                logger.info("Device IP : " + deviceIP)
                break
            except Exception as e:
                logger.info("Getting IP Address.... " + str(e))
                continue
        bonjour_util = BonjourUpdateUtils(device=device)
        #Need to perform Bonjour Update twice
        BonjourCnt = 0
        while True:
            try:
                bonjour_util.upload_zipfile(zip_file, deviceIP)
            except SystemExit as e:
                logger.info("System Exit Exception in Bonjour Update .... " + str(e))
                BonjourCnt += 1
                logger.info("Bonjour Update Cnt : " + str(BonjourCnt))                
                if BonjourCnt < int(cfg.get('Settings', 'BONJOUR_UPDATE_LOOP')):
                    time.sleep(120)
                    #bonjourUpdateSupport.confirm_installation_versions()
                    logger.info("Second Iteration.....")
                    continue
                else:
                    logger.info("Iteration Completed.....")
                    logger.info("Bonjour Update Cnt : " + str(BonjourCnt))
                    time.sleep(120)
                    #bonjourUpdateSupport.confirm_installation_versions()
                    break
        return True
    except Exception as e:
        logger.info("Exception in Bonjour Update .... " + str(e))
        return False

def PerformBonjourUpdateTest(request):
    """
    Perform Bonjour Update twice on same build to validate software update
    """
    device = request.config.getoption("--device-id")
    zip_file = request.config.getoption("--zipfile")
    adb = ADBCommunication()
    adb.setCommunicationDetail(device)
    starttime = time.time()
    _timeout = 60
    try:
        while(time.time() - starttime < _timeout):
            try:
                deviceIP = adb.getIPAddress()
                logger.info("Device IP : " + deviceIP)
                break
            except Exception as e:
                logger.info("Getting IP Address.... " + str(e))
                continue
        bonjour_util = BonjourUpdateUtils(device=device)
        try:
            bonjour_util.upload_zipfile(zip_file, deviceIP)
        except SystemExit as e:
            logger.info("System Exit Exception in Bonjour Update .... " + str(e))
            return True
    except Exception as e:
        logger.info("Exception in Bonjour Update .... " + str(e))
        return False
	
