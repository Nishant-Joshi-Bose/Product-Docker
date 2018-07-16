# manufacturing/test_service_page.py
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
Tests for the Manufacturing Service Page
"""
# pylint: disable=E1101

import requests
import pytest

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

LOGGER = get_logger(__file__)


@pytest.mark.usefixtures('ip_address_wlan', 'riviera_no_service')
def test_service_page_unavailable(ip_address_wlan):
    """
    Test to ensure that page is not available without the service flag

    :param ip_address_wlan: Networked Riviera device's IP Address
    """
    assert ip_address_wlan, "Without an IP Address, we cannot run test."

    response = requests.get('http://{}/service'.format(ip_address_wlan))
    assert response.status_code == requests.codes.not_found, \
       "With service flag unavailable we should retrieve a {} response, not {}.".format(requests.codes.not_found,
                                                                                        response.status_code)


@pytest.mark.usefixtures('ip_address_wlan', 'riviera_service_enabled')
def test_service_page_availabile(ip_address_wlan):
    """
    Test to ensure service page is available with service flag set

    :param ip_address_wlan: Networked Riviera device's IP Address
    """
    assert ip_address_wlan, "Without an IP Address, we cannot run test."

    response = requests.get('http://{}/service'.format(ip_address_wlan))
    assert response.status_code == requests.codes.ok, \
       "With service flag enabled we should retrieve a {} response, not {}.".format(requests.codes.ok,
                                                                                    response.status_code)
