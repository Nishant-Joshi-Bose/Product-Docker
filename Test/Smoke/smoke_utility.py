"""
Organization: BOSE CORPORATION
Copyright: COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
This program may not be reproduced, in whole or in part in any
form or any means whatsoever without the written permission of:
BOSE CORPORATION
The Mountain,
Framingham, MA 01701-9168

Eddie Smoke Test Utility
"""

import time
import pexpect
import json
import xml.etree.ElementTree as ET
from CastleTestUtils.NetworkUtils.network_base import NetworkBase
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.CAPSUtils.TransportUtils.commonBehaviorHandler import CommonBehaviorHandler
from CastleTestUtils.CAPSUtils.TransportUtils.messageCreator import MessageCreator
from CastleTestUtils.CAPSUtils.TransportUtils.responseHandler import ResponseHandler
from CastleTestUtils.RivieraUtils.hardware.keys import KeyUtils
from CastleTestUtils.RivieraUtils.hardware.keys import keypress
from global_resources_data import RESOURCES

class SmokeUtils:
    """
    :Abstract: Common Helper class for Smoke Test
    """

    def __init__(self, logger=None):
        """
        init method to set Logger object
        """
        self.logger = logger or get_logger(__name__)

    def get_deviceip(self, deviceid, inf):
        """
        :param deviceid: adb deviceid of Eddie
        :param inf: Network Interface For ex. wlan0 or eth0
        - Get Device IP based on deviceid
        """
        netbase = NetworkBase(None, deviceid)
        ipaddress = netbase.check_inf_presence(inf, timeout=120)
        return ipaddress

    def get_tuneIn_content(self):
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

    def get_amazon_content(self):
        """ Sample playbackRequest content for Amazon MSP """
        amazon_content = {
            'container_location': \
            '/v1/playback/type/playable/url/cHJpbWUvc3RhdGlvbnMvQTEwMlVLQzcxSTVEVTgvI3BsYXlhYmxl/trackIndex/0',
            'container_name': 'Classic Hits',
            'track_location':\
            '/v1/playback/type/playable/url/cHJpbWUvc3RhdGlvbnMvQTEwMlVLQzcxSTVEVTgvI3BsYXlhYmxl/trackIndex/0',
            }

        return amazon_content

    def validate_sources(self, source, adb, timeout=120):
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
                self.logger.debug("Sources.xml file is not available... Verifying again...")
                continue
            else:
                break

        sourceList = []
        self.logger.info("Soruces : %s ", str(sources_xml))
        root = ET.fromstring(str(sources_xml))
        for sources in root.findall("source"):
            for sourcekey in sources.findall("sourceKey"):
                sourceList.append(sourcekey.get('type'))
        if source not in sourceList:
            assert False, "Sources.XML does not contain " + source + " Source"

    def get_common_handler(self, source_name, source_account_name, frontDoor):
        """
        :param source_name: Music Source Name For ex. TUNEIN, AMAZON etc.
        :param frontDoor: FrontDoor object

        - Common function to get common_behavior_handler, response_handler and playback message based on each source.
        """
        message_creator = MessageCreator(source_name)

        if source_name == 'TUNEIN':
            playback_msg = self.get_tuneIn_content()
            user = ''
        elif source_name == 'AMAZON':
            content = self.get_amazon_content()
            resource = RESOURCES[source_account_name]
            user = resource['name']
            playback_msg = message_creator.playback_msg(user, content['container_location'], content['container_name'], content['track_location'])

        response_handler = ResponseHandler(source_name, user)
        common_behavior_handler = CommonBehaviorHandler(frontDoor, response_handler, message_creator)

        return (common_behavior_handler, response_handler, playback_msg)

    def play_music(self, source_name, source_account_name, frontDoor, timeout = 10):
        """
        :param source_name: Music Source Name For ex. TUNEIN, AMAZON etc.
        :param frontDoor: FrontDoor object

        - Common function to play music based on each source.
        """
        common_obj = self.get_common_handler(source_name, source_account_name, frontDoor)
        common_behavior_handler = common_obj[0]
        playback_msg = common_obj[2]
        common_behavior_handler.playContentItemAndVerifyPlayStatus(playback_msg)
    
        # Below loop to play music for few seconds
        start_time = time.time()
        while time.time() < timeout + start_time:
            time.sleep(1)

    def validate_music_play(self, source_name, source_account_name, frontDoor):
        """
        :param source_name: Music Source Name For ex. TUNEIN, AMAZON etc.
        :param frontDoor: FrontDoor object

        - Common function to validate play music based on each source.
        """
        common_obj = self.get_common_handler(source_name, source_account_name, frontDoor)
        common_behavior_handler = common_obj[0]
        response_handler = common_obj[1]
        playback_msg = common_obj[2]
        playresponse = common_behavior_handler.checkPlayStatus('play')
        response_handler.verify_device_now_playing_response(nowplaying_response=playresponse, playback_msg=playback_msg)

    def stop_music(self, deviceip, frontDoor):
        """
        :param deviceip: Device IP of Eddie.

        - Common function to stop music by putting Eddie on Network Standby.
        """
        response = frontDoor.stopPlaybackRequest()
        self.logger.info("Stop Playback Request : %s ", response)
        tap = pexpect.spawn("telnet %s 17000" % deviceip)
        KeyUtils.power_key_press(tap, 3000)

    def validate_preset(self, adb, presetid, source_name):
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
        self.logger.debug("Preset Details : %s", presets_source)
        assert presets_source[0]['ContentItem']['source'] == source_name, "Preset is not stored, Please verify"

    def press_key(self, deviceip, keyid, presstime):
        """
        :param deviceip: Device IP of Eddie.
        :param keyid: Key Id of Eddie
        :param presstime: time in ms.

        - Common function to press key on Eddie.
        """
        tap = pexpect.spawn("telnet %s 17000" % deviceip)
        keypress.press_key(tap, keyid, presstime)

    def get_volume(self, frontDoor):
        """
        :param frontDoor: FrontDoor object

        - Common function to get current volume of Eddie.
        """
        response = frontDoor.getVolume()
        volume = response['body']['value']
        return volume
