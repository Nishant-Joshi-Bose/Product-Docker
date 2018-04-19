"""
Organization: BOSE CORPORATION
Copyright: COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
This program may not be reproduced, in whole or in part in any
form or any means whatsoever without the written permission of:
BOSE CORPORATION
The Mountain,
Framingham, MA 01701-9168

This file is to test Eddie Smoke Test
"""

import time
import re
import xml.etree.ElementTree as ET
import json
import logging
import pexpect
import pytest
from CastleTestUtils.NetworkUtils.network_base import NetworkBase
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.SupportUtils.StringUtils import BoseStringUtils
from CastleTestUtils.CAPSUtils.TransportUtils.commonBehaviorHandler import CommonBehaviorHandler
from CastleTestUtils.CAPSUtils.TransportUtils.messageCreator import MessageCreator
from CastleTestUtils.CAPSUtils.TransportUtils.responseHandler import ResponseHandler
from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI
from CastleTestUtils.RivieraUtils.hardware.keys import KeyUtils
from CastleTestUtils.RivieraUtils.hardware.keys import keypress
from CastleTestUtils.RivieraUtils.hardware.keys.keys import Keys
from global_resources_data import RESOURCES

logger = get_logger(__name__, "EddieSmokeTest.log", level=logging.DEBUG, fileLoglevel=logging.DEBUG)
logger.info("Starting Eddie Smoke Tests...")


