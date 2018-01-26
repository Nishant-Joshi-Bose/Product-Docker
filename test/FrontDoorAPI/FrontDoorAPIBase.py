import json

class FrontDoorAPIBase:
    """
    Class that defines getters and setters for API's defined
    """
    def send_api(self, api, method="", _data=""):
        """
        Parameters: api   : API to Send
                _data : JSON data to send for POST API
        """
        print "Sending API", api
        if _data == "":
            return self.send("GET", api)
        else:
            if method == "":
                return self.send("POST", api, _data)
            else:
                return self.send(method, api, _data)

    # Individual Get & Set functions for each implemented API

    # 1. Handler - Product Eddie
    def getLanguage(self):
        """ Returns the Language """
        print "Getting the language"
        language_list = self.send("GET", '/system/language')
        language_list = json.loads(language_list)
        return language_list["body"]["code"]

    def setLanguage(self, data):
        """ Sets the Language """
        language = self.send("POST", '/system/language', data)
        language = json.loads(language)
        return language["body"]["code"]

    def getState(self):
        """ Returns product State """
        state = self.send("GET", '/system/state')
        state = json.loads(state)
        return state["body"]["state"]

    def getconfigurationStatus(self):
        """ Returns configuration Status """
        configStatus = self.send("GET", '/system/configuration/status')
        configStatus = json.loads(configStatus)
        return configStatus["body"]["status"]["account"],\
               configStatus["body"]["status"]["language"],\
               configStatus["body"]["status"]["network"]

    def getInfo(self):
        """ Returns Product Info """
        print "Getting the Info from the Product"
        inf = self.send("GET", '/system/info')
        inf = json.loads(inf)
        return inf["body"]["GUID"],\
               inf["body"]["countryCode"],\
               inf["body"]["moduleType"],\
               inf["body"]["name"],\
               inf["body"]["serialNumber"],\
               inf["body"]["type"],\
               inf["body"]["variant"]

    def getDemoMode(self):
        """ Returns demo flag state """
        demoMode = self.send("GET", '/demo')
        demoMode = json.loads(demoMode)
        return demoMode["body"]["demoMode"]

    def setDemo(self, data):
        """ Sets the demo Mode """
        demoMode = self.send("PUT", '/demo', data)
        demoMode = json.loads(demoMode)
        return demoMode["body"]["demoMode"]

    # 2. Handler - CAPS
    def getSources(self):
        """ Returns the Source List """
        print "Getting the Sources List"
        sourceList = self.send("GET", '/system/sources')
        sourceList = json.loads(sourceList)
        return sourceList

    def getCapsIntializationStatus(self):
        """ Checks for CAPS Initialization """
        print "Getting the CAPS status"
        capsStatus = self.send("GET", '/system/capsInitializationStatus')
        capsStatus = json.loads(capsStatus)
        return capsStatus["body"]["CapsInitialized"]

    def getNowPlaying(self):
        """ Returns nowPlaying Information """
        print "Getting the nowPlaying information"
        nowplaying = self.send("GET", '/content/nowPlaying')
        nowplaying = json.loads(nowplaying)
        return nowplaying

    def sendPlaybackRequest(self, data):
        """ Sends playbackRequest msg """
        print "Send playback request"
        playbackRequest = self.send("POST", '/content/playbackRequest', data)
        playbackRequest = json.loads(playbackRequest)
        return playbackRequest

    def stopPlaybackRequest(self):
        """ Sends /stopPlayback; deactivates the source """
        print "Stop/Deactivate the source"
        stopPlaybackResponse = self.send("GET", '/content/stopPlayback')
        stopPlaybackResponse = json.loads(stopPlaybackResponse)
        return stopPlaybackResponse

    def sendTransportControl(self, data):
        """ Sends transportControl operations """
        print "Set transportcontrol"
        transportControl = self.send("PUT", '/content/transportControl', data)
        transportControl = json.loads(transportControl)
        return transportControl

    def getVolume(self):
        """ Returns the response to /volume """
        print "Getting the current volume"
        volume = self.send("GET", '/audio/volume')
        volume = json.loads(volume)
        return volume

    def sendVolume(self, data):
        """ Sets the Volume level """
        print "Sending Volume Level"
        volume = self.send("PUT", '/audio/volume', data)
        volume = json.loads(volume)
        return volume["body"]["value"]

    def sendMinVolume(self, data):
        """ Sets the minimum threshold volume """
        print "Setting the minimum Volume"
        minVolume = self.send("POST", '/audio/volumeMin', data)
        minVolume = json.loads(minVolume)
        return minVolume

    def sendMaxVolume(self, data):
        """ Sets the maximum threshold volume """
        print "Setting the Maximum Volume"
        maxVolume = self.send("POST", '/audio/volumeMax', data)
        maxVolume = json.loads(maxVolume)
        return maxVolume
    
    def sendAbsoluteMinVolume(self, data):
        """ 
        Sets the minimum volume
        User cannot set volume below this
        """
        print "Setting the minimum Volume"
        minVolume = self.send("POST", '/audio/volume', data)
        minVolume = json.loads(minVolume)
        return minVolume

    def sendAbsoluteMaxVolume(self, data):
        """ 
        Sets the absolute maximum volume
        User cannot set volume above this 
        """
        print "Setting the Maximum Volume"
        maxVolume = self.send("POST", '/audio/volume', data)
        maxVolume = json.loads(maxVolume)
        return maxVolume

    def sendDefaultVolume(self, data):
        """ Sets the Default volume """
        print "Setting the default Volume"
        defaultVolume = self.send("POST", '/audio/defaultVolume', data)
        defaultVolume = json.loads(defaultVolume)
        return defaultVolume["body"]["defaultOn"]

    def getDefaultVolume(self):
        """ Returns the default Volume """
        print "Setting the default Volume"
        defaultVolume = self.send("GET", '/audio/defaultVolume')
        defaultVolume = json.loads(defaultVolume)
        return defaultVolume["body"]["defaultOn"]

    def sendIncrementVolume(self, data):
        """ Increments the Volume by given level """
        print "Incrementing Volume"
        incrementVolume = self.send("PUT", '/audio/volume/increment', data)
        incrementVolume = json.loads(incrementVolume)
        return incrementVolume

    def sendDecrementVolume(self, data):
        """ Decrements the Volume by given level """
        print "Decrementing Volume"
        decrementVolume = self.send("PUT", '/audio/volume/decrement', data)
        decrementVolume = json.loads(decrementVolume)
        return decrementVolume

    def addZone(self, data):
        """ Create New Zone """
        print "Add Zone Request"
        zoneDetails = self.send("POST", '/audio/zone', data)
        zoneDetails = json.loads(zoneDetails)
        return zoneDetails

    def addSlaveInZone(self, data):
        """ Add new slave in Zone """
        print "Add slave in Zone Request"
        zoneSlaveDetails = self.send("PUT", '/audio/zone', data)
        zoneSlaveDetails = json.loads(zoneSlaveDetails)
        return zoneSlaveDetails

    def deleteZone(self, data):
        """ Delete Zone """
        print "Delete Zone Request"
        zoneDetails = self.send("DELETE", '/audio/zone', data)
        zoneDetails = json.loads(zoneDetails)
        return zoneDetails

    def getZone(self):
        """ Get Zone Info """
        print "Get Zone Request"
        zoneDetails = self.send("GET", '/audio/zone')
        zoneDetails = json.loads(zoneDetails)
        return zoneDetails

    # 3. Handler - NetworkService
    def ping(self):
        """ Sending Ping """
        pong = self.send("GET", '/network/ping')
        pong = json.loads(pong)
        return pong["body"]["pong"]

    def getWifiProfile(self):
        """ Gett the Wi-Fi profiles added to the device """
        wifiProfiles = self.send("GET", '/network/wifi/profile')
        wifiProfiles = json.loads(wifiProfiles)
        return wifiProfiles

    def addWifiProfile(self, data):
        """ Add Wi-Fi profile to the device """
        addWifiProfileResponse = self.send("POST", '/network/wifi/profile', data)
        addWifiProfileResponse = json.loads(addWifiProfileResponse)
        return addWifiProfileResponse

    def getNetworkStatus(self):
        """ Get Network Status """
        networkStatus = self.send("GET", '/network/status')
        networkStatus = json.loads(networkStatus)
        return networkStatus

    def sendSiteScan(self, data):
        """ Get site-scan results """
        siteSurvey = self.send("POST", '/network/wifi/siteScan', data)
        siteSurvey = json.loads(siteSurvey)
        return siteSurvey

    def getAPModeStatus(self):
        """ Get AP mode status on the device """
        apStatus = self.send("GET", '/network/wifi/ap')
        apStatus = json.loads(apStatus)
        return apStatus

    def setAPMode(self, data):
        """ Set AP Mode on the device """
        apMode = self.send("POST", '/network/wifi/ap', data)
        apMode = json.loads(apMode)
        return apMode

    def getNetworkWiFiStatus(self):
        """ Get network WiFi Status """
        wifiStatus = self.send("GET", '/network/wifi/status')
        wifiStatus = json.loads(wifiStatus)
        return wifiStatus

    # 4. Handler - FrontDoor
    def getCapabilities(self):
        """ Getting the List of API's for this build """
        apis = self.send("GET", '/system/capabilities')
        apis = json.loads(apis)
        return apis

    # 5. Handler - LightBarController
    def getActiveAnimation(self):
        """ Get current active animation """
        lightbar_status = self.send("GET", '/ui/lightbar')
        lightbar_status = json.loads(lightbar_status)
        return lightbar_status["body"]

    def playLightBarAnimation(self, data):
        """" Request Lightbar to play animation """
        lightbar_status = self.send("PUT", '/ui/lightbar', data)
        lightbar_status = json.loads(lightbar_status)
        return lightbar_status["body"]

    def stopActiveAnimation(self, data):
        """ Stopping the current active animation """
        lightbar_status = self.send("DELETE", '/ui/lightbar', data)
        lightbar_status = json.loads(lightbar_status)
        return lightbar_status["body"]

    #    Handler - FrontDoor BLE
    def setBLESetupStatus(self, data):
        """ Set BLE advertising status"""
        bleSetupStatus = self.send("POST", '/bluetooth/BLESetup/activate', data)
        bleSetupStatus = json.loads(bleSetupStatus)
        return bleSetupStatus

    def getBLESetupStatus(self):
        """ Get BLE advertising status"""
        bleStatus = self.send("GET", '/bluetooth/BLESetup/status')
        bleStatus = json.loads(bleStatus)
        return bleStatus["body"]["state"]

    def getMacAddr(self):
        """ Get BT sink mac addr (same mac for BT Source) """
        macAddr = self.send("GET", "/bluetooth/sink/macAddr")
        macAddr = json.loads(macAddr)
        return macAddr["body"]["mac"]

    # 6. Handler - FrontDoor BT Sink
    def sinkGoPairable(self):
        """ Set BT Sink into pairable/connectable state """
        pairableResponse = self.send("POST", "/bluetooth/sink/pairable", "{}")
        pairableResponse = json.loads(pairableResponse)
        return pairableResponse

    def sinkClearAllConnectedDevices(self):
        """ BT Sink clear connected devices list """
        clearConnecteList = self.send("POST", "/bluetooth/sink/removeAll", "{}")
        clearConnecteList = json.loads(clearConnecteList)
        return clearConnecteList

    def sinkGetConnectedDevicesList(self):
        """ BT Sink get connected BT device list"""
        getConnectedDeviceList = self.send("GET", "/bluetooth/sink/list")
        getConnectedDeviceList = json.loads(getConnectedDeviceList)
        macAddressList = []
        for device in getConnectedDeviceList["body"]["devices"]:
            macAddressList.append(device["mac"])
        return macAddressList

    def sinkGetAppStatus(self):
        """ Get BT Sink app status """
        sinkAppStatus = self.send("GET", "/bluetooth/sink/status")
        sinkAppStatus = json.loads(sinkAppStatus)
        return sinkAppStatus["body"]

    def sinkConnectToRemoteDevice(self, data):
        """ BT Sink connect to remote device
            data = { "mac":"ff:ff:ff:ff:ff:ff" }
        """
        sinkConnectRemoteDevice = self.send("POST", "/bluetooth/sink/connect", data)
        sinkConnectRemoteDevice = json.loads(sinkConnectRemoteDevice)
        return sinkConnectRemoteDevice

    def sinkDisconnectFromRemoteDevice(self, data):
        """ BT Sink disconnect from remote device
            data = { "mac":"ff:ff:ff:ff:ff:ff" }
        """
        sinkDisconnect = self.send("POST", "/bluetooth/sink/disconnect", data)
        sinkDisconnect = json.loads(sinkDisconnect)
        return sinkDisconnect

    # 7. Handler - FrontDoor BT Source
    def sourcePairDevice(self, data):
        """ BT Source pair to remote device
            data = { "mac":"ff:ff:ff:ff:ff:ff" }
        """
        sourcePairDevice = self.send("POST", "/bluetooth/source/pair", data)
        sourcePairDevice = json.loads(sourcePairDevice)
        return sourcePairDevice

    def sourceConnectToRemoteDevice(self, data):
        """ BT Source connect to remote device
            data = { "mac":"ff:ff:ff:ff:ff:ff" }
        """
        sourceConnect = self.send("POST", "/bluetooth/source/connect", data)
        sourceConnect = json.loads(sourceConnect)
        return sourceConnect

    def sourceDisconnectFromRemoteDevice(self, data):
        """ BT Source disconnect from remote device
            data = { "mac":"ff:ff:ff:ff:ff:ff" }
        """
        sourceDisconnect = self.send("POST", "/bluetooth/source/disconnect", data)
        sourceDisconnect = json.loads(sourceDisconnect)
        return sourceDisconnect

    def sourceVolumeToRemoteDevice(self, data):
        """ BT Source change volume
            data = {"volume":"10"}
        """
        sourceVolume = self.send("POST", "/bluetooth/source/volume", data)
        sourceVolume = json.loads(sourceVolume)
        return sourceVolume

    def sourceGetConnectedRemoteDevices(self):
        """ BT Source get connected device """
        connected_device = self.send("GET", "/bluetooth/source/connect")
        connected_device = json.loads(connected_device)
        return connected_device["body"]

    # 8. Handler - Passport
    def cloudSync(self, data):
        """ Sends /cloudSync - Syncs 'ALL' """
        sync = self.send("POST", "/cloudSync", data)
        sync = json.loads(sync)
        return sync
