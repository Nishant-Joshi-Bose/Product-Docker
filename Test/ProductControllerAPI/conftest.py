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
from CastleTestUtils.PassportUtils.passport_utils import create_passport_account
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
def device_in_aux(deviceid, front_door_queue):
    """
    This fixture is used to change playing source to AUX and verifies the device state.
    Test steps:
    1. Change state to AUX play by pressing AUX button.
    2. Verify device state which should be "SELECTED".
    """
    # Change state to AUX play by pressing AUX button.
    tap = adb_utils.adb_telnet_tap(deviceid)
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
    sys_info = front_door_queue.getInfo()["body"]
    device_guid = sys_info["guid"]
    assert device_guid is not None
    LOGGER.debug("GUID is: %s", device_guid)

    device_type = sys_info["productType"]
    LOGGER.debug("Device type: %s", device_type)
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
    boseperson_id = create_passport_account(passport_base_url, "Eddie", "APITest", apikey)
    passport_user = PassportAPIUsers(boseperson_id, apikey, passport_base_url)

    def delete_passport_user():
        """
        This function will delete passport user.
        """
        LOGGER.info("delete_passport_user")
        assert passport_user.delete_users(), "Fail to delete person id: %s" % boseperson_id
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

    LOGGER.info("add_device_to_passport")
    assert passport_user.add_product(device_guid, device_type), "Failed to add device to passport account."
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
def rebooted_and_out_of_booting_state_device(deviceid, adb):
    """
    This fixture is used to reboot the device and wait until device come out of 'Booting' state.
    """
    adb.rebootDevice_adb()

    # Wait for CLI-Server to start and listens on 17000 port.
    status = None
    for _ in range(30):
        status = adb.executeCommand("(netstat -tnl | grep -q 17000) && echo OK")
        if status and status.strip() == 'OK':
            break
        time.sleep(1)
    assert status, "CLIServer not started within 30s."
    assert (status.strip() == 'OK'), 'CLIServer is not stated even after 30 seconds'
    LOGGER.debug("CLIServer started within 30s.")

    time.sleep(2)

    for _ in range(10):
        if adb.executeCommand("echo '?' | nc 0 17000 | grep 'getproductstate'"):
            break
        time.sleep(1)
    LOGGER.debug("getproductstate command is registered to CLI-Server.")

    # Wait until product state come out from 'Booting' state.
    for _ in range(30):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=deviceid)
        if device_state != eddie_helper.BOOTING:
            break
        LOGGER.debug("Current device state : %s", device_state)
        time.sleep(1)


@pytest.fixture(scope="class")
def set_no_audio_timeout(deviceid, adb):
    """
    This fixture changes "NoAudioTimeout" param to test NetworkStandby state transition.
    Test steps:
    1. Change "NoAudioTimeout" to 1 minute.
    2. Reboot and wait for CLI-Server to start and device state get out of Booting.
    3. At the end of test case revert "NoAudioTimeout" to 20 minutes.
    4. Reboot and wait for CLI-Server to start and device state get out of Booting.
    """
    timer_file = '/opt/Bose/etc/InActivityTimer.json'
    # 1. Change "NoAudioTimeout" to 1 minute.
    adb.executeCommand("/opt/Bose/bin/rw")
    adb.executeCommand("sed 's/\\\"NoAudioTimeout\\\": 20,/\\\"NoAudioTimeout\\\": 1,/' -i {}".format(timer_file))
    adb.executeCommand('sync')
    # 2. Reboot and wait for CLI-Server to start and device state get out of Booting.
    rebooted_and_out_of_booting_state_device(deviceid, adb)

    yield

    # 3. At the end of test case revert "NoAudioTimeout" to 20 minutes.
    adb.executeCommand("/opt/Bose/bin/rw")
    adb.executeCommand("sed 's/\\\"NoAudioTimeout\\\": 1,/\\\"NoAudioTimeout\\\": 20,/' -i {}".format(timer_file))
    adb.executeCommand('sync')
    # 4. Reboot and wait for CLI-Server to start and device state get out of Booting.
    rebooted_and_out_of_booting_state_device(deviceid, adb)


@pytest.fixture(scope="function")
def remove_oob_setup_state_and_reboot_device(deviceid, adb):
    """
    This fixture is used to remove "/mnt/nv/product-persistence/SetupState.json" file and reboot the device.
    """
    oob_setup_file = '/mnt/nv/product-persistence/SetupState.json'
    # 1. Remove "/mnt/nv/product-persistence/SetupState.json" file.
    adb.executeCommand("/opt/Bose/bin/rw")
    adb.executeCommand("rm {}".format(oob_setup_file))
    adb.executeCommand('sync')
    rebooted_and_out_of_booting_state_device(deviceid, adb)

    # Wait until product state set to 'SetupOther' or 'SetupNetwork' state.
    for _ in range(30):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=deviceid)
        if device_state in [eddie_helper.SETUPOTHER, eddie_helper.SETUPNETWORK]:
            break
        time.sleep(1)

    device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=deviceid)
    assert device_state in [eddie_helper.SETUPOTHER, eddie_helper.SETUPNETWORK], \
        'Device should be in {} or {} state. Current state : {}'.format(eddie_helper.SETUPOTHER,
                                                                        eddie_helper.SETUPNETWORK, device_state)
