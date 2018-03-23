# clikey_data.py
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
'''
CLI Key positive and negetive scenarios for key events

keyEvent    : Integer that descirbes the CLI key events class
origin      : Eddie console has origin value of 0
keyList     : List of different keyids
timeOutList : This is a list(maximum size is 2) of timing information will be used by various keyEvents
action      : This is action to be translated when a key is pressed, released, held
comment     : This is just a description that is used to define the entry
'''

# Positive keyEvent scenarios
positive_scenarios =[
    {"origin": 0, "keyEvent": 1, "keyList": [4], "timeOutList": [0], "action": 1, "comment": "Console: MFB release between 0-1750 ms ---> play-pause"},
    {"origin": 0, "keyEvent": 4, "keyList": [4,4], "timeOutList": [500], "action": 2, "comment": "Console: MFB repeat release twice within 500ms ---> next track"},
    {"origin": 0, "keyEvent": 4, "keyList": [4,4,4], "timeOutList": [500], "action": 3, "comment": "Console: MFB repeat release thrice within 500ms ---> prev track"},
    {"origin": 0, "keyEvent": 1, "keyList": [4], "timeOutList": [1750], "action": 4, "comment": "Console: MFB release after 1750 ms ---> Network Standby"},
    {"origin": 0, "keyEvent": 2, "keyList": [2,4], "timeOutList": [5020], "action": 5, "comment": "Console: Aux+MFB press and hold for at least 5020ms ---> Manual setup Mode"},
    {"origin": 0, "keyEvent": 2, "keyList": [2,3], "timeOutList": [5020], "action": 6, "comment": "Console: AUX+VolumeUp press and hold for at least 5020ms ---> Manual Update"},
    {"origin": 0, "keyEvent": 2, "keyList": [1,4], "timeOutList": [5020], "action": 7, "comment": "Console: Bluetoot+MFB press and hold for at least 5020ms ---> Toggle WiFi Radio"},
    {"origin": 0, "keyEvent": 2, "keyList": [4], "timeOutList": [15000], "action": 9, "comment": "Console: MFB press and hold for at least 15000ms ---> Low Power Standby"},

    {"origin": 0, "keyEvent": 1, "keyList": [1], "timeOutList": [0], "action": 20, "comment": "Console: Bluetooth release ---> CarouselList-Discoverable-ConnectToLast"},
    {"origin": 0, "keyEvent": 2, "keyList": [1], "timeOutList": [750], "action": 21, "comment": "Console: Bluetooth Press and Hold ---> SendToDiscoverable"},
    {"origin": 0, "keyEvent": 2, "keyList": [1], "timeOutList": [10000], "action": 22, "comment": "Console: Bluetooth Press and Hold ---> Clear Pairing List"},

    {"origin": 0, "keyEvent": 1, "keyList": [3], "timeOutList": [0], "action": 23, "comment": "Console: VolumeUp(+) release ---> VolumeUp"},
    {"origin": 0, "keyEvent": 3, "keyList": [3], "timeOutList": [300,100], "action": 39, "comment": "Console: VolumeUp(+) Press and Hold ---> VolumeRampUp"},

    {"origin": 0, "keyEvent": 1, "keyList": [5], "timeOutList": [0], "action": 24, "comment": "Console: VolumeUp(-) release ---> VolumeDown"},
    {"origin": 0, "keyEvent": 3, "keyList": [5], "timeOutList": [300,100], "action": 40, "comment": "Console: VolumeUp(-) Press and Hold ---> VolumeRampDown"},

    {"origin": 0, "keyEvent": 1, "keyList": [2], "timeOutList": [0], "action": 25, "comment": "Console: AUX release ---> SendToAux"},

    {"origin": 0, "keyEvent": 1, "keyList": [6], "timeOutList": [0], "action": 26, "comment": "Console: action release ---> Alexa"},

    {"origin": 0, "keyEvent": 2, "keyList": [8], "timeOutList": [1000], "action": 27, "comment": "Console: Preset 1 press and hold for at least 1000ms ---> Store Currently playing into Preset 1"},
    {"origin": 0, "keyEvent": 2, "keyList": [9], "timeOutList": [1000], "action": 28, "comment": "Console: Preset 2 press and hold for at least 1000ms ---> Store Currently playing into Preset 2"},
    {"origin": 0, "keyEvent": 2, "keyList": [10], "timeOutList": [1000], "action": 29, "comment": "Console: Preset 3 press and hold for at least 1000ms ---> Store Currently playing into Preset 3"},
    {"origin": 0, "keyEvent": 2, "keyList": [11], "timeOutList": [1000], "action": 30, "comment": "Console: Preset 4 press and hold for at least 1000ms ---> Store Currently playing into Preset 4"},
    {"origin": 0, "keyEvent": 2, "keyList": [12], "timeOutList": [1000], "action": 31, "comment": "Console: Preset 5 press and hold for at least 1000ms ---> Store Currently playing into Preset 5"},
    {"origin": 0, "keyEvent": 2, "keyList": [13], "timeOutList": [1000], "action": 32, "comment": "Console: Preset 6 press and hold for at least 1000ms ---> Store Currently playing into Preset 6"},

    {"origin": 0, "keyEvent": 1, "keyList": [8], "timeOutList": [200], "action": 33, "comment": "Console: Preset 1 release ---> Select Preset stored on Key 1"},
    {"origin": 0, "keyEvent": 1, "keyList": [9], "timeOutList": [200], "action": 34, "comment": "Console: Preset 2 release ---> Select Preset stored on Key 2"},
    {"origin": 0, "keyEvent": 1, "keyList": [10], "timeOutList": [200], "action": 35, "comment": "Console: Preset 3 release ---> Select Preset stored on Key 3"},
    {"origin": 0, "keyEvent": 1, "keyList": [11], "timeOutList": [200], "action": 36, "comment": "Console: Preset 4 release ---> Select Preset stored on Key 4"},
    {"origin": 0, "keyEvent": 1, "keyList": [12], "timeOutList": [200], "action": 37, "comment": "Console: Preset 5 release ---> Select Preset stored on Key 5"},
    {"origin": 0, "keyEvent": 1, "keyList": [13], "timeOutList": [200], "action": 38, "comment": "Console: Preset 6 release ---> Select Preset stored on Key 6"},

    {"origin": 0, "keyEvent": 5, "keyList": [3], "timeOutList": [], "action": 50, "comment": "Console: VolumeUp(+) release(keyEvent: Release Anyways ---> Cancel Ramp up"},
    {"origin": 0, "keyEvent": 5, "keyList": [5], "timeOutList": [], "action": 51, "comment": "Console: VolumeDown(-) release(keyEvent: Release Anyways ---> Cancel Ramp down"},

    {"origin": 0, "keyEvent": 3, "keyList": [2,3], "timeOutList": [5,1000], "action": 111, "comment": "Console: AUX+VolumeUp press and hold will generate generate first event at 5ms and repeat every 1000ms ---> Manual Update Count down"},
    {"origin": 0, "keyEvent": 5, "keyList": [2], "timeOutList": [], "action": 121, "comment": "Console: Release of AUX will generate ---> Manual Update Cancel"},
    {"origin": 0, "keyEvent": 5, "keyList": [3], "timeOutList": [], "action": 121, "comment": "Console: Release of VolumeUp will generate ---> Manual Update Cancel"},

    {"origin": 0, "keyEvent": 3, "keyList": [2,5], "timeOutList": [5,1000], "action": 112, "comment": "Console: AUX+VolumeDown press and hold will generate generate first event at 5ms and repeat every 1000ms ---> Factory Default Count down"},
    {"origin": 0, "keyEvent": 5, "keyList": [2], "timeOutList": [], "action": 122, "comment": "Console: Release of AUX will generate ---> Factory Default Cancel"},
    {"origin": 0, "keyEvent": 5, "keyList": [5], "timeOutList": [], "action": 122, "comment": "Console: Release of VolumeDown will generate ---> Factory Default Cancel"},

    {"origin": 0, "keyEvent": 3, "keyList": [2,4], "timeOutList": [5,1000], "action": 113, "comment": "Console: Aux+MFB press and hold will generate generate first event at 5ms and repeat every 1000ms---> Manual setup Count down"},
    {"origin": 0, "keyEvent": 5, "keyList": [2], "timeOutList": [], "action": 123, "comment": "Console: Release of AUX will generate ---> Manual setup Cancel"},
    {"origin": 0, "keyEvent": 5, "keyList": [4], "timeOutList": [], "action": 123, "comment": "Console: Release of MFB will generate ---> Manual setup Cancel"},

    {"origin": 0, "keyEvent": 3, "keyList": [1,4], "timeOutList": [5,1000], "action": 114, "comment": "Console: Bluetooth+MFB press and hold will generate first event at 5ms and repeat every 1000ms ---> Toggle WiFi Radio Count down"},
    {"origin": 0, "keyEvent": 5, "keyList": [1], "timeOutList": [], "action": 124, "comment": "Console: Release of Bluetooth will generate ---> Toggle WiFi Radio Cancel"},
    {"origin": 0, "keyEvent": 5, "keyList": [4], "timeOutList": [], "action": 124, "comment": "Console: Release of MFB will generate ---> Toggle WiFi Radio Cancel"},

    {"origin": 0, "keyEvent": 2, "keyList": [3,5], "timeOutList": [5020], "action": 105, "comment": "Console: VolumeUp+VolumeDown press and release during count down ---> System Info"},
    {"origin": 0, "keyEvent": 3, "keyList": [3,5], "timeOutList": [5,1000], "action": 115, "comment": "Console: VolumeUp+VolumeDown press and hold will generate first event at 5ms and repeat every 1000ms ---> System Info Count down"},
    {"origin": 0, "keyEvent": 5, "keyList": [3], "timeOutList": [], "action": 125, "comment": "Console: Release of VolumeUP will generate ---> System Info Cancel"},
    {"origin": 0, "keyEvent": 5, "keyList": [5], "timeOutList": [], "action": 125, "comment": "Console: Release of VolumeDown will generate ---> System Info Cancel"},
    {"origin": 0, "keyEvent": 2, "keyList": [2,5], "timeOutList": [10020], "action": 8, "comment": "Console: AUX+VolumeDown press and hold for at lease 10020ms ---> Factory Default"}
]

