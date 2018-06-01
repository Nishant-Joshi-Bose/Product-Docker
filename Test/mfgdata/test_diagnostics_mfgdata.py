# test_diagnostics_mfgdata.py
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
test_diagnostics_mfgdata.py file checks the Eddie diagnostics on the webpage and compares them with the Device under test (dut)
"""
import logging
import pytest

from diagnostics_mfgdata_page import DiagnosticsPage
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

logger = get_logger(__name__, "DiagnosticsPage.log", level=logging.DEBUG, fileLoglevel=logging.DEBUG)

@pytest.mark.usefixtures('driver', 'device_id', 'device_ip', 'force_rndis')
def test_diagnostics_mfgdata(driver, device_id, device_ip, request):
    """
    Navigates to DUT's diagnostics page and verfies the
    Device's information (e.g. Software Version, Manufacturing Data, Display Tests)
    param driver: Selenium Webdriver
    param device_id - Fixture to get device id from command line
    param device_ip: Fixture to get device_ip
    """
    try:
        diagnostics_page = DiagnosticsPage(driver, logger=logger, device_id=device_id)
        # navigate to Diagnostics Page URL
        diagnostics_page.navigate_diagnostics_page(device_ip, device_id, request)
        # Check for Page Header
        diagnostics_page.get_diagnostics_header()
        # Check for the Software Version on Diagnostics Page
        diagnostics_page.get_sofware_version()
        # Check for the LPM Version on Diagnostics Page
        diagnostics_page.get_lpm_data()
        # Check for the webpage title
        diagnostics_page.get_title()
        # Check for the Display Tests Link on Diagnostics Page
        diagnostics_page.get_display_tests()
        # Check for the Manufacturing data on Diagnostics Page
        diagnostics_page.get_manufacturing_data()
        # Unset PTS mode and check if usb interface exists
        diagnostics_page.reboot_check_interface(device_ip)
    finally:
        driver.quit()
        # This reboot is needed for the system to get out of PTS mode if the test
        # stops abruptly. IP needs to be rendered for the next test
        diagnostics_page = DiagnosticsPage(driver, logger=logger, device_id=device_id)
        diagnostics_page.system_reboot(device_ip)
