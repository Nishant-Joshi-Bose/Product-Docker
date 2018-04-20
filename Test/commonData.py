"""
This common dictionary contains keyConfig details used for automated tests
"""
keyConfig= \
    { "keyTable":
        [
        {"origin": 0, "keyEvent": 1, "keyList": [4], "timeOutList": [0], "action": 1001, "comment": "Console: MFB release between 0-1750 ms ---> play-pause"},
        {"origin": 0, "keyEvent": 4, "keyList": [4,4], "timeOutList": [500], "action": 1002, "comment": "Console: MFB repeat release twice within 500ms ---> next track"},
        {"origin": 0, "keyEvent": 4, "keyList": [4,4,4], "timeOutList": [500], "action": 1003, "comment": "Console: MFB repeat release thrice within 500ms ---> prev track"},
        {"origin": 0, "keyEvent": 1, "keyList": [4], "timeOutList": [1750], "action": 1004, "comment": "Console: MFB release after 1750 ms ---> Network Standby"},
        {"origin": 0, "keyEvent": 2, "keyList": [2,4], "timeOutList": [5020], "action": 1005, "comment": "Console: Aux+MFB press and hold for at least 5020ms ---> Manual setup Mode"},
        {"origin": 0, "keyEvent": 2, "keyList": [2,3], "timeOutList": [5020], "action": 1006, "comment": "Console: AUX+VolumeUp press and hold for at least 5020ms ---> Manual Update"},
        {"origin": 0, "keyEvent": 2, "keyList": [1,4], "timeOutList": [5020], "action": 1007, "comment": "Console: Bluetoot+MFB press and hold for at least 5020ms ---> Toggle WiFi Radio"},
        {"origin": 0, "keyEvent": 2, "keyList": [2,5], "timeOutList": [10020], "action": 1008, "comment": "Console: AUX+VolumeDown press and hold for at lease 10020ms ---> Factory Default"},
        {"origin": 0, "keyEvent": 2, "keyList": [4], "timeOutList": [15000], "action": 1009, "comment": "Console: MFB press and hold for at least 15000ms ---> Low Power Standby"},

        {"origin": 0, "keyEvent": 1, "keyList": [1], "timeOutList": [0], "action": 1020, "comment": "Console: Bluetooth release ---> CarouselList-Discoverable-ConnectToLast"},
        {"origin": 0, "keyEvent": 2, "keyList": [1], "timeOutList": [750], "action": 1021, "comment": "Console: Bluetooth Press and Hold ---> SendToDiscoverable"},
        {"origin": 0, "keyEvent": 2, "keyList": [1], "timeOutList": [10000], "action": 1022, "comment": "Console: Bluetooth Press and Hold ---> Clear Pairing List"},

        {"origin": 0, "keyEvent": 1, "keyList": [3], "timeOutList": [20], "action": 23, "comment": "Console: VolumeUp(+) release ---> VolumeUp"},
        {"origin": 0, "keyEvent": 3, "keyList": [3], "timeOutList": [300,100], "action": 1039, "comment": "Console: VolumeUp(+) Press and Hold ---> VolumeRampUp"},

        {"origin": 0, "keyEvent": 1, "keyList": [5], "timeOutList": [20], "action": 24, "comment": "Console: VolumeUp(-) release ---> VolumeDown"},
        {"origin": 0, "keyEvent": 3, "keyList": [5], "timeOutList": [300,100], "action": 1040, "comment": "Console: VolumeUp(-) Press and Hold ---> VolumeRampDown"},

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
        {"origin": 0, "keyEvent": 1, "keyList": [13], "timeOutList": [200], "action": 1038, "comment": "Console: Preset 6 release ---> Select Preset stored on Key 6"},

        {"origin": 0, "keyEvent": 5, "keyList": [3], "timeOutList": [], "action": 1050, "comment": "Console: VolumeUp(+) release(keyEvent: Release Anyways ---> Cancel Ramp up"},
        {"origin": 0, "keyEvent": 5, "keyList": [5], "timeOutList": [], "action": 1051, "comment": "Console: VolumeDown(-) release(keyEvent: Release Anyways ---> Cancel Ramp down"},

        {"origin": 0, "keyEvent": 3, "keyList": [2,3], "timeOutList": [5,1000], "action": 1111, "comment": "Console: AUX+VolumeUp press and hold will generate generate first event at 5ms and repeat every 1000ms ---> Manual Update Count down"},
        {"origin": 0, "keyEvent": 5, "keyList": [2], "timeOutList": [], "action": 1121, "comment": "Console: Release of AUX will generate ---> Manual Update Cancel"},
        {"origin": 0, "keyEvent": 5, "keyList": [3], "timeOutList": [], "action": 1121, "comment": "Console: Release of VolumeUp will generate ---> Manual Update Cancel"},

        {"origin": 0, "keyEvent": 3, "keyList": [2,5], "timeOutList": [5,1000], "action": 1112, "comment": "Console: AUX+VolumeDown press and hold will generate generate first event at 5ms and repeat every 1000ms ---> Factory Default Count down"},
        {"origin": 0, "keyEvent": 5, "keyList": [2], "timeOutList": [], "action": 1122, "comment": "Console: Release of AUX will generate ---> Factory Default Cancel"},
        {"origin": 0, "keyEvent": 5, "keyList": [5], "timeOutList": [], "action": 1122, "comment": "Console: Release of VolumeDown will generate ---> Factory Default Cancel"},

        {"origin": 0, "keyEvent": 3, "keyList": [2,4], "timeOutList": [5,1000], "action": 1113, "comment": "Console: Aux+MFB press and hold will generate generate first event at 5ms and repeat every 1000ms---> Manual setup Count down"},
        {"origin": 0, "keyEvent": 5, "keyList": [2], "timeOutList": [], "action": 1123, "comment": "Console: Release of AUX will generate ---> Manual setup Cancel"},
        {"origin": 0, "keyEvent": 5, "keyList": [4], "timeOutList": [], "action": 1123, "comment": "Console: Release of MFB will generate ---> Manual setup Cancel"},

        {"origin": 0, "keyEvent": 3, "keyList": [1,4], "timeOutList": [5,1000], "action": 1114, "comment": "Console: Bluetooth+MFB press and hold will generate first event at 5ms and repeat every 1000ms ---> Toggle WiFi Radio Count down"},
        {"origin": 0, "keyEvent": 5, "keyList": [1], "timeOutList": [], "action": 1124, "comment": "Console: Release of Bluetooth will generate ---> Toggle WiFi Radio Cancel"},
        {"origin": 0, "keyEvent": 5, "keyList": [4], "timeOutList": [], "action": 1124, "comment": "Console: Release of MFB will generate ---> Toggle WiFi Radio Cancel"},

        {"origin": 0, "keyEvent": 2, "keyList": [3,5], "timeOutList": [5020], "action": 1105, "comment": "Console: VolumeUp+VolumeDown press and release during count down ---> System Info"},
        {"origin": 0, "keyEvent": 3, "keyList": [3,5], "timeOutList": [5,1000], "action": 1115, "comment": "Console: VolumeUp+VolumeDown press and hold will generate first event at 5ms and repeat every 1000ms ---> System Info Count down"},
        {"origin": 0, "keyEvent": 5, "keyList": [3], "timeOutList": [], "action": 1125, "comment": "Console: Release of VolumeUP will generate ---> System Info Cancel"},
        {"origin": 0, "keyEvent": 5, "keyList": [5], "timeOutList": [], "action": 1125, "comment": "Console: Release of VolumeDown will generate ---> System Info Cancel"}
        ]
    }