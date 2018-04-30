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
from CastleTestUtils.SoftwareUpdateUtils.BonjourUpdate.seleniumUtils import SeleniumWrapper
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
    expected_page_title = CONFIG["expected_text"]["diagnostics_page_title"]
    diagnostics_diag = CONFIG["diagnostics_diag"]
    mfg_data_command = CONFIG["mfg_data_command"]
    fs_version_command = CONFIG["fs_version_command"]
    expected_usb_ip = CONFIG["expected_text"]["usb_ip_address"]
    # locators
    manufacturing_data_locator = CONFIG["locator"]["manufacturing_data_locator"]
    partial_link_locator = CONFIG["locator"]["partial_link_locator"]
    mfg_data_all = CONFIG["locator"]["manufacturing_data_all"]
    software_version_header = CONFIG["locator"]["software_version_header"]
    table_locator = CONFIG["locator"]["table_locator"]
    diagnostics_header = CONFIG["locator"]["diagnostics_header"]
    left_display_test = CONFIG["locator"]["left_display_test"]
    network_interface_list_command = CONFIG["network_interfaces_list_cmd"]

    def __init__(self, driver, logger, deviceid):
        """
        driver: Selenium Webdriver
        logger: Logger
        deviceid: Fixture to get device id from command line
        """
        SeleniumWrapper.__init__(driver)
        self.driver = driver
        self.logger = logger or get_logger(__name__, "DiagnosticsPage.log", level=logging.INFO, fileLoglevel=logging.DEBUG)
        self.adb = ADBCommunication()
        self.adb.setCommunicationDetail(deviceid)
        self.dev_logger = LogreadAdb(deviceid)
        self.riviera_utils = RivieraUtils('ADB', device=deviceid)
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


    def usb_ip_address(self, deviceid, request):
        """
        This fixture gets the device USB IP
        param: deviceid - Fixture to get device id from command line
        :return: device usb ip
        """
        usb_device_ip = None
        self.logger.info("usb_ip_address")
        if request.config.getoption("--target").lower() == 'device':
            network_base = NetworkBase(None, deviceid)
            usb_interface = request.config.getoption("--usb2-iface")
            usb_device_ip = network_base.check_inf_presence(usb_interface)
        return usb_device_ip
    
    def system_reboot(self, devicid, device_ip):
        self.adb.rebootDevice()
        ip_address = self.riviera_utils.waitUntilNetworkUp(device_ip)
        self.logger.info(ip_address)
        assert ip_address is True, "wlan0 does not have IP"

    def navigate_diagnostics_page(self, device_ip, deviceid, request):
        """
        This function will reboots system and puts Eddie into PTS mode and fetches usb2 IP address
        Navigate to the Diagnostics Page URL page using usb2 IP address
        param: device_ip - Fixture to get device ip from device id
        param: deviceid - Fixture to get device id from command line
        return: None
        """
        # Reboot the system
        self.system_reboot(deviceid, device_ip)

        # Enter into PTS Mode
        driver = self.driver
        self.pts_mode_keypress(device_ip)
        time.sleep(5)
        network_base = NetworkBase(None, deviceid)
        usb_interface = request.config.getoption("--usb2-iface")
        usb_ip_address = network_base.check_inf_presence(usb_interface)
        assert usb_ip_address == self.expected_usb_ip, \
                     'USB IP did not match. Found {}, expected {}'.format(usb_ip_address, self.expected_usb_ip)
        self.logger.info(usb_ip_address)

        # Navigate to the URL
        base_url = "http://" + usb_ip_address + ":" + "/" + str(self.diagnostics_diag)
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


    def get_diagnostics_header(self, deviceid):
        """
        This function will check the Diagnostics Page Header on the page
        param: deviceid - Fixture to get device id from command line
        return: diagnostics_header text on the page
        """
        self.logger.info("get_diagnostics_header")
        diagnostics_header = self.getElement(self.diagnostics_header, locatorType="css", timeout=180).text
        assert diagnostics_header == self.expected_page_header, \
                'Page Header did not match. Found {}, expected {}'.format(diagnostics_header, self.expected_page_header)
        self.logger.info("Header matches as expected - %s / %s", diagnostics_header, self.expected_page_header)
        return diagnostics_header


    def get_sofware_version(self, deviceid):
        """
        This function will check the software version on the diagnostics page
        compares /opt/Bose/etc/BoserVersion.json of the Device under test for the same
        param: deviceid - Fixture to get device id from command line
        return: software_version_header, software_version_build on the page
        """
        self.logger.info("get_sofware_version")
        software_version_info = self.getElement(self.software_version_header, locatorType="css")
        software_version_header, software_version_build = software_version_info.text.split('\n')
        software_version_dut = json.loads(self.adb.executeCommand(self.fs_version_command))
        self.logger.info(software_version_header)
        self.logger.info(software_version_build)
        self.logger.debug("Diagnostic's software_version_header is - %s and software_version_build is - %s", software_version_header, software_version_build)
        assert self.expected_software_header == software_version_header, \
                'Software Version Header did not match. Found {}, expected {}'.format(self.expected_software_header, software_version_header)
        assert software_version_build == software_version_dut["long"], \
                'Software Version Build did not match. Found {}, expected {}'.format(software_version_build, software_version_dut["long"])
        self.logger.info("Software version Build matches as expected - %s / %s", software_version_build, software_version_dut["long"])
        return software_version_header, software_version_build


    def get_display_tests(self, deviceid):
        """
        This function will check the Display Tests on the diagnostics page
        work as intended by checking the display on Device under Test
        param: deviceid - Fixture to get device id from command line
        return: None
        """
        self.logger.info("get_display_tests")
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

    def get_manufacturing_data(self, deviceid):
        """
        This function will check the Manufacturing data on the diagnostics page
        compares with persist/mfg_data.json of the Device under test (dut)
        param: deviceid - Fixture to get device id from command line
        return: manufacturing_data_diag on the page
        """
        self.logger.info("get_manufacturing_data")
        mfg_data_dut = json.loads(self.adb.executeCommand(self.mfg_data_command))
        html_data = self.getElement(self.table_locator, locatorType="css").get_attribute('innerHTML')
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


    def reboot_check_interface(self, deviceid, device_ip):
        """
        This function will reboot DUT and resets PTS mode.
        Confirms usb2 interface in network interface list
        param: device_ip - Fixture to get device ip from device id
        param: deviceid - Fixture to get device id from command line
        return: none
        """
        self.logger.info("reboot_check_interface")
        self.system_reboot(deviceid, device_ip)
        output = self.adb.executeCommand(self.network_interface_list_command)
        network_list = output.strip().replace('\r', '').split('\n')
        self.logger.info(network_list)
        assert 'usb2' not in network_list, "usb2 interface in the list"
