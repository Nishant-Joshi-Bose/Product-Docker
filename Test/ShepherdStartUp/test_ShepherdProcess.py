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
import os
import sys
import logging
import ConfigParser

from CastleTestUtils.RivieraUtils.rivieraUtils import RivieraUtils
from CastleTestUtils.SoftwareUpdateUtils.BonjourUpdateScripts.bonjourUpdate import BonjourUpdateUtils
from CastleTestUtils.SoftwareUpdateUtils.BonjourUpdate.bonjourUpdateSupport import BonjourUpdateSupport
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

logger = get_logger(__name__, "ShepherdProcess.log", level=logging.INFO, fileLoglevel=logging.DEBUG)
cfg = ConfigParser.SafeConfigParser()
cfg.read("conf_Shepherd.ini")

def test_shepherd_process(request):
    """
    This is to validate all processes running after Bonjour Update
    We are continuously validating each process for 5 minutes
    Fetching All Processes list from Eddie : cat /var/run/shepherd/Shepherd*.xml
    Validate Process is Running from Eddie : cat /var/run/shepherd/pids
    """
    result = PerformBonjourUpdate(request)
    device = request.config.getoption("--device-id")
    assert result, "Bonjour Update Failed. Please see logs for more details"
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
    bonjourUpdateSupport = BonjourUpdateSupport(device=device, logger=logger)

    try:
        deviceIP = bonjourUpdateSupport.getDeviceIP()
        logger.info("Device IP : " + deviceIP)
        bonjour_util = BonjourUpdateUtils(device=device)
        #Need to perform Bonjour Update twice
        BonjourCnt = 0
        while True:
            bonjour_util.upload_zipfile(zip_file, deviceIP)
            bonjourUpdateSupport.confirmInstallationVersions()
            BonjourCnt += 1
            if BonjourCnt >= int(cfg.get('Settings', 'BONJOUR_UPDATE_LOOP')):
                break
        return True
    except Exception as e:
        logger.info("Exception in Bonjour Update .... " + str(e))
        return False
