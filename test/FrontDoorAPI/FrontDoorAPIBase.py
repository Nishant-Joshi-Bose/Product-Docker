import json
class FrontDoorAPIBase:
	"""
	Parameters: api   : API to Send
		    _data : JSON data to send for POST API
	"""
	def send_api(self, api, _data=""):
	    print "Sending API", api
	    if _data == "":
	        return self.send("GET", api)
	    else:
                return self.send("POST", api, _data)

        # Individual Get & Set functions for each implemented API
	
	# 1. Handler - Product Eddie
	def getLanguage(self):
	    print "Getting the language"
	    language_list = self.send("GET", '/system/language')
	    language_list = json.loads(language_list)
	    return language_list["body"]["code"]

        def setLanguage(self, data):
	    language = self.send("POST", '/system/language', data)
	    language = json.loads(language)
	    print "The set language is:", language["body"]["code"]
	    return language["body"]["code"]
    	
        def getState(self):
	    state = self.send("GET", '/system/state')
	    state = json.loads(state)
	    print "The product state is:", state["body"]["state"]
	    return state["body"]["state"]

    	def getconfigurationStatus(self):
	    configStatus = self.send("GET", '/system/configuration/status')
	    configStatus = json.loads(configStatus)
	    return configStatus["body"]["status"]["account"],\
	           configStatus["body"]["status"]["language"],\
		   configStatus["body"]["status"]["network"]

        def getInfo(self):
	    print "Getting the Info from the Product"
	    inf = self.send("GET", '/system/info')
	    inf = json.loads(inf)
	    return inf["body"]["GUID"],\
	           inf["body"]["country"],\
		   inf["body"]["moduleType"],\
		   inf["body"]["regionCode"],\
		   inf["body"]["serialNumber"],\
		   inf["body"]["type"],\
		   inf["body"]["variant"]

    	# 2. Handler - CAPS
	def getSources(self):
	    print "Getting the Sources List"
	    sourceList = self.send("GET",'/system/sources')
	    return sourceList 
    
    	def getCapsIntializationStatus(self):
	    print "Getting the CAPS status"
	    capsStatus = self.send("GET", '/system/capsInitializationStatus')
	    capsStatus = json.loads(capsStatus)
	    return capsStatus["body"]["CapsInitialized"]

    	def getNowPlaying(self):
	    print "Getting the nowPlaying information"
	    nowplaying = self.send("GET", '/content/nowPlaying')
	    nowplaying = json.loads(nowplaying)
	    return nowplaying

	def sendPlaybackRequest(self, data):
            print "Send playback request"
            playbackRequest= self.send("POST",'/content/playbackRequest', data)
	    playbackRequest = json.loads(playbackRequest)
	    return playbackRequest

	def sendTransportControl(self, data):
	    print "Set transportcontrol"
	    transportControl = self.send("POST", '/content/transportControl', data)
            transportControl = json.loads(transportcontrol)
	    return transportControl

	# 3. Handler - NetworkService
	def ping(self):
	    print "Sending Ping"
	    pong = self.send("GET", '/network/ping')
	    pong = json.loads(pong)
	    return pong["body"]["pong"]
    	
    	def getWifiProfile(self):
	    print "Getting the Wi-Fi profiles added to the device"
	    wifiProfiles = self.send("GET", '/network/wifi/profile')
	    print "The wifiProfile's are:", wifiProfiles
	    wifiProfiles = json.loads(wifiProfiles)
	    return wifiProfiles 

        def addWifiProfile(self, data):
	    print "Adding Wi-Fi profile to the device"
	    return self.send("POST", '/network/wifi/profile', data)
	
    	def getNetworkStatus(self):
            print "Getting Network Status"
	    networkStatus = self.send("GET", '/network/status')
	    print "The NetworkStatus is:\n", networkStatus
	    networkStatus = json.loads(networkStatus)
	    return networkStatus

    	def sendSiteScan(self, data):
	    print "Fetching the site-scan results"
	    siteSurvey = self.send("POST", '/network/wifi/site_scan', data)
            siteSurvey = json.loads(siteSurvey)
	    return siteSurvey

        def getAPModeStatus(self):
	    print "Setting AP mode status on the device"
	    apStatus = self.send("GET", '/network/wifi/ap')
	    apStatus = json.loads(apStatus)
	    return apStatus

        def setAPMode(self, data):
            print "Setting the AP Mode"
	    apMode =  self.send("POST", '/network/wifi/ap', data)
            apMode = json.loads(apMode)
	    return apMode

    	def getNetworkWiFiStatus(self):
	    print "Getting the network WiFi Status"
	    wifiStatus = self.send("GET", '/network/wifi/status')
	    wifiStatus = json.loads(wifiStatus)
	    return wifiStatus

	# 4. Handler - FrontDoor
	def getCapabilities(self):
	    print "Getting the List of API's for this build"
	    return self.send("GET", '/system/capabilities')
