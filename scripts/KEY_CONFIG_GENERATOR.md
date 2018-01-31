Key Configuration Generator
===========================

The key configuration generator script (friendly_to_raw_key_config.py) simplifies maintenance of the .json configuration file for the CastleKeyHandler component by allowing the user to specify keys and actions by their symbolic names from enumerations in the source code.  The script takes as input one or more "friendly" .json configuration files, C/C++ header files containing key value enumerations (one per possible source, each source is optional), and C/C++ header files containing key action (intent) enumerations.  The script outputs a .json configuration file suitable for use with the CastleKeyHandler component (enumerations converted to numeric values).

# clang dependency
Note that this has a dependency on python clang.  Additionally, depending on the location of your clang library, you may need to set LD_LIBRARY_PATH so that the library can be found (i.e. export LD_LIBRARY_PATH=/usr/lib/llvm-4.0/lib/).

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
usage: generate key config [-h] --inputcfgs INPUTCFGS [INPUTCFGS ...]
                           --actions ACTIONS_FILES [ACTIONS_FILES ...]
                           [--console CONSOLE_FILE] [--cap CAP_FILE]
                           [--ir IR_FILE] [--rf RF_FILE] [--cec CEC_FILE]
                           [--net NET_FILE] [--tap TAP_FILE] [--reverse]
                           --outputcfg OUTPUTCFG
                           [--incdirs INC_DIRS [INC_DIRS ...]]


* inputcfgs -  list of "friendly" .json configuration files
* outputcfg - "raw" .json confiugration file
* actions - list of header files containing action enumeration (enum class Action i.e. Intents.h)
* console/cap/ir/rf/cec/net/tap - header file containing list of keys for corresponding origin ("typedef enum KEY_VALUE;"); only required if you have keys defined from the corresponding origin
* incdirs - list of include dirs (for finding files included from C/C++ header files)
* reverse - "recovery" mode - attempt to convert a raw json config to friendly config (inputcfgs and outputcfg swap meanings in this case) 

export LD_LIBRARY_PATH=/usr/lib/llvm-4.0/lib
LPM_KEYS=/scratch/components-cache/trunk@1750/RivieraLPM/include/RivieraLPM_KeyValues.h

./scripts/friendly_to_raw_key_config.py \
 --inputcfg ./Config/UserKeyConfig.json \
 --actions ./ProductController/source/IntentHandler/Intents.h \
 --cap ${LPM_KEYS} \
 --ir ${LPM_KEYS} \
 --tap ${LPM_KEYS} \
 --cec ${LPM_KEYS} \
 --rf ${LPM_KEYS} \
 --outputcfg ./opt-bose-fs/etc/KeyConfiguration.json \
 --incdirs  /scratch/components-cache/master@0bfef333cf5e65dead8725265b0ed471ff2e7dfd/CastleProductControllerCommon/IntentHandler

