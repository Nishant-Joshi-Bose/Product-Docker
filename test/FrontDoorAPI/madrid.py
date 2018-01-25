#!/usr/bin/env python

"""
Coomand Line tool to send requests to FrontDoor + Passport + Riviera Device
"""

import argparse
import json

from FrontDoorAPI import FrontDoorAPI
from ..PassportUtils.passport_utils import *
from ..PassportUtils.passport_api import PassportAPIUsers
from ..RivieraUtils.rivieraUtils import RivieraUtils
from global_resources_data import RESOURCES

"""
Currently; Hard-coded details
"""

# IP Address of Eddie / Professor
print "Enter the IP Address of the Eddie"
_ip_address = '192.168.1.210'

# Ideally, every team should have a different key
# This gives access to streaming-int; Passport Integration URL
APIKEY = "FITj7BYxfotTOHuJGSktJJNdukyUIYJy"

frontDoor = FrontDoorAPI(_ip_address)
global passportUser

class Madrid(object):

    def __init__(self):
        parser = argparse.ArgumentParser(
            description='Command-Line Version of Madrid-App',
            usage=''' <api> [<args>]

The map is as follows

  Part0: OOB Setup & Riviera Commands
  1. get_riviera_version
  2. wifi_setup
  3. get_FS_version
  4. cat <xml> File

  Part1: REST calls to Passport-Server
  Users
  - create_boseperson

  Product
  - add_product
  - get_productsInfo

  MSP's
  - add_MSPAccounts
  - getMSPDetails
  - remove_MSPAccounts

  ---------------------------------------------------------------

  Part2: WebSocket requests to FrontDoor

   -- Eddie --
   audio_bass			 	/audio/bass
   audio_treble			 	/audio/treble
   configuration_status          	/system/configuration/status
   product_Settings              	/system/productSettings
   system_capabilties            	/system/capabilties
   system_language              	/system/language
   system_state                  	/system/state
   system_sources                	/system/sources
   ui_display                    	/ui/display
   ui_lightbar                   	/ui/lightbar

   -- AVS --
   voice_listen                  	/voice/listen
   voice_mic                     	/voice/mic
   voice_status                  	/voice/status

   -- CAPS --
   addZoneSlave                  	/addZoneSlave
   audio_defaultVolume           	/audio/defaultVolume
   audio_volume                  	/audio/volume
   audio_volume_decrement        	/audio/volume/decrement
   audio_volume_increment        	/audio/volume/increment
   audio_volumeMax               	/audio/volumeMax
   audio_volumeMin               	/audio/volumeMin
   audio_zone                    	/audio/zone
   content_allowSourceSelect     	/content/allowSourceSelect
   content_nowPlaying             	/content/nowPlaying
   content_playbackRequest              /content/playbackRequest
   content_selectLastSource             /content/selectLastSource
   content_selectLastStreamingSource	/content/selectLastStreamingSource
   content_stopPlayback                 /content/stopPlayback
   content_transportControl             /content/transportControl
   getZone			        /getZone
   removeZoneSlave		        /removeZoneSlave
   setZone                              /setZone
   system_capsInitializationStatus      /system/capsInitializationStatus

   -- NetworkServiceFrontDoorIF --
   network_status                       /network/status
   network_wifi_ap                      /network/wifi/ap
   network_wifi_profile                 /network/wifi/profile
   network_wifi_siteScan                /network/wifi/siteScan
   network_wifi_status                  /network/wifi/status

   -- BLEFrontDoorClient --
   bluetooth_BLESetup_activate          /bluetooth/BLESetup/activate
   bluetooth_BLESetup_status            /bluetooth/BLESetup/status

   -- BTSinkController --
   bluetooth_sink_connect               /bluetooth/sink/connect
   bluetooth_sink_disconnect            /bluetooth/sink/disconnect
   bluetooth_sink_list                  /bluetooth/sink/list
   bluetooth_sink_macAddr               /bluetooth/sink/macAddr
   bluetooth_sink_pairable              /bluetooth/sink/pairable
   bluetooth_sink_remove                /bluetooth/sink/remove
   bluetooth_sink_removeAll             /bluetooth/sink/removeAll
   bluetooth_sink_status                /bluetooth/sink/status

   -- BTSourceController --

   bluetooth_source_connect             /bluetooth/source/connect
   bluetooth_source_disconnect          /bluetooth/source/disconnect
   bluetooth_source_list                /bluetooth/source/list
   bluetooth_source_pair                /bluetooth/source/pair
   bluetooth_source_remove              /bluetooth/source/remove
   bluetooth_source_removeAll           /bluetooth/source/removeAll
   bluetooth_source_volume              /bluetooth/sink/status

   -- Passport --
   cloudSync                            /cloudSync

   -- SWUpdateService --
   install_now                          /install/now

   -- Galapagos Client --
   product_user_association             /product-user-association
   system_activationStatus              /system/activationStatus
   system_authenticationStatus          /system/authenticationStatus

   -- FrontDoorSelfSerice --
   network_ping                         /network/ping


''')
        parser.add_argument('command', help='Subcommand to run')
        # parse_args defaults to [1:] for args, but you need to
        # exclude the rest of the args too, or validation will fail
        args = parser.parse_args(sys.argv[1:2])
        if not hasattr(self, args.command):
            print 'Unrecognized command'
            parser.print_help()
            exit(1)
        # use dispatch pattern to invoke method with same name
        getattr(self, args.command)()

