# test_language.py
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
Automated Test to set and verify different languages for Eddie.
"""

import pytest
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils import rivieraUtils, device_utils
from CastleTestUtils.PassportUtils.passport_api import PassportAPIUsers
from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI

LOGGER = get_logger(__file__)

# cloudsync message
UPDATE = ['ALL']
# invalid language code
INVALID_LANGUAGE_CODES = ["abc", "ty", "su", "sr", "en-US", "sa"]
DEFAULT_LANGUAGE = "en"


def supported_language_codes():
    """
    This function gets all supported languages from productSettings.json file on product.

    :return languages: supported languages
    """
    device_id = pytest.config.getoption('--device-id')
    adb = rivieraUtils.RivieraUtils('ADB', device=device_id)

    response_device = adb.get_product_settings()
    languages = response_device["ProductSettings"]["properties"]["supportedLanguages"]
    LOGGER.debug("Supported languages are \n %s", languages)

    return languages


def update_product_language(request, front_door, language_code):
    """
    Update product language using update_product passport utility

    :param request: Instance of request fixture
    :param front_door: Instance of FrontDoorAPI connection
    :param language_code: language code to update in product
    :return: updated language code on product
    """
    # Get the guid and type of product
    LOGGER.info("Getting the GUID and product type")
    sys_info = front_door.getInfo()
    product_type = sys_info["body"]["productType"]
    product_guid = sys_info["body"]["guid"]

    # Create passport account
    passport_base_url = request.config.getoption('--passport-base-url')
    api_key = request.config.getoption('--api-key')

    #pylint: disable=W0212
    passport_user = PassportAPIUsers(front_door._bosepersonID, api_key, front_door._access_token,
                                     passport_base_url, logger=LOGGER)

    # Update product with language to set
    response = passport_user.update_product(product_id=product_guid,
                                            product_type=product_type,
                                            language=language_code)
    # return language
    return response["settings"]["language"]


@pytest.mark.usefixtures("request", "ip_address_wlan", "frontdoor_wlan")
@pytest.mark.parametrize("language", supported_language_codes())
def test_valid_language_set(request, frontdoor_wlan, language):
    """
    Set and verify different languages on Eddie

    Test Steps:
    1 Set the language using passport utility
    2 Perform cloud sync using /cloudSync frontdoor api
    3 Verify the language is set through get /system/language api

    :param request: A request for a fixture from a test or fixture function
    :param frontdoor_wlan: Instance of frontDoor
    :param language: parameterize language value of all supported languages
    """
    LOGGER.info("Language to set for Eddie is %s", language)

    # Update product with language
    response = update_product_language(request, frontdoor_wlan, language)
    LOGGER.debug("Language %s is set on product", response)

    # Verify language is updated on product
    assert response == language, \
        "Language {0} not updated on product language is {1}".format(language, response)

    # Perform cloudsync
    LOGGER.debug("Performing cloudsync on product")
    message = '{"update":' + str(UPDATE).replace("'", "\"") + '}'
    output = frontdoor_wlan.cloudSync(message)

    # Verify cloudsync is performed
    assert output["body"]["update"] == UPDATE, "Failed to perform cloud sync."

    # Verify language is set using get api
    output_language = frontdoor_wlan.getLanguage()
    assert output_language == language, \
        "Language {0} not set on eddie, language is {1}".format(language, output_language)


@pytest.mark.usefixtures("request", "ip_address_wlan", "frontdoor_wlan")
@pytest.mark.parametrize("invalid_language", INVALID_LANGUAGE_CODES)
def test_invalid_language_set(request, frontdoor_wlan, invalid_language):
    """
    Set and verify invalid languages on Eddie

    Test Steps:
    1 Get the language set on product
    2 Set the invalid language using passport utility
    3 Perform cloud sync using /cloudSync frontdoor api
    4 Verify the invalid language is not set through get /system/language api

    :param request: A request for a fixture from a test or fixture function
    :param frontdoor_wlan: Instance of frontDoor
    :param invalid_language: parameterize language value of not supported languages
    """
    LOGGER.info("Invalid Language to set for Eddie is %s", invalid_language)

    # Get the language code using get api
    language_code = frontdoor_wlan.getLanguage()
    LOGGER.debug("Language code on product is %s ", language_code)

    # Update product with invalid language code
    response = update_product_language(request, frontdoor_wlan, invalid_language)
    LOGGER.debug("Language code to update is %s ", response)

    # Perform cloudsync
    LOGGER.debug("Performing cloudsync on product")
    message = '{"update":' + str(UPDATE).replace("'", "\"") + '}'
    output = frontdoor_wlan.cloudSync(message)

    # Verify cloudsync is performed
    assert output["body"]["update"] == UPDATE, "Failed to perform cloud sync."

    # Verify language is not set using get api
    output_language = frontdoor_wlan.getLanguage()
    LOGGER.debug("Language code %s is set on product", output_language)

    assert output_language != invalid_language, \
        "Language {0} is set to invalid language {1}".format(output_language, invalid_language)


@pytest.mark.usefixtures("request", "wifi_config")
def test_default_language_set(request, wifi_config):
    """
    Set and verify invalid languages on Eddie

    Test Steps:
    1 Perform factory default on product
    2 Verify the default language is set through get /system/language api

    :param request: A request for a fixture from a test or fixture function
    :param wifi_config: Fixture for wifi configuration
    """
    LOGGER.info("Test to verify default %s language on product", DEFAULT_LANGUAGE)

    # Perform factory default
    device_id = request.config.getoption('--device-id')
    adb = rivieraUtils.RivieraUtils('ADB', device=device_id)
    adb.perform_factory_default()

    # Create instance of frontdoor api
    galapagos_env = request.config.getoption("--galapagos-env")
    interface = request.config.getoption("--network-iface")

    router = request.config.getoption("--router")
    ssid = wifi_config.get(router, 'ssid')
    security = wifi_config.get(router, 'security')
    password = wifi_config.get(router, 'password')

    ip_address = device_utils.get_ip_address(device_id, interface, ssid, security, password)

    assert adb.wait_for_galapagos_activation()
    frontdoor = FrontDoorAPI(ip_address, logger=LOGGER, env=galapagos_env)

    # Verify default language is set using get api
    output_language = frontdoor.getLanguage()
    LOGGER.debug("Default Language code set on product is %s ", output_language)

    assert output_language == DEFAULT_LANGUAGE, \
        "Default Language {0} not set on eddie, language set is {1}".format(DEFAULT_LANGUAGE, output_language)
