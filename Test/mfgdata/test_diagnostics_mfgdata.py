# test_audit_mfgdata.py
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168

"""
test_audit_mfgdata.py file checks the Eddie diagnostics on the webpage and compares them with the Device under test (dut)
"""
import logging

from diagnostics_mfgdata_page import DiagnosticsPage
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

logger = get_logger(__name__, "DiagnosticsPage.log", level=logging.DEBUG, fileLoglevel=logging.DEBUG)

def test_diagnostics_mfgdata(driver, device, device_ip):
    """
    Test will naviagate to Device's diagnostics page and verfies
    information with the device under test
    param driver: Selenium Webdriver
    param device: Fixture to get device from command line
    param device_ip: Fixture to get device_ip
    """
    try:
        diagnostics_page = DiagnosticsPage(driver, logger=logger, device=device)
        # navigate to Diagnostics Page URL
        diagnostics_page.navigate_diagnostics_page(device_ip)
        # Check for the webpage title
        diagnostics_page.get_title()
        # Check for Page Header
        diagnostics_page.get_diagnostics_header(device)
        # Check for the Software Version on Diagnostics Page
        diagnostics_page.get_sofware_version(device)
        # Check for the Display Tests Link on Diagnostics Page
        diagnostics_page.get_display_tests(device)
        # Check for the Manufacturing data on Diagnostics Page
        diagnostics_page.get_manufacturing_data(device)
    finally:
        driver.quit()
