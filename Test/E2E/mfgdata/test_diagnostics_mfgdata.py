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
from conf_diagnostics import CONFIG


logger = get_logger(__name__, "DiagnosticsPage.log", level=logging.DEBUG, fileLoglevel=logging.DEBUG)
expected_software_header = CONFIG["expected_text"]["software_version_text"]
expected_hash_text = CONFIG["expected_text"]["expected_hash_text"]

@pytest.mark.usefixtures('save_speaker_log', 'driver', 'device_id', 'device_ip', 'force_rndis')
def test_diagnostics_mfgdata(driver, device_id, device_ip, riviera, request):
    """
    Navigates to DUT's diagnostics page and verfies the
    Device's information (e.g. Software Version, Manufacturing Data, Display Tests)
    param save_speaker_log: fixture to capture speaker logs
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
        software_version_header, software_version_build, software_update_hash, software_version_dut = diagnostics_page.get_sofware_version()
        assert expected_software_header == software_version_header, \
                'Software Version Header did not match. Found {}, expected {}'.format(expected_software_header, software_version_header)
        assert software_version_build == software_version_dut["long"], \
                'Software Version Build did not match. Found {}, expected {}'.format(software_version_build, software_version_dut["long"])
        assert "unknown" not in software_update_hash, \
                'Software Version Text did not match. Found {}, expected {}'.format(software_update_hash.split()[6], expected_hash_text)
        # "Software Version string contains either 'production' or 'development'"
        logger.info("Software version Build matches as expected - %s / %s", software_version_build, software_version_dut["long"])
        # Check for the LPM Version on Diagnostics Page and compare with DUT
        lpm_version_dut = riviera.get_lpm_version()
        diagnostics_lpm_data = diagnostics_page.diagnostics_lpm_versions()
        assert lpm_version_dut["BLOB Version"].lstrip() == diagnostics_lpm_data["Blob Version"], \
                    'Blob Version did not match. Found {}, expected {}'.format(diagnostics_lpm_data["Blob Version"], lpm_version_dut["BLOB Version"].lstrip())
        assert lpm_version_dut["LPM Bootloader"].lstrip() == diagnostics_lpm_data["Bootloader Version"], \
                    'LPM Bootloader did not match. Found {}, expected {}'.format(diagnostics_lpm_data["Bootloader Version"], lpm_version_dut["LPM Bootloader"].lstrip())
        assert lpm_version_dut["LPM"].split(" ")[1] == diagnostics_lpm_data["User App Version"], \
                    'LPM did not match. Found {}, expected {}'.format(diagnostics_lpm_data["User App Version"], lpm_version_dut["LPM"].split(" ")[1])
        assert lpm_version_dut["PSoC"].lstrip() == diagnostics_lpm_data["PSOC Version"], \
                    'PSoC did not match. Found {}, expected {}'.format(diagnostics_lpm_data["PSOC Version"], lpm_version_dut["PSoC"].lstrip())
        assert lpm_version_dut["Lightbar Animation DB"].lstrip() == diagnostics_lpm_data["Lightbar Version"], \
                    'Lightbar Animation DB did not match. Found {}, expected {}'.format(diagnostics_lpm_data["Lightbar Version"], lpm_version_dut["Lightbar Animation DB"].lstrip())
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
