Key Configuration Generator
===========================

The key configuration generator script (friendly_to_raw_key_config.py) simplifies maintenance of the .json configuration file for the CastleKeyHandler component by allowing the user to specify keys and actions by their symbolic names from enumerations in the source code.  The script takes as input a "friendly" .json configuration file, a C/C++ header file containing key value enumerations, and a header file containing key action enumerations.  The script outputs a .json configuration file suitable for use with the CastleKeyHandler component (enumerations converted to numeric values).

# Configuration fields

The following fields can be specified in a "friendly" format which will then be converted to the numeric format expected by CastleKeyHandler.

## KeyList

The values in the "KeyList" array must be of type KEY_VALUE (see KeyValue.h).  

## Action

The "Action" must be of type KEY_EVENT (see KeyEvents.h).  Generate the "raw" configuration file with the following command:

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

The "KeyEvent" can be any of the following strings
* "PRESS"
* "RELEASE"
* "PRESS_HOLD"
* "PRESS_HOLD_REPEAT"
* "RELEASE_BURST"

# Generating the configuration file

The following files are input to the configuration generator.

* Config/TestKeyConfig.json 
* ProductController/include/KeyValue.h 
* ProductController/include/KeyEvents.h 

TestKeyConfig.json contains a sample "friendly" configuration.  

Run the configuration generator as follows (KeyConfiguration.json will be the output).

```./scripts/friendly_to_raw_key_config.py Config/TestKeyConfig.json ProductController/include/KeyValue.h ProductController/include/KeyEvents.h Config/KeyConfiguration.json


