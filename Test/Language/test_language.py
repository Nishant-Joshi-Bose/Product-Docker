# Language/test_language.py
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
import json
import pytest

from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.OAuthUtils.OAuthUtils import UserAccount
from CastleTestUtils.PassportUtils.passport_api import PassportAPIUsers
from CastleTestUtils.PassportUtils.passport_utils import get_passport_url
from CastleTestUtils.RivieraUtils import rivieraUtils, device_utils


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
    device_id = pytest.config.getoption('--device-id') # pylint: disable=E1101
    adb = rivieraUtils.RivieraUtils('ADB', device=device_id)

    response_device = adb.get_product_settings()
    languages = response_device["ProductSettings"]["properties"]["supportedLanguages"]
    LOGGER.debug("Supported languages: %s", languages)

    return languages


@pytest.mark.usefixtures('passport_user_with_device', 'frontdoor_wlan')
@pytest.mark.parametrize("language", supported_language_codes())
def test_valid_language_set(passport_user_with_device, frontdoor_wlan, language):
    """
    Set and verify different languages on Eddie

    Test Steps:
    1 Set the language using passport utility
    2 Perform cloud sync using /cloudSync frontdoor api
    3 Verify the language is set through get /system/language api

    :param passport_user_with_device: Passport user with device under test attached
    :param frontdoor_wlan: Instance of frontDoor
    :param language: parameterize language value of all supported languages
    """
    LOGGER.info("Setting Language to '%s'", language)
    # Update product with language to set
    sys_info = frontdoor_wlan.getInfo()
    language_info = passport_user_with_device.update_product(product_id=sys_info['body']['guid'],
                                            product_type=sys_info['body']['productType'],
                                            language=language)['settings']['language']

    LOGGER.debug("Passport Language set to '%s'", language_info)

    # Verify language is updated on product
    assert language_info == language, \
        "Passport Language ({}) not updated to {}.".format(language_info, language)

    # Perform cloudsync
    LOGGER.debug("Performing cloudsync on product")
    output = frontdoor_wlan.cloudSync(json.dumps({"update": UPDATE}, indent=4))

    # Verify cloudsync is performed
    assert output["body"]["update"] == UPDATE, "Failed to perform cloud sync. {}".format(output)

    # Verify language is set using get api
    output_language = frontdoor_wlan.getLanguage()
    assert output_language == language, \
        "Language {} not set on Eddie ({}).".format(language, output_language)


@pytest.mark.usefixtures('passport_user_with_device', 'frontdoor_wlan')
@pytest.mark.parametrize('invalid_language', INVALID_LANGUAGE_CODES)
def test_invalid_language_set(passport_user_with_device, frontdoor_wlan, invalid_language):
    """
    Set and verify invalid languages on Eddie

    Test Steps:
    1 Get the language set on product
    2 Set the invalid language using passport utility
    3 Perform cloud sync using /cloudSync frontdoor api
    4 Verify the invalid language is not set through get /system/language api

    :param passport_user_with_device: Passport user with device under test attached
    :param frontdoor_wlan: Instance of FrontDoor
    :param invalid_language: parameterize language value of not supported languages
    """
    LOGGER.info("Invalid Language to set for Eddie is %s", invalid_language)

    # Get the language code using get api
    language_code = frontdoor_wlan.getLanguage()
    LOGGER.debug("Current Language code on product is %s ", language_code)

    # Update product with invalid language code
    sys_info = frontdoor_wlan.getInfo()
    language_info = passport_user_with_device.update_product(product_id=sys_info['body']['guid'],
                                            product_type=sys_info['body']['productType'],
                                            language=invalid_language)['settings']['language']

    LOGGER.debug("Passport Language update to %s ", language_info)

    # Perform cloudsync
    LOGGER.debug("Performing cloudsync on product")
    output = frontdoor_wlan.cloudSync(json.dumps({"update": UPDATE}, indent=4))

    # Verify cloudsync is performed
    assert output["body"]["update"] == UPDATE, "Failed to perform cloud sync."

    # Verify language is not set using get api
    output_language = frontdoor_wlan.getLanguage()
    LOGGER.debug("Product Language is: %s", output_language)

    assert output_language != invalid_language, \
        "Product Language {} is set to invalid language".format(output_language)


@pytest.mark.usefixtures('factory_defaulted_device', 'frontdoor_wlan')
def test_default_language(frontdoor_wlan):
    """
    Verify default initial languages on factory defaulted Eddie

    Verification is through the /system/language API.

    :param frontdoor_wlan: Instance of frontdoor
    """
    LOGGER.info("Verifying default language (%s) on device.", DEFAULT_LANGUAGE)

    # Verify default language is set using get api
    output_language = frontdoor_wlan.getLanguage()
    LOGGER.debug("Default Language code set on product is %s ", output_language)

    assert output_language == DEFAULT_LANGUAGE, \
        "Default Language should be {}, not {}.".format(DEFAULT_LANGUAGE, output_language)
