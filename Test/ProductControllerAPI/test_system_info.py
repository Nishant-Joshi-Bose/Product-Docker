# coding=utf-8
# test_system_info.py
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
PyTest based validation tests for the "/system/info" API.
"""
import os
import pytest
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
import eddie_helper

logger = get_logger(os.path.basename(__file__))


@pytest.mark.usefixtures('remove_oob_setup_state_and_reboot_device', 'front_door_queue')
def test_system_info_setup_state(front_door_queue):
    """
    Test for GET method of system info api after rebooting the device
    Test Steps:
    1. Check that endpoint is returned in capabilities.
    2. Get system info and verify response.
    """
    # 1. Check that endpoint is returned in capabilities.
    eddie_helper.check_if_end_point_exists(front_door_queue, eddie_helper.SYSTEM_INFO_API)

    # 2. Get system info and verify response.
    logger.info("Testing get system info")
    response = eddie_helper.get_system_info(front_door_queue)

    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_INFO_API, eddie_helper.METHOD_GET,
                                                 eddie_helper.STATUS_OK)

    assert (response["body"]["guid"] and
            response["body"]["countryCode"] and
            response["body"]["name"] and
            response["body"]["productColor"] and
            response["body"]["productName"] and
            response["body"]["productType"] and
            response["body"]["regionCode"] and
            response["body"]["serialNumber"] and
            response["body"]["softwareVersion"]), \
        'Not all information returned. Got response : {}'.format(response["body"])


@pytest.mark.usefixtures('front_door_queue', 'device_playing_from_amazon')
def test_system_info_playing_from_amazon(front_door_queue):
    """
    Test for GET method of system info api while playing from Amazon MSP
    Test Steps:
    1. Configure Amazon MSP account and play music.
    2. Get system info and verify response.
    """
    # 1. Configure Amazon MSP account and play music from fixture.

    # 2. Get system info and verify response.
    logger.info("Testing get system info")
    response = eddie_helper.get_system_info(front_door_queue)

    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_INFO_API, eddie_helper.METHOD_GET,
                                                 eddie_helper.STATUS_OK)

    assert (response["body"]["guid"] and
            response["body"]["countryCode"] and
            response["body"]["name"] and
            response["body"]["productColor"] and
            response["body"]["productName"] and
            response["body"]["productType"] and
            response["body"]["regionCode"] and
            response["body"]["serialNumber"] and
            response["body"]["softwareVersion"]), \
        'Not all information returned. Got response : {}'.format(response["body"])


@pytest.mark.usefixtures('front_door_queue', 'device_in_aux')
def test_system_info_playing_from_aux(front_door_queue):
    """
    Test for GET method of system info api while playing from AUX
    Test Steps:
    1. Change playing source to AUX and verifies the device state.
    2. Get system info and verify response.
    """
    # 1. Change playing source to AUX and verifies the device state from fixture.

    # 2. Get system info and verify response.
    logger.info("Testing get system info")
    response = eddie_helper.get_system_info(front_door_queue)

    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_INFO_API, eddie_helper.METHOD_GET,
                                                 eddie_helper.STATUS_OK)

    assert (response["body"]["guid"] and
            response["body"]["countryCode"] and
            response["body"]["name"] and
            response["body"]["productColor"] and
            response["body"]["productName"] and
            response["body"]["productType"] and
            response["body"]["regionCode"] and
            response["body"]["serialNumber"] and
            response["body"]["softwareVersion"]), \
        'Not all information returned. Got response : {}'.format(response["body"])