class TestEddieSmoke:
    """
    Eddie Smoke Test Class
    """

    @pytest.mark.usefixtures("deviceid", "riviera", "adb")
    def test_factory_default(self, deviceid, riviera, adb):
        """
        :param deviceid: adb deviceid of Eddie
        :param riviera: Fixture to return RivieraUtils object
        :param adb: Fixture to return ADBCommunication object

        - Perform Factory default via TAP command
        - Validate Setup AP mode after Factory Default
        - Validate Product state should not be booting
        """
        riviera.perform_factory_default()
        ipaddress = get_deviceip(deviceid, 'wlan1')
        logger.debug("Device IP Address after factory default : %s ", ipaddress)
        assert ipaddress == '192.0.2.1', "Setup AP mode not up after factory default."

        product_state = adb.executeCommand('echo getproductstate | nc 0 17000')
        logger.debug("Eddie Product State : %s", product_state)
        if 'Booting' in product_state:
            assert False, "Eddie is in Booting State"


    @pytest.mark.usefixtures("ip_address_wlan")
    def test_wifi_profiles(self, ip_address_wlan):
        """
        :param ip_address_wlan: Fixture to get IP address of the device connected to WLAN

        - Add wifi profile to Eddie
        - Validate Device IP
        """

        logger.debug("Device IP Address : %s ", ip_address_wlan)
        assert BoseStringUtils.isIPValid(ip_address_wlan), "IP Address is not valid"

    @pytest.mark.usefixtures("adb")
    def test_galapagos_validation(self, adb):
        """
        :param adb: Fixture to return ADBCommunication object

        - Validate galapagos status
        - Validate server.crt file access mode
        """

        galapagos_status = adb.executeCommand('echo galapagos status | nc 0 17000')
        logger.debug("Galapagos Status : %s ", galapagos_status)
        if 'AUTHENTICATION_NO_ERROR' not in galapagos_status:
            assert False, "Galapogus status - Error in Authentication"

        if 'ACTIVATION_NO_ERROR' not in galapagos_status:
            assert False, "Galapogus status - Error in Activation"

        # It will take few seconds to appeared server.crt file after factory default
        start_time = time.time()
        timeout = 30
        while time.time() < timeout + start_time:
            file_access = adb.executeCommand('stat /mnt/nv/product-persistence/server.crt')
            if "No such file or directory" in file_access:
                logger.debug("server.crt file is not yet available... Retring again... Please wait....")
                continue
            else:
                result = re.search('Access: (\(\d{4}\/)', file_access)
                logger.debug("Server.crt file check : %s ", file_access)
                logger.debug("Server.crt file result : %s ", result)
                logger.debug("Server.crt file result group(0) : %s ", result.group(0))
                if '0444' not in result.group(1):
                    assert False, "Eddie server.crt file don't have read access"
                break
        product_status = adb.executeCommand('echo getproductstate | nc 0 17000')
        logger.debug("Eddie Product Status : %s ", product_status)
        if 'SetupOther' in product_status:
            oob_status = adb.executeCommand('echo setoobsetupcompleted | nc 0 17000')
            if 'SetOOBSetupCompleted is set' not in oob_status:
                assert False, "OOB status - Error in exit OOB"

    @pytest.mark.usefixtures("multiple_music_service_account", "verify_device_source", "music_sources", "adb", "frontDoor")
    def test_play_music(self, multiple_music_service_account, verify_device_source, music_sources, adb, frontDoor):
        """
        :param adb: Fixture to return ADBCommunication object

        - Play and Validate Music Sources
        """
        logger.info("Bose Person ID : %s ", frontDoor._bosepersonID)

        for source_name in music_sources:
            logger.info("Music Sources : %s, ", source_name)
            validate_sources(source_name, adb)
            play_music(source_name, music_sources[source_name], frontDoor)
            stop_music(frontDoor._ip_address, frontDoor)

    @pytest.mark.usefixtures("music_sources", "frontDoor")
    def test_switch_sources(self, music_sources, frontDoor):
        """
        :param music_sources: Fixture to return Sources List

        - Play and Validate Different Sources
        """
        # Below code will switch sources two times and play diffrent music sources
        for repeatcnt in range(0, 2):
            for source_name in music_sources:
                play_music(source_name, music_sources[source_name], frontDoor)

        stop_music(frontDoor._ip_address, frontDoor)

    @pytest.mark.usefixtures("adb", "music_sources", "frontDoor")
    def test_store_preset(self, adb, music_sources, frontDoor):
        """
        :param adb: Fixture to return ADBCommunication object
        :param music_sources: Fixture to return Sources List

        - Store Preset on Eddie
        """

        preset_key_start = Keys.PRESET_1.value
        index = 0
        for source_name in music_sources:
            preset_key = preset_key_start + index
            play_music(source_name, music_sources[source_name], frontDoor)
            press_key(frontDoor._ip_address, preset_key, 5000)
            validate_preset(adb, (index+1), source_name)
            index += 1

        stop_music(frontDoor._ip_address, frontDoor)

    @pytest.mark.usefixtures("music_sources", "frontDoor")
    def test_play_preset(self, music_sources, frontDoor):
        """
        :param music_sources: Fixture to return Sources List

        - Play stored Preset on Eddie
        """

        preset_key_start = Keys.PRESET_1.value
        index = 0
        for source_name in music_sources:
            preset_key = preset_key_start + index
            press_key(frontDoor._ip_address, preset_key, 750)
            validate_music_play(source_name, music_sources[source_name], frontDoor)
            index += 1

        stop_music(frontDoor._ip_address, frontDoor)

    @pytest.mark.usefixtures("adb", "frontDoor")
    def test_bluetooth_key(self, adb, frontDoor):
        """
        :param adb: Fixture to return ADBCommunication object

        - Press Bluetooth Key and Validate
        """
        press_key(frontDoor._ip_address, Keys.BLUETOOTH.value, 750)
        bluetooth_response = adb.executeCommand('echo getpdo system | nc 0 17000')
        result = re.search('BLUETOOTH()', bluetooth_response)
        if not result:
            assert False, "Bluetooth Key is not press"

    @pytest.mark.usefixtures("adb", "frontDoor")
    def test_aux_key(self, adb, frontDoor):
        """
        :param adb: Fixture to return ADBCommunication object

        - Press AUX Key and Validate
        """
        press_key(frontDoor._ip_address, Keys.AUX.value, 750)
        aux_response = adb.executeCommand('echo getpdo system | nc 0 17000')
        result = re.search('AUX()', aux_response)
        if not result:
            assert False, "AUX Key is not press"

    @pytest.mark.usefixtures("frontDoor")
    def test_volume_updown_key(self, frontDoor):
        """
        :param frontDoor: Fixture to return frontDoor object

        - Press Volume Up and Down Key and Validate
        """
        volume_min = 100
        volume_max = 0
        
        volume_actual = get_volume(frontDoor)
        logger.debug("Actual Volume : %s ", volume_actual)
        # Volume already set to MAX then need to do volume down before volume up key press
        if volume_actual == volume_max:
            press_key(frontDoor._ip_address, Keys.VOLUME_DOWN.value, 30)
            volume_actual = get_volume(frontDoor)
            logger.debug("Actual Volume : %s ", volume_actual)
        
        press_key(frontDoor._ip_address, Keys.VOLUME_UP.value, 20)
        volume_up = get_volume(frontDoor)
        logger.debug("Volume Up : %s ", volume_up)
        assert volume_up > volume_actual, "Volume Up Key Press not happened"

        volume_actual = get_volume(frontDoor)
        logger.debug("Actual Volume : %s ", volume_actual)

        # Volume already set to MIN then need to do volume up before volume down key press
        if volume_actual == volume_min:
            press_key(frontDoor._ip_address, Keys.VOLUME_UP.value, 30)
            volume_actual = get_volume(frontDoor)
            logger.debug("Actual Volume : %s ", volume_actual)
        
        press_key(frontDoor._ip_address, Keys.VOLUME_DOWN.value, 20)
        volume_down = get_volume(frontDoor)
        logger.debug("Volume Down : %s ", volume_down)
        assert volume_down < volume_actual, "Volume Down Key Press not happened"

    @pytest.mark.usefixtures("frontDoor")
    def test_volume_updown_ramp_key(self, frontDoor):
        """
        :param frontDoor: Fixture to return frontDoor object

        - Press Volume Up and Down Ramp Key and Validate
        """
        volume_min = 100
        volume_max = 0
        
        volume_actual = get_volume(frontDoor)
        logger.debug("Actual Volume : %s ", volume_actual)
        
        # Volume already set to MAX then need to do volume down before volume up key press
        if volume_actual == volume_max:
            press_key(frontDoor._ip_address, Keys.VOLUME_DOWN.value, 400)
            volume_actual = get_volume(frontDoor)
            logger.debug("Actual Volume : %s ", volume_actual)

        press_key(frontDoor._ip_address, Keys.VOLUME_UP.value, 350)
        volume_up = get_volume(frontDoor)
        logger.debug("Volume Up : %s ", volume_up)
        assert volume_up > volume_actual, "Volume Up Key Press not happened"

        volume_actual = get_volume(frontDoor)
        logger.debug("Actual Volume : %s ", volume_actual)
        
        # Volume already set to MIN then need to do volume up before volume down key press
        if volume_actual == volume_min:
            press_key(frontDoor._ip_address, Keys.VOLUME_UP.value, 400)
            volume_actual = get_volume(frontDoor)
            logger.debug("Actual Volume : %s ", volume_actual)
        
        press_key(frontDoor._ip_address, Keys.VOLUME_DOWN.value, 350)
        volume_down = get_volume(frontDoor)
        logger.debug("Volume Down : %s ", volume_down)
        assert volume_down < volume_actual, "Volume Down Key Press not happened"

