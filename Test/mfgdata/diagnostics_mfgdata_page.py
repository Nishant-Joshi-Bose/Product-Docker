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

from selenium import webdriver
from BeautifulSoup import BeautifulSoup
from CastleTestUtils.SoftwareUpdateUtils.BonjourUpdate.seleniumUtils import SeleniumWrapper
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils.rivieraCommunication import ADBCommunication
from CastleTestUtils.LoggerUtils.logreadAdb import LogreadAdb
from conf_diagnostics import CONFIG

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
    # locators
    manufacturing_data_locator = CONFIG["locator"]["manufacturing_data_locator"]
    partial_link_locator = CONFIG["locator"]["partial_link_locator"]
    mfg_data_all = CONFIG["locator"]["manufacturing_data_all"]
    software_version_header = CONFIG["locator"]["software_version_header"]
    table_locator = CONFIG["locator"]["table_locator"]
    diagnostics_header = CONFIG["locator"]["diagnostics_header"]
    left_display_test = CONFIG["locator"]["left_display_test"]

    def __init__(self, driver, logger, deviceid):
        """
        driver: Selenium Webdriver
        logger: Logger
        deviceid: Device information from command line
        """
        SeleniumWrapper.__init__(driver)
        self.driver = driver
        self.logger = logger or get_logger(__name__, "DiagnosticsPage.log", level=logging.INFO, fileLoglevel=logging.DEBUG)
        self.adb = ADBCommunication()
        self.adb.setCommunicationDetail(deviceid)
        self.dev_logger = LogreadAdb(deviceid)
        self.dir_name = 'adb_logger'
        self.file_name = 'adb_logs'


    def navigate_diagnostics_page(self, device_ip):
        """
        This function will navigate to the Diagnostics Page URL page
        param: device_ip - Fixture to get device ip from device id
        return: None
        """
        self.logger.info("------ Executing get_title function------")
        driver = self.driver
        self.logger.info(device_ip)
        base_url = "http://" + device_ip + ":" + "/" + str(self.diagnostics_diag)
        driver.get(base_url)


    def get_title(self):
        """
        This function will check the webpage Title, converts the unicode characters
        return: website title
        """
        self.logger.info("-------------- Executing get_title function -------------- ")
        website_title = self.driver.title
        assert website_title == self.expected_page_title, \
                'Website Title did not match. Found {}, expected {}'.format(website_title, self.expected_page_title)
        self.logger.info(" -----Title matches as expected---- %s / %s", website_title, self.expected_page_title)
        return website_title


    def get_diagnostics_header(self, deviceid):
        """
        This function will check the Diagnostics Page Header on the page
        param: deviceid - Fixture to get device id from command line
        return: diagnostics_header on the page
        """
        self.logger.info("-------------- Executing get_diagnostics_header function -------------- ")
        diagnostics_header = self.getElement(self.diagnostics_header, locatorType="css").text
        assert diagnostics_header == self.expected_page_header, \
                'Page Header did not match. Found {}, expected {}'.format(diagnostics_header, self.expected_page_header)
        self.logger.info(" -----Header matches as expected---- %s / %s", diagnostics_header, self.expected_page_header)
        return diagnostics_header


    def get_sofware_version(self, deviceid):
        """
        This function will check the software version on the diagnostics page
        compares /opt/Bose/etc/BoserVersion.json of the Device under test for the same
        param: deviceid - Fixture to get device id from command line
        return: software_version_header, software_version_build on the page
        """
        self.logger.info("-------------- Executing get_sofware_version function -------------- ")
        software_version_info = self.getElement(self.software_version_header, locatorType="css")
        software_version_header, software_version_build = software_version_info.text.split('\n')
        software_version_dut = json.loads(self.adb.executeCommand(self.fs_version_command))
        self.logger.debug("Diagnostic's software_version_header %s and  software_version_build are %s", software_version_header, software_version_build)
        assert self.expected_software_header == software_version_header, \
                'Software Version Header did not match. Found {}, expected {}'.format(self.expected_software_header, software_version_header)
        assert software_version_build == software_version_dut["long"], \
                'Software Version Build did not match. Found {}, expected {}'.format(software_version_build, software_version_dut["long"])
        self.logger.info(" -----Software version Build matches as expected---- %s / %s", software_version_build, software_version_dut["long"])
        return software_version_header, software_version_build


    def get_display_tests(self, deviceid):
        """
        This function will check the Display Tests on the diagnostics page
        work as intended by checking the display on Device under Test
        param: deviceid - Fixture to get device id from command line
        return: None
        """
        self.logger.info("-------------- Executing get_display_tests function -------------- ")
        self.dev_logger.start_log_collection(self.file_name, path=self.dir_name)
        time.sleep(5)
        elements = self.getElements(self.left_display_test, locatorType="css")
        for element in elements:
            element.click()
            time.sleep(0.1)
        self.dev_logger.stop_log_collection(saveLog=True, zip=False)
        for f in os.listdir(self.dir_name):
            filename = os.path.join(self.dir_name, f)
            size = (os.path.getsize(filename))
            assert size >= 0
            with open(filename) as myfile:
                output = myfile.read()
                assert "colorbars-top.png" in output, "colorbars-top.png string did not match"
                assert "colorbars-bottom.png" in output, "colorbars-bottom.png string did not match"
                assert "colorbars-right.png" in output, "colorbars-right.png string did not match"
                assert "colorbars-left.png" in output, "colorbars-left.png string did not match"


    def get_manufacturing_data(self, deviceid):
        """
        This function will check the Manufacturing data on the diagnostics page
        compares with persist/mfg_data.json of the Device under test (dut)
        param: deviceid - Fixture to get device id from command line
        return: manufacturing_data_diag on the page
        """
        self.logger.info("-------------- Executing get_manufacturing_data function -------------- ")
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
        self.logger.info(" ----- Manufacturiung Data matches as expected---- %s / %s", mfg_data_dut, mfg_data_page)
        return mfg_data_page