# ----------------------------------------------------------------------------------------------
# OOB & Generic Riviera Commands

    def get_riviera_version(self):
        parser = argparse.ArgumentParser(
            description='Get the Riviera-Version')
        riviera = RivieraUtils('ADB')
        version = json.dumps(riviera.get_riviera_version(), indent=4)
        print "The riviera-version is:\n", version

    def getXmlorJson(self):
        parser = argparse.ArgumentParser(
            description='Get the contents of given XML / JSON file')

    def get_deviceFSVersion(self):
        parser = argparse.ArgumentParser(
            description='Get the S/W Build Version')
        riviera = RivieraUtils('ADB')
        version = json.dumps(riviera.getDeviceFsVersion(), indent=4)
        print "The S/W version is:\n", version

    def get_product_settings(self):
        parser = argparse.ArgumentParser(description='Get the Riviera Product Settings')
        riviera = RivieraUtils('ADB')
        product_settings = json.dumps(riviera.get_product_settings(), indent=4)
        print('The Product Settings are:\n{}'.format(product_settings))

# ----------------------------------------------------------------------------------------------
# REST CALLS to Passport Server

    def create_boseperson(self):
        parser = argparse.ArgumentParser(
            description='Create a New Passport Account')
        print "\nEnter your First Name"
        fname = raw_input()
        print "\nEnter your Last Name"
        lname = raw_input()
        # Currently by default points to Integration
        passport_base_url = "https://platform.bose.io/dev/svc-passport-core/integration/passport-core/"
        bosepersonID = create_passport_account(passport_base_url, fname, lname)
        self.boseID = bosepersonID
        print "\nYour Bose-PersonID is:", bosepersonID

    def get_bosepersonID(self):
        parser = argparse.ArgumentParser(
            description='Retrieve BosePerson ID')
        passport_base_url = "https://platform.bose.io/dev/svc-passport-core/integration/passport-core/"
        bosepersonID = get_bosePersonID(passport_base_url, APIKEY)
        print "Your Bose-PersonID is:", bosepersonID

    def delete_boseperson(self):
        parser = argparse.ArgumentParser(
            description='Delete the Bose-Person')
        parser.add_argument('--bosepersonID', required=True)
        args = parser.parse_args(sys.argv[2:])
        self.passportUser.delete_users(args.bosepersonID)

    def add_product(self):
        parser = argparse.ArgumentParser(
            description='Add Product to Passport-Account Created')
        print "Enter the eth0 MAC Address of the product"
        product_id = raw_input()
        product_type = "Eco2Products"
        print "Please enter your BoseID"
        bosepersonID = raw_input()
        passport_base_url = "https://platform.bose.io/dev/svc-passport-core/integration/passport-core/"
        passportUser = PassportAPIUsers(bosepersonID, APIKEY, passport_base_url)
        passportUser.add_product(product_id, product_type)

    def get_productsInfo(self):
        parser = argparse.ArgumentParser(
            description='Get Product Info for given Passport-Account')
        print "Please enter your BoseID"
        bosepersonID = raw_input()
        passport_base_url = "https://platform.bose.io/dev/svc-passport-core/integration/passport-core/"
        passportUser = PassportAPIUsers(bosepersonID, APIKEY, passport_base_url)
        products = passportUser.get_all_products_info()
        print "The products are:", products_accounts

    def get_MSPDetails(self):
        parser = argparse.ArgumentParser(
            description='Get MSP Details for given Passport-Account')
        print "Please enter your BoseID"
        bosepersonID = raw_input()
        passport_base_url = "https://platform.bose.io/dev/svc-passport-core/integration/passport-core/"
        passportUser = PassportAPIUsers(bosepersonID, APIKEY, passport_base_url)
        passportUser.get_all_service_accounts_info()

    def add_MSPAccounts(self):
        parser = argparse.ArgumentParser(
            description='Add MSP Accounts to given Passport-Account')
        print "Please enter your BoseID"
        bosepersonID = raw_input()
        passport_base_url = "https://platform.bose.io/dev/svc-passport-core/integration/passport-core/"
        passportUser = PassportAPIUsers(bosepersonID, APIKEY, passport_base_url)
        print "Enter the source you want to add?"
        source = raw_input()
        account_data = RESOURCES[source]
        print "The account secret is:", account_data['secret']
        accountID = passportUser.add_service_account(service=account_data['provider'], account_id=account_data['name'],account_name=account_data['provider_account_id'],refresh_token=account_data['secret'])
        self.cloudSync()
        print "The associated account id for the source %s is",source,accountID

    def remove_MSPAccounts(self):
        parser = argparse.ArgumentParser(
            description='remove MSP Accounts for given Passport-Account')
        print "Please enter your BoseID"
        bosepersonID = raw_input()
        passport_base_url = "https://platform.bose.io/dev/svc-passport-core/integration/passport-core/"
        passportUser = PassportAPIUsers(bosepersonID, APIKEY, passport_base_url)
        print "Enter the associated accountID"
        accountID = raw_input()
        passportUser.remove_service_account(accountID)

