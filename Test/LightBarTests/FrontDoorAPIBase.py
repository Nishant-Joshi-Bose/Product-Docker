import json
class FrontDoorAPIBase:
	"""
	Parameters: api   : API to Send
		    _data : JSON data to send for POST API
	"""
	def send_api(self, api, method="",_data=""):
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
	    playbackRequest = self.send("POST",'/content/playbackRequest',data)
	    playbackRequest = json.loads(playbackRequest)
	    return playbackRequest

	# 3. Handler - NetworkService
	def ping(self):
	    print "Sending Ping"
	    pong = self.send("GET", '/network/ping')
	    pong = json.loads(pong)
	    return pong["body"]["pong"]
    	
    	def getWifiProfile(self):
	    print "Getting the Wi-Fi profiles added to the device"
	    return self.send("GET", '/network/wifi/profile')

        def addWifiProfile(self, data):
	    print "Adding Wi-Fi profile to the device"
	    return self.send("POST", '/network/wifi/profile', data)
	
    	def sendSiteScan(self, data):
	    print "Sending the site-scan results"
	    return self.send("POST", '/network/wifi/site_scan', data)

        def getAPModeStatus(self):
	    print "Setting AP mode status on the device"
	    return self.send("GET", '/network/wifi/ap')

        def setAPMode(self, data):
            print "Setting the AP Mode"
	    return self.send("POST", '/network/wifi/ap', data)
               
    	def getnetworkWiFiStatus(self):
	    print "Getting the network WiFi Status"
	    return self.send("GET", '/network/wifi/status')

	# 4. Handler - FrontDoor
	def getCapabilities(self):
	    print "Getting the List of API's for this build"
	    return self.send("GET", '/system/capabilities')

	# 5. Handler - LightBarController
	def getActiveAnimation(self):
		print "Getting currently active animation"
		lightbar_status =  self.send("GET",'/ui/lightbar')
		lightbar_status = json.loads (lightbar_status)
		return lightbar_status["body"]

	def playLightBarAnimation(self, data):
		print "Request Lightbar to play animation"
		lightbar_status = self.send("PUT",'/ui/lightbar',data)
                print("#############################################################")
                print("lightbar_status is :-- ",lightbar_status)
                print("##############################################################")
		lightbar_status = json.loads(lightbar_status)
		return lightbar_status["body"]

	def stopActiveAnimation(self,data):
		print "Request to stop active animation"
		lightbar_status = self.send("DELETE",'/ui/lightbar',data)
                print("#############################################################")
                print("lightbar_status is :-- ",lightbar_status)
                print("##############################################################")
		lightbar_status = json.loads(lightbar_status)
		return lightbar_status ["body"]
