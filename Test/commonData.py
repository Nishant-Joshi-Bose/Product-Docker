"""
This common dictionary contains keyConfig details used for automated tests
"""
keyConfig= \
    { "keyTable":
        [
        {"origin": 0, "keyEvent": 1, "keyList": [4], "timeOutList": [0], "action": 1001, "comment": "Console: MFB release between 0-1750 ms ---> play-pause"},
        {"origin": 0, "keyEvent": 4, "keyList": [4,4], "timeOutList": [500], "action": 1002, "comment": "Console: MFB repeat release twice within 500 ms ---> next track"},
        {"origin": 0, "keyEvent": 4, "keyList": [4,4,4], "timeOutList": [500], "action": 1003, "comment": "Console: MFB repeat release thrice within 500 ms ---> prev track"},
        {"origin": 0, "keyEvent": 1, "keyList": [4], "timeOutList": [1750], "action": 1004, "comment": "Console: MFB release after 1750 ms ---> Network Standby"},
        {"origin": 0, "keyEvent": 2, "keyList": [2,4], "timeOutList": [500], "action": 1005, "comment": "Console: MFB+Aux press and hold for at least 500ms ---> AP Mode"},
        {"origin": 0, "keyEvent": 2, "keyList": [3,4], "timeOutList": [500], "action": 1006, "comment": "Console: MFB+VolumeUp press and hold for at least 500ms ---> Manual Update"},
        {"origin": 0, "keyEvent": 2, "keyList": [1,4], "timeOutList": [500], "action": 1007, "comment": "Console: MFB+Bluetooth press and hold for at least 500ms ---> Disable Networking"},
        {"origin": 0, "keyEvent": 2, "keyList": [4,5], "timeOutList": [10000], "action": 1008, "comment": "Console: MFB+VolumeDown press and hold for at least 10000 ---> Factory Reset"},
        {"origin": 0, "keyEvent": 2, "keyList": [4], "timeOutList": [15000], "action": 1009, "comment": "Console: MFB press and hold for at least 1500ms ---> Low Power Standby"},
        {"origin": 0, "keyEvent": 1, "keyList": [1], "timeOutList": [0], "action": 1020, "comment": "Console: Bluetooth release ---> CarouselList-Discoverable-ConnectToLast"},
        {"origin": 0, "keyEvent": 2, "keyList": [1], "timeOutList": [750], "action": 1021, "comment": "Console: Bluetooth Press and Hold ---> SendToDiscoverable"},
        {"origin": 0, "keyEvent": 2, "keyList": [1], "timeOutList": [10000], "action": 1022, "comment": "Console: Bluetooth Press and Hold ---> Clear Pairing List"},
        {"origin": 0, "keyEvent": 1, "keyList": [3], "timeOutList": [0], "action": 23, "comment": "Console: VolumeUp(+) release ---> VolumeUp"},
        {"origin": 0, "keyEvent": 3, "keyList": [3], "timeOutList": [300,200], "action": 1023, "comment": "Console: VolumeUp(+) Press and Hold ---> VolumeUp"},
        {"origin": 0, "keyEvent": 1, "keyList": [5], "timeOutList": [0], "action": 24, "comment": "Console: VolumeUp(-) release ---> VolumeDown"},
        {"origin": 0, "keyEvent": 3, "keyList": [5], "timeOutList": [300,200], "action": 1024, "comment": "Console: VolumeUp(-) Press and Hold ---> VolumeDown"},
        {"origin": 0, "keyEvent": 1, "keyList": [2], "timeOutList": [0], "action": 1025, "comment": "Console: AUX release ---> SendToAux"},
        {"origin": 0, "keyEvent": 1, "keyList": [6], "timeOutList": [0], "action": 1026, "comment": "Console: action release ---> Alexa"},
        {"origin": 0, "keyEvent": 2, "keyList": [8], "timeOutList": [1000], "action": 1027, "comment": "Console: Preset 1 press and hold for at least 1000ms ---> Store Currently playing into Preset 1"},
        {"origin": 0, "keyEvent": 2, "keyList": [9], "timeOutList": [1000], "action": 1028, "comment": "Console: Preset 2 press and hold for at least 1000ms ---> Store Currently playing into Preset 2"},
        {"origin": 0, "keyEvent": 2, "keyList": [10], "timeOutList": [1000], "action": 1029, "comment": "Console: Preset 3 press and hold for at least 1000ms ---> Store Currently playing into Preset 3"},
        {"origin": 0, "keyEvent": 2, "keyList": [11], "timeOutList": [1000], "action": 1030, "comment": "Console: Preset 4 press and hold for at least 1000ms ---> Store Currently playing into Preset 4"},
        {"origin": 0, "keyEvent": 2, "keyList": [12], "timeOutList": [1000], "action": 1031, "comment": "Console: Preset 5 press and hold for at least 1000ms ---> Store Currently playing into Preset 5"},
        {"origin": 0, "keyEvent": 2, "keyList": [13], "timeOutList": [1000], "action": 1032, "comment": "Console: Preset 6 press and hold for at least 1000ms ---> Store Currently playing into Preset 6"},
        {"origin": 0, "keyEvent": 1, "keyList": [8], "timeOutList": [200], "action": 1033, "comment": "Console: Preset 1 release ---> Select Preset stored on Key 1"},
        {"origin": 0, "keyEvent": 1, "keyList": [9], "timeOutList": [200], "action": 1034, "comment": "Console: Preset 2 release ---> Select Preset stored on Key 2"},
        {"origin": 0, "keyEvent": 1, "keyList": [10], "timeOutList": [200], "action": 1035, "comment": "Console: Preset 3 release ---> Select Preset stored on Key 3"},
        {"origin": 0, "keyEvent": 1, "keyList": [11], "timeOutList": [200], "action": 1036, "comment": "Console: Preset 4 release ---> Select Preset stored on Key 4"},
        {"origin": 0, "keyEvent": 1, "keyList": [12], "timeOutList": [200], "action": 1037, "comment": "Console: Preset 5 release ---> Select Preset stored on Key 5"},
        {"origin": 0, "keyEvent": 1, "keyList": [13], "timeOutList": [200], "action": 1038, "comment": "Console: Preset 6 release ---> Select Preset stored on Key 6"}
        ]
    }