# -----------------------------------------------------------------------------------------------
# End-Point Handler - Eddie

    def system_capabilties(self):
        parser = argparse.ArgumentParser(
            description='List all APIs registered by the Product')
        response = frontDoor.getCapabilities()

    def system_state(self):
        parser = argparse.ArgumentParser(
            description='State of Product-Controller - Eddie')
        response = frontDoor.getState()

    def configuration_status(self):
        parser = argparse.ArgumentParser(
            description='Configuration Status')
        response = frontDoor.getconfigurationStatus()

    def product_info(self):
        parser = argparse.ArgumentParser(
            description='Product Info - Eddie')
        response = frontDoor.getInfo()

    def language(self):
        parser = argparse.ArgumentParser(
            description='Language in the device')
        # prefixing the argument with -- means it's optional
        parser.add_argument('--method', choices=['post', 'get'])
        # now that we're inside a subcommand, ignore the first
        # TWO argvs, ie the command (git) and the subcommand (commit)
        args = parser.parse_args(sys.argv[2:])
        print 'Sending /system/language, method=%s' % args.method
        if args.method.lower() == 'post':
            print "\nEnter the language you want to send on the Device"
            lng = raw_input()
            data = json.dumps({"code": lng})
            print "The data to be sent is:", data
            response = frontDoor.setLanguage(data)
        elif args.method.lower() == 'get':
            response = frontDoor.getLanguage()

# --------------------------------------------------------------------------------------

