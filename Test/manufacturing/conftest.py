# manufacturing/conftest.py
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
PyTest Configuration & Fixtures for the Eddie Manufacturing tests.
"""
# pylint: disable=invalid-name

import json
import os
import requests

import pytest
from bs4 import BeautifulSoup
from selenium import webdriver
from selenium.webdriver.chrome.options import Options


from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

LOGGER = get_logger(__name__)


@pytest.mark.usefixtures('riviera')
@pytest.fixture(scope='function')
def riviera_service_enabled(riviera):
    """
    Sets the Service Flag on the Riviera Device

    :param riviera: Riviera Device Object
    :return: A Riviera Device with Service Manufacturing flag enabled
    """
    riviera.communication.executeCommand('/opt/Bose/bin/rw')

    output = riviera.communication.executeCommand('/usr/bin/mfg_data_set service true').strip()
    assert 'pass' in output, "Setting service did not pass: {}".format(output)

    output = riviera.communication.executeCommand('/usr/bin/mfgdata get service').strip()
    assert output == 'true', "Manufacturing service flag is not true: {}".format(output)

    yield riviera

    # Delete the Servie Flag at the end of the test, if it is not already gone
    riviera.communication.executeCommand('/opt/Bose/bin/rw')
    riviera.communication.executeCommand('/usr/bin/mfgdata del service')
    riviera.communication.executeCommand('/opt/Bose/bin/rw -r')


@pytest.mark.usefixtures('riviera_service_enabled')
@pytest.fixture(scope='function')
def validated_riviera_service_enabled(riviera_service_enabled):
    """
    Riviera object that has its Manufacturing Data validated after service is enabled

    :param riviera_service_enabled: A Riviera device that has its service
                                    manufacturing flag enabled
    :return: Riviera Object with validated manufacturing data with service
    """
    output = riviera_service_enabled.communication.executeCommand('/opt/Bose/bin/validate-mfgdata')
    assert output is None, "Please fix Manufacturing Data to continue."

    yield riviera_service_enabled


@pytest.mark.usefixtures('riviera')
@pytest.fixture(scope='function')
def riviera_no_service(riviera):
    """
    Ensures that the Riviera Device does not have the service flag set

    :param riviera: Riviera Device Object
    :return: Riviera device without service manufacturing flag
    """
    riviera.communication.executeCommand('/opt/Bose/bin/rw')
    riviera.communication.executeCommand('/usr/bin/mfgdata del service')
    riviera.communication.executeCommand('/opt/Bose/bin/rw -r')

    yield riviera


@pytest.fixture(scope='function')
@pytest.mark.usefixtures('ip_address_wlan', 'riviera_service_enabled')
def base_service_page(ip_address_wlan):
    """
    A BeautifulSoup rendered service page.

    :param ip_address_wlan: The IP Address of the Device connected over Wifi
    :return: Beautiful Soup base service page
    """
    page_address = 'http://{}/service'.format(ip_address_wlan)
    page = requests.get(page_address)
    soup = BeautifulSoup(page.text, 'html.parser')

    yield soup


@pytest.fixture(scope='module')
def driver():
    """
    Selenium WebDriver

    :return: A Selenium WebDriver fixture based upon Headless Chrome
    """
    chromedriver = '/usr/bin/chromedriver'
    if not os.path.isfile(chromedriver):
        pytest.fail("chromedriver not found.")

    options = Options()
    options.add_argument('headless')
    chrome = webdriver.Chrome(chrome_options=options, executable_path=chromedriver)
    yield chrome

    chrome.close()


@pytest.fixture(scope='function')
@pytest.mark.usefixtures('riviera_service_enabled')
def manufacturing_data(riviera_service_enabled):
    """
    Acquires the current manufacturing data from the device

    :param riviera_service_enabled: A Riviera device that has its service
                                    manufacturing flag enabled
    :return: Dictionary of Manfacturing Data.
    """
    mfg_data = riviera_service_enabled.communication.executeCommand("cat /persist/mfg_data.json")

    yield json.loads(mfg_data)