# Negative keyEvent scenarios
negative_scenarios =[
     {"origin": 0, "keyEvent": 2, "keyList": [8,9], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [8,10], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [8,11], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [8,12], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [8,13], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [9,10], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [9,11], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [9,12], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [9,13], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [10,11], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [10,12], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [10,13], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [11,12], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [11,13], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [12,13], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [8,9,10], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [8,9,11], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [8,9,12], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [8,9,13], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [8,10,11], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [8,10,12], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [8,10,13], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [8,11,12], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [8,11,13], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [8,12,13], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [9,10,11], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [9,10,12], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [9,10,13], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [9,11,12], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [9,11,13], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [10,11,12], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [10,11,13], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [10,12,13], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 2, "keyList": [11,12,13], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 4, "keyList": [8,8,8], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 4, "keyList": [9,9,9], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 4, "keyList": [10,10,10], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 4, "keyList": [11,11,11], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 4, "keyList": [12,12,12], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 4, "keyList": [13,13,13], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 4, "keyList": [8,8], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 4, "keyList": [9,9], "timeOutList": [500]},
     {"origin": 0, "keyEvent": 4, "keyList": [10,10], "timeOutList": [100]},
     {"origin": 0, "keyEvent": 4, "keyList": [11,11], "timeOutList": [100]},
     {"origin": 0, "keyEvent": 4, "keyList": [12,12], "timeOutList": [100]},
     {"origin": 0, "keyEvent": 4, "keyList": [13,13], "timeOutList": [100]}
]