# End-Point Handler - CAPS
    def content_playbackRequest(self):
        parser = argparse.ArgumentParser(
            description='Sending Playback Request')
        print "\nWhat source do you want to play?"
        source = raw_input()
        if source.lower() == 'deezer':
            #TODO: Can potentially parse Sources.xml and raise Error - Better Logic
            print "\nEnter your sourceAccount?:"
            account = raw_input()
            data = json.dumps({"source": source,
                               "sourceAccount": account,
                               "preset":{"type":"tracklisturl",
                                         "location":"/v1/playback/containerType/album/containerId/7776833",
                                         "name":"XSCAPE",
                                         "presetable":"true",
                                         "containerArt":""},
                               "playback":{"type":"tracklisturl",
                                           "location":"/v1/playback/containerType/album/containerId/7776833/track/78213533",
                                           "name":"XSCAPE",
                                           "presetable":"true"}}, indent=4)

        if source.lower() == 'amazon':
           print "\nEnter your sourceAccount?:"
           account = raw_input()
           data = json.dumps({"source": source,
                              "sourceAccount": account,
                              "preset":{"type":"tracklisturl",
                                        "location":"/v1/playback/type/playable/url/cHJpbWUvc3RhdGlvbnMvQTEwMlVLQzcxSTVEVTgvI3BsYXlhYmxl/trackIndex/0",
                                        "name":"Classic Hits",
                                        "presetable":"true", "containerArt":""},
                              "playback":{"type":"tracklisturl",
                                          "location":"/v1/playback/type/playable/url/cHJpbWUvc3RhdGlvbnMvQTEwMlVLQzcxSTVEVTgvI3BsYXlhYmxl/trackIndex/0",
                                          "presetable":"true"}}, indent=4)

        if source.lower() == 'tunein':
           data = json.dumps({"source": source,
                              "sourceAccount":"",
                              "preset":{"type":"stationurl",
                                        "location":"/v1/playback/station/s24375",
                                        "name":"100.7 WZLX",
                                        "presetable":"true",
                                        "containerArt":"http://cdn-radiotime-logos.tunein.com/s24375q.png"},
                              "playback":{"type":"stationurl",
                                          "location":"/v1/playback/station/s24375",
                                          "name":"100.7 WZLX",
                                          "presetable":"true",
                                          "containerArt":"http://cdn-radiotime-logos.tunein.com/s24375q.png"}}, indent=4)

        if source.lower() == 'spotify':
            data = json.dumps({"source": source,
                               "sourceAccount": account,
                               "preset":{"type":"tracklisturl",
                                         "location":"/v1/playback/container/c3BvdGlmeTp1c2VyOjEyMTIyODgyNzYwOnBsYXlsaXN0OjdEWkhpb1E1RldaOXZhTHNPY2YwaVU=",
                                         "name":"Take A Chance On Me",
                                         "presetable":"true","containerArt":""},
                               "playback":{"type":"tracklisturl",
                                           "location":"/v1/playback/container/c3BvdGlmeTp1c2VyOjEyMTIyODgyNzYwOnBsYXlsaXN0OjdEWkhpb1E1RldaOXZhTHNPY2YwaVU=/track/2",
                                           "name":"Take A Chance On Me",
                                           "presetable":"true"}}, indent=4)

        print "\nThe data going to be sent is\n", data
        playbackRequest = frontDoor.sendPlaybackRequest(data)

    def content_nowPlaying(self):
        parser = argparse.ArgumentParser(
            description='Sending Playback Request')
        response = frontDoor.getNowPlaying()

    def system_sources(self):
        parser = argparse.ArgumentParser(
            description='Fetching the source-list from Eddie')
        sourcelist = frontDoor.getSources()


# --------------------------------------------------------------------------------------
# Handler - Network Service

    def network_status(self):
        parser = argparse.ArgumentParser(
            description='Fetching Network Status')
        response = frontDoor.getNetworkStatus()

    def network_wifi_status(self):
        parser = argparse.ArgumentParser(
            description='Fetching Network Wi-Fi Status')
        response = frontDoor.getNetworkWiFiStatus()

    def network_wifi_profile(self):
        parser = argparse.ArgumentParser(
            description='GET/POST network-wifi-profile')
        # prefixing the argument with -- means it's optional
        parser.add_argument('--method', choices=['post', 'get'], default='get')
        # now that we're inside a subcommand, ignore the first
        # TWO argvs, ie the command (git) and the subcommand (commit)
        args = parser.parse_args(sys.argv[2:])
        print 'Sending /system/language, method=%s' % args.method
        if args.method.lower() == 'post':
            print "Enter the SSID / Router-name"
            routername = raw_input()
            security = 'wpa_or_wpa2'
            print "Enter your Wi-Fi password"
            password = raw_input()
            print "Adding Wireless Profile - Connecting to:", routername
            data = json.dumps({"profile":{"security":security, "ssid":routername, "passphrase":password}, "timeout":120}, indent=4)
            addwifiProfileResponse = frontDoor.addWifiProfile(data)
            assert addwifiProfileResponse
            assert addwifiProfileResponse["body"]["result"] == "WIFI_ADD_SUCCEEDED"

        if args.method.lower() == 'get':
            response = frontDoor.getWifiProfile()

    def network_wifi_siteScan(self):
        parser = argparse.ArgumentParser(
            description = 'Site-Scan for potential APs')
        data = json.dumps({}, indent=4)
        siteSurvey = frontDoor.sendSiteScan(data)

# --------------------------------------------------------------------------------------
# BLUETOOTH related API's

# --------------------------------------------------------------------------------------
# Handler - Galapagos Client

# --------------------------------------------------------------------------------------
# Handler - AVS

# --------------------------------------------------------------------------------------
# Handler - Passport

    def cloudSync(self):
        parser = argparse.ArgumentParser(
            description= 'Performing cloudSync')
        syncMsg = json.dumps({"update":["PRESET","MSP","ACCOUNT"]}, indent =4)
        response = frontDoor.cloudSync(syncMsg)

# --------------------------------------------------------------------------------------
# Handler - Software Update


if __name__ == '__main__':
    Madrid()