def get_deviceip(deviceid, inf):
    """
    :param deviceid: adb deviceid of Eddie
    :param inf: Network Interface For ex. wlan0 or eth0
    - Get Device IP based on deviceid
    """
    netbase = NetworkBase(None, deviceid)
    ipaddress = netbase.check_inf_presence(inf, timeout=120)
    return ipaddress

def get_tuneIn_content():
    """
    TuneIn Data - New England Public Radio Station
    """
    data = '{"source":"TUNEIN",\
                    "sourceAccount":"",\
                    "preset":{"type":"stationurl",\
                                "location":"/v1/playback/station/s28589",\
                                "name":"100.7 WZLX",\
                                "presetable":"true",\
                                "containerArt":"http://cdn-radiotime-logos.tunein.com/s28589q.png"},\
                    "playbackLocation":{"type":"stationurl",\
                                "location":"/v1/playback/station/s28589",\
                                "name":"100.7 WZLX",\
                                "presetable":"true",\
                                "containerArt":"http://cdn-radiotime-logos.tunein.com/s28589q.png"}}'
    return data

def get_amazon_content():
    """ Sample playbackRequest content for Amazon MSP """
    amazon_content = {
        'container_location': \
        '/v1/playback/type/playable/url/cHJpbWUvc3RhdGlvbnMvQTEwMlVLQzcxSTVEVTgvI3BsYXlhYmxl/trackIndex/0',
        'container_name': 'Classic Hits',
        'track_location':\
        '/v1/playback/type/playable/url/cHJpbWUvc3RhdGlvbnMvQTEwMlVLQzcxSTVEVTgvI3BsYXlhYmxl/trackIndex/0',
        }

    return amazon_content

