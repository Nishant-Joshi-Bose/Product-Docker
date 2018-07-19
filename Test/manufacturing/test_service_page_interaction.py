# manufacturing/test_service_page_interaction.py
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
Tests for Interacting Manufacturing Service Page through Selenium
"""
# pylint: disable=invalid-name
import pytest
from selenium.webdriver.common.by import By

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

LOGGER = get_logger(__file__)

# Modifiable Manufacturing items in the Service Page
service_items = ('BMAPVariantID', 'productColor', 'country_code',
                 'deviceAbsorptionLux', 'mic_gain', 'snSystem')


@pytest.mark.usefixtures('ip_address_wlan', 'riviera_service_enabled', 'driver')
def test_page_title(ip_address_wlan, driver):
    """
    Quick check to ensure there is a Service title and driver works correctly

    :param ip_address_wlan: The IP Address of the connected device
    :param driver: The Selenium WebDriver fixture
    """
    assert ip_address_wlan, "Without an IP Address, we cannot run test."

    driver.get('http://{}/service'.format(ip_address_wlan))
    assert "Service" in driver.title


@pytest.mark.usefixtures('ip_address_wlan', 'riviera_service_enabled', 'driver')
@pytest.mark.parametrize('service_item', service_items)
def test_service_change_items(ip_address_wlan, driver, service_item):
    """
    Ensure that we have the available fields to modify on the service page

    :param driver: The Selenium WebDriver fixture
    :param ip_address_wlan: The IP Address of the connected device
    :param service_item: The Manufacturing Data item that can be changed on the service page
    :param location: The Table Row location of the Service Item
    """
    assert ip_address_wlan, "Without an IP Address, we cannot run test."
    driver.get('http://{}/service'.format(ip_address_wlan))

    LOGGER.debug("Attempting to find %s in 'keys' table", service_item)

    # Acquire the Keys table
    keys_element = driver.find_element(By.ID, 'keys')
    # Determine first column in the keys table
    keys = keys_element.text.split('\n')
    key_items = [key.split()[0] for key in keys]

    assert service_item in key_items, \
        "Not able to find {} in keys table: {}".format(service_item, key_items)


@pytest.mark.usefixtures('ip_address_wlan', 'manufacturing_data', 'driver')
@pytest.mark.parametrize('service_item', service_items)
def test_mfg_data_correct(ip_address_wlan, manufacturing_data, driver, service_item):
    """
    Ensure default service item values match manufacturing data

    :param ip_address_wlan: The IP Address of the connected device
    :param manufacturing_data: Initially stored Manufacturing Data on the device
    :param driver: The Selenium WebDriver fixture
    :param service_item: The Manufacturing Data item that can be changed on the service page
    :param location: The Table Row location of the Service Item
    """
    assert ip_address_wlan, "Without an IP Address, we cannot run test."

    driver.get('http://{}/service'.format(ip_address_wlan))

    service_name = 'i_{}'.format(service_item)
    LOGGER.debug("Attempting to find %s at %s", service_item, service_name)

    item_input_element = driver.find_element(By.NAME, service_name)

    assert manufacturing_data[service_item] == item_input_element.get_attribute('value'), \
        "Value displayed does not match value stored."



@pytest.mark.usefixtures('ip_address_wlan', 'validated_riviera_service_enabled', 'driver')
def test_apply_no_modifications(ip_address_wlan, validated_riviera_service_enabled, driver):
    """
    Ensure that we can apply settings with success

    :param ip_address_wlan: The IP Address of the connected device
    :param validated_riviera_service_enabled: Riviera Object with validated
                                              manufacturing data with service
    :param driver: The Selenium WebDriver fixture
    """
    assert ip_address_wlan, "Without an IP Address, we cannot run test."

    driver.get('http://{}/service'.format(ip_address_wlan))

    # Click the Apply Button
    driver.find_element(By.ID, 'xapply').click()

    # Make sure we are still on the Service Page
    assert "Service" in driver.title
    # Make sure we get a Validation Successful
    validation_element = driver.find_element(By.ID, "validation_output")
    assert "Validation succeeded" in validation_element.text
    # Ensure Service Flag is removed
    service_status = validated_riviera_service_enabled.communication.executeCommand('/usr/bin/mfgdata get service')
    assert not service_status, "We are recieving a service status code: {}".format(service_status)
