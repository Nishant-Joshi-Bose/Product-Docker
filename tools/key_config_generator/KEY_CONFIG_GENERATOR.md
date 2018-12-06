Key Configuration Generator
===========================

# abbreviated instructions

## Prerequisites
* Make sure you have virtualenv installed ("sudo pip install virtualenv"; if you've ever built RivieraLpmService you should already have this installed)

## Building
* Add your changes to opt-bose-fs/etc/UserKeyConfig.json
* Run "make keyconfig"
* You should now have an updated opt-bose-fs/etc/KeyConfiguration.json; test it and commit it

The key configuration generator script (friendly_to_raw_key_config.py) simplifies maintenance of the .json configuration file for the CastleKeyHandler component by allowing the user to specify keys and actions by their symbolic names from enumerations in the source code.  The script takes as input a "friendly" .json configuration file, .proto files containing key value enumerations, and .proto files containing key action (intent) enumerations.  The script outputs a .json configuration file suitable for use with the CastleKeyHandler component (enumerations converted to numeric values).

# Configuration fields

The following fields can be specified in a "friendly" format which will then be converted to the numeric format expected by CastleKeyHandler.

## keyList

The values in the "keyList" array must be of type KEY_VALUE.  The key list header file for each source must contain define keys for each source as "typedef enum { ... } KEY_VALUE;"

## action

The "action" must be of type Action (see Intents.h).  Generate the "raw" configuration file with the following command:

## origin

The "origin" can be any of the following strings
* "CONSOLE_BUTTON"
* "CAPSENSE"
* "IR"
* "RF"
* "CEC"
* "NETWORK"
* "TAP"

## keyEvent

The "keyEvent" can be any of the following strings
* "PRESS"
* "RELEASE"
* "PRESS_HOLD"
* "PRESS_HOLD_REPEAT"
* "RELEASE_BURST"

## comment

The comment field can be any string

## timeOutList

*TODO*

# Generating the configuration file

Run the sript without any arguments for help.

./friendly_to_raw_key_config.py 
usage: generate key config [-h] --inputcfg INPUTCFG
                           --common COMMON_INTENTS_FILE
                           --custom CUSTOM_INTENTS_FILE
                           --keys KEY_DEFINITION_FILE
                           --outputcfg OUTPUTCFG

* inputcfg -  "friendly" .json configuration file
* outputcfg - "raw" .json confiugration file
* common - file containing common intent definitions
* custom - file containing custom intent definitions

The top-level makefile contains a "keyconfig" rule that will run the script with the correct arguments i.e.
make keyconfig