def validate_sources(source, adb, timeout=120):
    """
    :param source: Music Source For ex. TUNEIN, AMAZON etc.
    :param adb: ADBCommunication object
    :param timeout: time in seconds

    - Validate Sources available in Source.xml file of Eddie device
    """
    start_time = time.time()

    while time.time() < timeout + start_time:
        sources_xml = adb.executeCommand('cat /mnt/nv/product-persistence/Sources.xml')
        if "No such file or directory" in sources_xml:
            logger.debug("Sources.xml file is not available... Verifying again...")
            continue
        else:
            break

    sourceList = []
    logger.info("Soruces : %s ", str(sources_xml))
    root = ET.fromstring(str(sources_xml))
    for sources in root.findall("source"):
        for sourcekey in sources.findall("sourceKey"):
            sourceList.append(sourcekey.get('type'))
    if source not in sourceList:
        assert False, "Sources.XML does not contain " + source + " Source"

def get_common_handler(source_name, source_account_name, frontDoor):
    """
    :param source_name: Music Source Name For ex. TUNEIN, AMAZON etc.
    :param frontDoor: FrontDoor object

    - Common function to get common_behavior_handler, response_handler and playback message based on each source.
    """
    message_creator = MessageCreator(source_name)

    if source_name == 'TUNEIN':
        playback_msg = get_tuneIn_content()
        user = ''
    elif source_name == 'AMAZON':
        content = get_amazon_content()
        resource = RESOURCES[source_account_name]
        user = resource['name']
        playback_msg = message_creator.playback_msg(user, content['container_location'], content['container_name'], content['track_location'])

    response_handler = ResponseHandler(source_name, user)
    common_behavior_handler = CommonBehaviorHandler(frontDoor, response_handler, message_creator)

    return (common_behavior_handler, response_handler, playback_msg)

def play_music(source_name, source_account_name, frontDoor, timeout = 10):
    """
    :param source_name: Music Source Name For ex. TUNEIN, AMAZON etc.
    :param frontDoor: FrontDoor object

    - Common function to play music based on each source.
    """
    common_obj = get_common_handler(source_name, source_account_name, frontDoor)
    common_behavior_handler = common_obj[0]
    playback_msg = common_obj[2]
    common_behavior_handler.playContentItemAndVerifyPlayStatus(playback_msg)
    
    # Below loop to play music for few seconds
    start_time = time.time()
    while time.time() < timeout + start_time:
        time.sleep(1)

def validate_music_play(source_name, source_account_name, frontDoor):
    """
    :param source_name: Music Source Name For ex. TUNEIN, AMAZON etc.
    :param frontDoor: FrontDoor object

    - Common function to validate play music based on each source.
    """
    common_obj = get_common_handler(source_name, source_account_name, frontDoor)
    common_behavior_handler = common_obj[0]
    response_handler = common_obj[1]
    playback_msg = common_obj[2]
    playresponse = common_behavior_handler.checkPlayStatus('play')
    response_handler.verify_device_now_playing_response(nowplaying_response=playresponse, playback_msg=playback_msg)

def stop_music(deviceip, frontDoor):
    """
    :param deviceip: Device IP of Eddie.

    - Common function to stop music by putting Eddie on Network Standby.
    """
    response = frontDoor.stopPlaybackRequest()
    logger.info("Stop Playback Request : %s ", response)
    tap = pexpect.spawn("telnet %s 17000" % deviceip)
    KeyUtils.power_key_press(tap, 3000)

def validate_preset(adb, presetid, source_name):
    """
    :param adb: ADBCommunication object.
    :param presetid: Stored Preset Id
    :param source_name: Music Source Name For ex. TUNEIN, AMAZON etc.

    - Common function to validate each preset.
    """
    response = adb.executeCommand('cat /mnt/nv/product-persistence/ProductSettings.json')
    presets_json = json.loads(response)
    presets_details = presets_json['ProductSettings']['presets']['preset']
    presets_source = [preset for preset in presets_details if preset['id'] == presetid]
    logger.debug("Preset Details : %s", presets_source)
    assert presets_source[0]['ContentItem']['source'] == source_name, "Preset is not stored, Please verify"

def press_key(deviceip, keyid, presstime):
    """
    :param deviceip: Device IP of Eddie.
    :param keyid: Key Id of Eddie
    :param presstime: time in ms.

    - Common function to press key on Eddie.
    """
    tap = pexpect.spawn("telnet %s 17000" % deviceip)
    keypress.press_key(tap, keyid, presstime)

def get_volume(frontDoor):
    """
    :param frontDoor: FrontDoor object

    - Common function to get current volume of Eddie.
    """
    response = frontDoor.getVolume()
    volume = response['body']['value']
    return volume
