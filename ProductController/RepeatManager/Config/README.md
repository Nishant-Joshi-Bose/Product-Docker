INTRODUCTION
------------------------------------------------------------
This README describes about the how to create config file based on the different parameters.
This will help out to perform any key pressed/release/hold. Useful for any kind of key and for any products

PARAMTERS
------------------------------------------------------------
Mainly there are FIVE fields are available in the KeyCOnfiguration.json file as below
1. Keys: This field is of string array, and consist of keys in it. can be more than one as well to support key combinations
2. TimeOut: Timeout interval for the key/keycombo
3. Repeat: This field describes if the event to be performed repeatedly for every timeout interval
4. ActionOnTimeOut: This field is useful if there are many actions to be performed for different timeout intervals for the same key.
        if this field is true, then after the completion of the timeout respective event will be sentout.
5. Event: This is the name of the event for the key/key combination.


EXAMPLES
------------------------------------------------------------
Below are the types of scenarios handled in the key configuration

1. Single key pressed, action to be performed immediately as soon as the key is pressed
Example :thumbs_up key, where we need to perform action asson as the key is pressed, here main criteria is TimeOut value is 0
    {"Keys": ["thumbs_up"],  "TimeOut": 0,      "Repeat": false,    "ActionOnTimeout": false,    "Event": "KEY_EVENT_THUMBS_UP"}

2. Single Key pressed and released, action to to be performed on the key release. no action on or during key press
Example: aux_input key, main criteria is TimeOut is -1-----------------------------------------------------
    {"Keys": ["thumbs_up"],  "TimeOut": -1,      "Repeat": false,    "ActionOnTimeout": false,    "Event": "KEY_EVENT_THUMBS_UP"}

3. Single Key pressed, held for certain duration and released after timeout.
Example: power button is pressed for 300 mSeconds, here TimeOut is the time for which the key should be held

    {"Keys": ["power"],   "TimeOut": 300,      "Repeat": false,    "ActionOnTimeout": false,    "Event": "KEY_EVENT_BOOKMARK"}

4. Single Key is pressed, held without release, here repetative action to be performed for every timeout interval
Example: volume_up key is pressed for longer duration and for every 300 mSeconds, action to be performed, here Repeat field should be made as true
    {"Keys": ["volume_up"],  "TimeOut": 300,      "Repeat": true,     "ActionOnTimeout": true,     "Event": "KEY_EVENT_VOLUME_UP"}

5. Combo Keys, where more than one Key is pressed and held for certain duration
Example: volume_down and preset_1 keys are pressed and held for certain time interval
a. Two key combination
    {"Keys": ["volume_down", "preset_1"],  "TimeOut": 1000,  "Repeat": false,    "ActionOnTimeout": false,    "Event": "KEY_EVENT_VOLUME_DOWN_PRESET_1"},
b. Four key combination
    {"Keys": ["preset_1", "preset_2", "preset_3", "preset_4"],  "TimeOut": 5000,     "Repeat": false,    "ActionOnTimeout": false,    "Event": "KEY_EVENT_PRESET_1_2_3_4"},

6. Different action to be performed at different intervals for same key is pressed for longer duration based on the time key is pressed and held,
    This can also be performed with key combinations
Example:
    a) preset_5 has 3 rows with ascending order in timeout of 5000, 15000, 25000 mSeconds. if the ActionTimeout is true, even will be sent out after every timeout interval
        {"Keys": ["preset_5"],  "TimeOut": 5000,     "Repeat": false,    "ActionOnTimeout": true,     "Event": "KEY_EVENT_LONG_PRESET_5"},
        {"Keys": ["preset_5"],  "TimeOut": 15000,    "Repeat": false,    "ActionOnTimeout": true,     "Event": "KEY_EVENT_EXTRA_LONG_PRESET5"},
        {"Keys": ["preset_5"],  "TimeOut": 25000,    "Repeat": false,    "ActionOnTimeout": true,     "Event": "KEY_EVENT_LONGEST_PRESET_5"}
    b) preset_5 has 3 rows with ascending order in timeout of 5000, 15000, 25000 mSeconds, if the ActionTImeOut is false, then event will not be sent out till
        the key is release or the final timeout duration is occured, if the key is release before the  second interval, previous event will be generated and sent out
        {"Keys": ["preset_5"],  "TimeOut": 5000,     "Repeat": false,    "ActionOnTimeout": false,     "Event": "KEY_EVENT_LONG_PRESET_5"},
        {"Keys": ["preset_5"],  "TimeOut": 15000,    "Repeat": false,    "ActionOnTimeout": false,     "Event": "KEY_EVENT_EXTRA_LONG_PRESET5"},
        {"Keys": ["preset_5"],  "TimeOut": 25000,    "Repeat": false,    "ActionOnTimeout": false,     "Event": "KEY_EVENT_LONGEST_PRESET_5"}

7. Many actions with same key for different events(key press and held for some timeout)
    Key events willbe generated as soon as key is pressed and after 15 seconds and after 25 seconds of press
    {"Keys": ["preset_5"],  "TimeOut": 0,        "Repeat": false,    "ActionOnTimeout": false,     "Event": "KEY_EVENT_LONG_PRESET_5"},
    {"Keys": ["preset_5"],  "TimeOut": 15000,    "Repeat": false,    "ActionOnTimeout": true,     "Event": "KEY_EVENT_EXTRA_LONG_PRESET5"},
    {"Keys": ["preset_5"],  "TimeOut": 25000,    "Repeat": false,    "ActionOnTimeout": true,     "Event": "KEY_EVENT_LONGEST_PRESET_5"}


IMPORTANT
------------------------------------------------------------
IF THERE ARE MANY EVENTS FOR SAME KEY AT DIFFERENT INTERVALS THEN ROWS SHOULD BE IN ASCENDING ORDER OF TIMEOUT VALUE

