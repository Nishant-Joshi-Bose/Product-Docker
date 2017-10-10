Key Configuration Generator
===========================

The key configuration generator script (friendly_to_raw_key_config.py) simplifies maintenance of the .json configuration file for the CastleKeyHandler component by allowing the user to specify keys and actions by their symbolic names from enumerations in the source code.  The script takes as input a "friendly" .json configuration file, a C/C++ header file containing key value enumerations, and a header file containing key action enumerations.  The script outputs a .json configuration file suitable for use with the CastleKeyHandler component (enumerations converted to numeric values).


# Generating a key configuration file

* Config/TestKeyConfig.json 
* ProductController/include/KeyValue.h 
* ProductController/include/KeyEvents.h 

TestKeyConfig.json contains a sample "friendly" configuration.  The values in the "KeyList" array must be of type KEY_VALUE (see KeyValue.h).  The "Action" must be of type KEY_EVENT (see KeyEvents.h).  Generate the "raw" configuration file with the following command:

```./scripts/friendly_to_raw_key_config.py Config/TestKeyConfig.json ProductController/include/KeyValue.h ProductController/include/KeyEvents.h Config/KeyConfiguration.json


