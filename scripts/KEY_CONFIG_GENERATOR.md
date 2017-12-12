Key Configuration Generator
===========================

The key configuration generator script (friendly_to_raw_key_config.py) simplifies maintenance of the .json configuration file for the CastleKeyHandler component by allowing the user to specify keys and actions by their symbolic names from enumerations in the source code.  The script takes as input one or more "friendly" .json configuration files, C/C++ header files containing key value enumerations (one per possible source, each source is optional), and one or more .pyc outputs from protobuf compilation or header files containing key action enumerations.  The script outputs a .json configuration file suitable for use with the CastleKeyHandler component (enumerations converted to numeric values).

# clang dependency
Note that this has a dependency on python clang.  Additionally, depending on the location of your clang library, you may need to set LD_LIBRARY_PATH so that the library can be found (i.e. export LD_LIBRARY_PATH=/usr/lib/llvm-4.0/lib/).

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
usage: generate key config [-h] --inputcfgs INPUTCFGS --actions ACTIONS_FILES
                           [--console CONSOLE_FILE] [--cap CAP_FILE]
                           [--ir IR_FILE] [--rf RF_FILE] [--cec CEC_FILE]
                           [--net NET_FILE] [--tap TAP_FILE] --outputcfg
                           OUTPUTCFG

* inputcfgs -  list of "friendly" .json configuration files
* outputcfg - "raw" .json confiugration file
* actions - list of .pyc files output from compilation of .proto file containing KEY_ACTION enumeration or header files containing list of actions ("typedef enum { ... } KEY_ACTION;") (type is determined by file extension)
* console/cap/ir/rf/cec/net/tap - header file containing list of keys for corresponding origin ("typedef enum KEY_VALUE;"); only required if you have keys defined from the corresponding origin

TestKeyConfig.json contains a sample "friendly" configuration.  

Run the configuration generator as follows (KeyConfiguration.json will be the output).


```
LPM_KEYS=/scratch/components-cache/master@5e3aefe78e9cd00f7dbeaab54f17be962be60cdf/RivieraLpmService/source/LPMIPC/LPM_KeyValues.h

./scripts/friendly_to_raw_key_config.py \
 --inputcfg \
    ./Config/UserKeyConfig.json \
    ./CommonConfig.json \
 --actions \
    ./builds/Release/qc8017_32/proto_py/KeyActions_pb2.pyc \
    ./CommonIntents.h \
 --cap ${LPM_KEYS} \
 --ir ${LPM_KEYS} \
 --outputcfg ./opt-bose-fs/etc/KeyConfiguration.json
```

