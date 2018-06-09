# - *- coding: utf- 8 - *
# diagnostics_page.py
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168
#
"""
diagnostics_page.py has helper functions related to Page Elements and Locators
"""
import os
import time
import logging
import json
import pexpect

from selenium import webdriver
from BeautifulSoup import BeautifulSoup
from CastleTestUtils.SeleniumUtils.seleniumUtils import SeleniumWrapper
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils.rivieraCommunication import ADBCommunication
from CastleTestUtils.LoggerUtils.logreadAdb import LogreadAdb
from CastleTestUtils.RivieraUtils.hardware.keys.keys import Keys
from CastleTestUtils.RivieraUtils.hardware.keys import keypress
from CastleTestUtils.NetworkUtils.network_base import NetworkBase
from CastleTestUtils.RivieraUtils.rivieraUtils import RivieraUtils
from conf_diagnostics import CONFIG

pts_combo_keys = [Keys.VOLUME_UP.value, Keys.VOLUME_DOWN.value]

class DiagnosticsPage(SeleniumWrapper):
    """
    Class methods for Diagnostics Page
    """
    expected_page_header = CONFIG["expected_text"]["main_header"]
    expected_software_header = CONFIG["expected_text"]["software_version_text"]
    expected_lpm_header = CONFIG["expected_text"]["lpm_version_text"]
    expected_page_title = CONFIG["expected_text"]["diagnostics_page_title"]
    diagnostics_diag = CONFIG["diagnostics_diag"]
    mfg_data_command = CONFIG["mfg_data_command"]
    fs_version_command = CONFIG["fs_version_command"]
    lpm_version_command = CONFIG["lpm_version_command"]
    expected_rndis_ip = CONFIG["expected_text"]["rndis1_ip_address"]
    # locators
    manufacturing_data_locator = CONFIG["locator"]["manufacturing_data_locator"]
    partial_link_locator = CONFIG["locator"]["partial_link_locator"]
    mfg_data_all = CONFIG["locator"]["manufacturing_data_all"]
    software_version_header = CONFIG["locator"]["software_version_header"]
    lpm_version_header = CONFIG["locator"]["lpm_version_header"]
    mfg_data_locator = CONFIG["locator"]["mfg_data_locator"]
    lpm_versions_locator = CONFIG["locator"]["lpm_versions_locator"]
    diagnostics_header = CONFIG["locator"]["diagnostics_header"]
    display_link_element = CONFIG["locator"]["display_link_element"]
    done_button = CONFIG["locator"]["done_button"]
    left_display_test = CONFIG["locator"]["left_display_test"]
    network_interface_list_command = CONFIG["network_interfaces_list_cmd"]

    def __init__(self, driver, logger, device_id):
        """
        driver: Selenium Webdriver
        logger: Logger
        device_id: Fixture to get device id from command line
        """
        self.selenium = SeleniumWrapper(driver, logger)
        self.driver = driver
        self.logger = logger or get_logger(__name__, "DiagnosticsPage.log", level=logging.INFO, fileLoglevel=logging.DEBUG)
        self.adb = ADBCommunication()
        self.adb.setCommunicationDetail(device_id)
        self.dev_logger = LogreadAdb(device_id)
        self.riviera_utils = RivieraUtils('ADB', device=device_id)
        self.dir_name = 'adb_logger'
        self.file_name = 'adb_logs'

    def pts_mode_keypress(self, device_ip):
        """
        This function will put Eddie into PTS mode
        param: device_ip - Fixture to get device ip from device id
        """
        self.logger.info("pts_mode_keypress")
        tap = pexpect.spawn("telnet %s 17000" % device_ip)
        keypress.key_press_only(tap, pts_combo_keys, async_response=True)


    def rndis1_ip_address(self, device_id, request):
        """
        This fixture gets the device rndis1 IP
        param: device_id - Fixture to get device id from command line
        :return: device rndis1 ip
        """
        rndis_interface_ip = None
        self.logger.info("rndis1_ip_address")
        if request.config.getoption("--target").lower() == 'device':
            network_base = NetworkBase(None, device_id)
            rndis_interface = request.config.getoption("--pts-iface")
            rndis_interface_ip = network_base.check_inf_presence(rndis_interface)
        return rndis_interface_ip

    def system_reboot(self, device_ip):
        """
        This fixture gets the device USB IP
        param: device_ip - Fixture to get device ip from device id
        """
        self.adb.rebootDevice()
        ip_address = self.riviera_utils.waitUntilNetworkUp(device_ip)
        self.logger.info(ip_address)
        assert ip_address is True, "wlan0 does not have IP"


    def navigate_diagnostics_page(self, device_ip, device_id, request):
        """
        This function will reboots system and puts Eddie into PTS mode and fetches rndis1 IP address
        Navigate to the Diagnostics Page URL page using rndis1 IP address
        param: device_ip - Fixture to get device ip from device id
        param: device_id - Fixture to get device id from command line
        return: None
        """
        # Reboot the system
        self.system_reboot(device_ip)

        # Enter into PTS Mode
        driver = self.driver
        self.pts_mode_keypress(device_ip)
        time.sleep(5)
        network_base = NetworkBase(None, device_id)
        rndis_interface = request.config.getoption("--pts-iface")
        rndis1_ip_address = network_base.check_inf_presence(rndis_interface)
        assert rndis1_ip_address == self.expected_rndis_ip, \
                     'rndis IP did not match. Found {}, expected {}'.format(rndis1_ip_address, self.expected_rndis_ip)
        self.logger.info(rndis1_ip_address)

        # Navigate to the URL
        base_url = "http://" + rndis1_ip_address + ":" + "/" + str(self.diagnostics_diag)
        driver.get(base_url)


    def get_title(self):
        """
        This function will check the webpage Title, converts the unicode characters
        return: website title
        """
        self.logger.info("get_title")
        website_title = self.driver.title
        assert website_title == self.expected_page_title, \
                'Website Title did not match. Found {}, expected {}'.format(website_title, self.expected_page_title)
        self.logger.info("Title matches as expected -  %s / %s", website_title, self.expected_page_title)
        return website_title


    def get_diagnostics_header(self):
        """
        This function will check the Diagnostics Page Header on the page
        return: diagnostics_header text on the page
        """
        self.logger.info("get_diagnostics_header")
        diagnostics_header = self.getElement(self.diagnostics_header, locatorType="css", timeout=180).text
        assert diagnostics_header == self.expected_page_header, \
                'Page Header did not match. Found {}, expected {}'.format(diagnostics_header, self.expected_page_header)
        self.logger.info("Header matches as expected - %s / %s", diagnostics_header, self.expected_page_header)
        return diagnostics_header


    def get_sofware_version(self):
        """
        This function will check the software version on the diagnostics page
        compares /opt/Bose/etc/BoserVersion.json of the Device under test for the same
        return: software_version_header, software_version_build on the page
        """
        self.logger.info("get_sofware_version")
        software_version_info = self.getElement(self.software_version_header, locatorType="css")
        software_version_header, software_version_build, software_update_hash = software_version_info.text.split('\n')
        software_version_dut = json.loads(self.adb.executeCommand(self.fs_version_command))
        self.logger.info(software_version_header)
        self.logger.info(software_version_build)
        self.logger.info(software_update_hash)
        self.logger.debug("Diagnostic's software_version_header is - %s and software_version_build is - %s", software_version_header, software_version_build)
        return software_version_header, software_version_build, software_version_dut

    def diagnostics_lpm_versions(self):
        """
        Will attempt to get the LPM version information from the page and return a
        dictionary with its values.
        """
        lpm_version_raw = self.getElement(self.lpm_version_header, locatorType="css")
        lpm_version_data = lpm_version_raw.text.splitlines()
        result = dict()
        for components in lpm_version_data:
            if ':' not in repr(components):
                lpm_version_data.remove(components)
        for elements in range(len(lpm_version_data)):
            value = lpm_version_data[elements]
            index = value.find(":")
            lpm_version_header = value[:index].strip()
            lpm_version = value[index+1:].strip()
            result[lpm_version_header] = lpm_version
        return result


    def get_display_tests(self):
        """
        This function will check the Display Tests on the diagnostics page
        work as intended by checking the display on Device under Test
        return: None
        """
        self.logger.info("get_display_tests")
        # Get the display link element
        self.getElement(self.display_link_element, locatorType="linktext").click()
        self.dev_logger.start_log_collection(self.file_name, path=self.dir_name)
        time.sleep(5)
        elements = self.getElements(self.left_display_test, locatorType="css")
        for element in elements:
            element.click()
            # Giving enough time to click the display-test images
            time.sleep(5)
        self.dev_logger.stop_log_collection(saveLog=True, zip=False)
        for f in os.listdir(self.dir_name):
            filename = os.path.join(self.dir_name, f)
            size = (os.path.getsize(filename))
            assert size >= 0
            with open(filename) as myfile:
                output = myfile.read()
                assert "white.png" in output, "white.png string did not match"
                assert "black.png" in output, "black.png string did not match"
                assert "red.png" in output, "red.png string did not match"
                assert "blue.png" in output, "blue.png string did not match"
                assert "green.png" in output, "green.png string did not match"
                assert "vertical.png" in output, "vertical.png string did not match"
                assert "horizontal.png" in output, "horizontal.png string did not match"
        # Delete the files once done checking for the string
        for item in os.listdir(self.dir_name):
            if item.endswith(".txt"):
                os.remove(os.path.join(self.dir_name, item))
        # Click the Done button
        self.driver.find_element_by_tag_name("button").click()


    def get_manufacturing_data(self):
        """
        This function will check the Manufacturing data on the diagnostics page
        compares with persist/mfg_data.json of the Device under test (dut)
        return: manufacturing_data_diag on the page
        """
        self.logger.info("get_manufacturing_data")
        mfg_data_dut = json.loads(self.adb.executeCommand(self.mfg_data_command))
        html_data = self.getElement(self.mfg_data_locator, locatorType="css").get_attribute('innerHTML')
        # Partial Manufacturing data displayed on the page
        mfg_partial_data = dict([[cell.text for cell in row("td")] for row in BeautifulSoup(html_data)("tr")])
        self.logger.info("Manufacturing Partial Data on the page are %s / %s", mfg_partial_data["productType"], mfg_data_dut["productType"])
        assert mfg_partial_data["country_code"] == mfg_data_dut["country_code"], \
                'Manufacturing Partial Data on the page did not match. Found {}, expected {}'.format(mfg_partial_data["country_code"], mfg_data_dut["country_code"])
         # Complete Manufacturing Data after you click "all" link on the page
        self.getElement(self.partial_link_locator, locatorType="linktext").click()
        mfg_data_page = self.getElement(self.mfg_data_all, locatorType="xpath").text
        mfg_data_page = json.loads(mfg_data_page)
        self.logger.debug("mfg_data_page %s and  mfg_data_dut are %s", mfg_data_dut, mfg_data_page)
        assert mfg_data_dut == mfg_data_page, \
                'Manufacturing Complete Data on the page did not match. Found {}, expected {}'.format(mfg_data_dut, mfg_data_page)
        self.logger.info("Manufacturiung Data matches as expected - %s / %s", mfg_data_dut, mfg_data_page)
        return mfg_data_page


    def reboot_check_interface(self, device_ip):
        """
        This function will reboot DUT and resets PTS mode.
        Confirms rndis1 interface in network interface list
        param: device_ip - Fixture to get device ip from device id
        return: none
        """
        self.logger.info("reboot_check_interface")
        self.system_reboot(device_ip)
        output = self.adb.executeCommand(self.network_interface_list_command)
        network_list = output.strip().replace('\r', '').split('\n')
        self.logger.info(network_list)
        assert 'rndis1' not in network_list, "rndis1 interface in the list"
