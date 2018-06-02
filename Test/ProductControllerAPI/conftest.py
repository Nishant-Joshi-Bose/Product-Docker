# conftest.py
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
PyTest Configuration & Fixtures for the Product Controller frontdoor APIs.
"""
import time

import pytest
from CastleTestUtils.CAPSUtils.TransportUtils.commonBehaviorHandler import CommonBehaviorHandler
from CastleTestUtils.CAPSUtils.TransportUtils.messageCreator import MessageCreator
from CastleTestUtils.CAPSUtils.TransportUtils.responseHandler import ResponseHandler
from CastleTestUtils.FrontDoorAPI.FrontDoorQueue import FrontDoorQueue
from CastleTestUtils.PassportUtils.passport_api import PassportAPIUsers
from CastleTestUtils.RivieraUtils import adb_utils
from CastleTestUtils.RivieraUtils.hardware.keys import keypress
from CastleTestUtils.RivieraUtils.hardware.keys.keys import Keys
from CastleTestUtils.scripts.config_madrid import RESOURCES
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
import eddie_helper

LOGGER = get_logger(__name__)


@pytest.fixture(scope="function")
def front_door_queue(request, ip_address_wlan):
    """
    Get FrontDoorQueue instance.
    """
    LOGGER.info("frontDoorQueue")
    if ip_address_wlan is None:
        pytest.fail("No valid device IP")
    _frontdoor = FrontDoorQueue(ip_address_wlan)

    if _frontdoor is None:
        pytest.fail("Not able to create socket connection to front_door_queue")

    def teardown():
        if _frontdoor:
            _frontdoor.close()

    request.addfinalizer(teardown)

    return _frontdoor


@pytest.fixture(scope='function')
def device_in_aux(device_id, front_door_queue):
    """
    This fixture is used to change playing source to AUX and verifies the device state.
    Test steps:
    1. Change state to AUX play by pressing AUX button.
    2. Verify device state which should be "SELECTED".
    """
    # Change state to AUX play by pressing AUX button.
    tap = adb_utils.adb_telnet_tap(device_id)
    keypress.press_key(tap, Keys.AUX.value, 500)
    tap.close()
    time.sleep(2)

    # Verify device state which should be "SELECTED".
    state = front_door_queue.getState()
    assert state == eddie_helper.SELECTED, \
        'Device should be in {} state. Current state : {}'.format(eddie_helper.SELECTED, state)


@pytest.fixture(scope='function')
def device_playing_from_amazon(request, front_door_queue):
    """
    This fixture will send playback request to device and verifies the right station or track is playing.
    Test steps:
    1. Play a station or a track by sending playback request to the device
    2. Verify the right station or track is playing by verifying 'nowPlaying' response
    """
    service_name = 'AMAZON'
    current_resource = 'STS_AMAZON_ACCOUNT'
    location = '/v1/playback/type/playable/url/cHJpbWUvc3RhdGlvbnMvQTEwMlVLQzcxSTVEVTgvI3BsYXlhYmxl/trackIndex/0'
    container_name = 'Classic Hits'
    track_location = '/v1/playback/type/playable/url/cHJpbWUvc3RhdGlvbnMvQTEwMlVLQzcxSTVEVTgvI3BsYXlhYmxl/trackIndex/0'
    content = dict(
        container_location=location,
        container_name=container_name,
        track_location=track_location)

    get_config = RESOURCES[current_resource]

    message_creator = MessageCreator(service_name)
    response_handler = ResponseHandler(service_name, get_config['name'])
    common_behavior_handler = CommonBehaviorHandler(front_door_queue, response_handler, message_creator)

    LOGGER.info("Create passport account")
    passport_base_url = request.config.getoption('--passport-base-url')
    apikey = request.config.getoption('--api-key')
    LOGGER.info("Bose Person ID : %s ", front_door_queue._bosepersonID)
    passport_user = PassportAPIUsers(front_door_queue._bosepersonID, apikey, front_door_queue._access_token,
                                     passport_base_url, logger=LOGGER)

    def delete_passport_user():
        """
        This function will delete passport user.
        """
        LOGGER.info("delete_passport_user")
        assert passport_user.delete_users(), "Fail to delete person id: {}".format(passport_user.bosePersonID)
        common_behavior_handler.performCloudSync()

    request.addfinalizer(delete_passport_user)

    LOGGER.info("music_service_account")
    account_id = passport_user.add_service_account(service=get_config['provider'], accountID=get_config['name'],
                                                   account_name=get_config['provider_account_id'],
                                                   refresh_token=get_config['secret'])
    assert account_id and account_id != "", "Fail to add music service account."

    def remove_music_service():
        """
        This function will remove music service from account.
        """
        LOGGER.info("remove_music_service")
        # get account id for music service
        assert passport_user.remove_service_account(account_id), \
            "Fail to remove music account from passport account."

    request.addfinalizer(remove_music_service)

    common_behavior_handler.performCloudSync()

    LOGGER.info("verify_device_source")
    common_behavior_handler.checkSourceStatus(service_name, get_config['name'])

    LOGGER.debug("-- Start to play " + str(content['container_name']))
    playback_msg = message_creator.playback_msg(get_config['name'], content['container_location'],
                                                content['container_name'], content['track_location'])
    now_playing = common_behavior_handler.playContentItemAndVerifyPlayStatus(playback_msg)
    LOGGER.debug("Now Playing : " + str(now_playing))

    # Verify device state which should be "SELECTED".
    state = front_door_queue.getState()
    assert state == eddie_helper.SELECTED, \
        'Device should be in {} state. Current state : {}'.format(eddie_helper.SELECTED, state)

    yield front_door_queue, common_behavior_handler, service_name, get_config

@pytest.fixture(scope="function")
def remove_oob_setup_state_and_reboot_device(device_id, adb):
    """
    This fixture is used to remove "/mnt/nv/product-persistence/SystemSetupDone" file and reboot the device.
    """
    oob_setup_file = '/mnt/nv/product-persistence/SystemSetupDone'
    # 1. Remove "/mnt/nv/product-persistence/SystemSetupDone" file.
    adb.executeCommand("/opt/Bose/bin/rw")
    adb.executeCommand("rm {}".format(oob_setup_file))
    adb.executeCommand('sync')
    rebooted_and_out_of_booting_state_device(device_id, adb)

    # Wait until product state set to 'SetupOther' or 'SetupNetwork' state.
    for _ in range(30):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
        if device_state in [eddie_helper.SETUPOTHER, eddie_helper.SETUPNETWORK]:
            break
        time.sleep(1)

    device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
    assert device_state in [eddie_helper.SETUPOTHER, eddie_helper.SETUPNETWORK], \
        'Device should be in {} or {} state. Current state : {}'.format(eddie_helper.SETUPOTHER,
                                                                        eddie_helper.SETUPNETWORK, device_state)
