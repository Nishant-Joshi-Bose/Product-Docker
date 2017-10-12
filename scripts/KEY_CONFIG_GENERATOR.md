Key Configuration Generator
===========================

The key configuration generator script (friendly_to_raw_key_config.py) simplifies maintenance of the .json configuration file for the CastleKeyHandler component by allowing the user to specify keys and actions by their symbolic names from enumerations in the source code.  The script takes as input a "friendly" .json configuration file, C/C++ header files containing key value enumerations (one per possible source, each source is optional), and a header file containing key action enumerations.  The script outputs a .json configuration file suitable for use with the CastleKeyHandler component (enumerations converted to numeric values).

# Configuration fields

The following fields can be specified in a "friendly" format which will then be converted to the numeric format expected by CastleKeyHandler.

## KeyList

The values in the "KeyList" array must be of type KEY_VALUE.  The key list header file for each source must contain define keys for each source as "typedef enum { ... } KEY_VALUE;"

## Action

The "Action" must be of type KEY_ACTION (see KeyActions.h).  Generate the "raw" configuration file with the following command:

## Origin

The "Origin" can be any of the following strings
* "CONSOLE_BUTTON"
* "CAPSENSE"
* "IR"
* "RF"
* "CEC"
* "NETWORK"
* "TAP"

## KeyEvent
S
The "KeyEvent" can be any of the following strings
* "PRESS"
* "RELEASE"
* "PRESS_HOLD"
* "PRESS_HOLD_REPEAT"
* "RELEASE_BURST"

# Generating the configuration file

Run the sript without any arguments for help.

./scripts/friendly_to_raw_key_config.py 
usage: generate key config [-h] --inputcfg INPUTCFG --actions ACTION_FILE
                           [--console CONSOLE_FILE] [--cap CAP_FILE]
                           [--ir IR_FILE] [--rf RF_FILE] [--cec CEC_FILE]
                           [--net NET_FILE] [--tap TAP_FILE] --outputcfg
                           OUTPUTCFG

* inputcfg -  "friendly" .json configuration file
* outputcfg - "raw" .json confiugration file
* actions - header file containing list of actions ("typedef enum { ... } KEY_ACTION;") 
* console/cap/ir/rf/cec/net/tap - header file containing list of keys for corresponding origin ("typedef enum KEY_VALUE;"); only required if you have keys defined from the corresponding origin

TestKeyConfig.json contains a sample "friendly" configuration.  

Run the configuration generator as follows (KeyConfiguration.json will be the output).


```./scripts/friendly_to_raw_key_config.py  --inputcfg=./Config/UserKeyConfig.json --actions=./ProductController/include/KeyActions.h --cap=/scratch/components-cache/trunk@911/RivieraLPM/include/RivieraLPM_KeyValues.h --ir=/scratch/components-cache/trunk@911/RivieraLPM/include/RivieraLPM_KeyValues.h --outputcfg=./Config/KeyConfiguration.json

