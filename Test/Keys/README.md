# Eddie Key Event Tests

## Setting Up Test Environment

Clone the Eddie Repository:

```bash
# Clone the respository
git clone git@github.com:BoseCorp/Eddie.git
cd Eddie
```

All steps assume your are in the directory of your Eddie repository (`$EDDIE_DIR`).

Generate a Virtual Environment and install used packages.

```bash
cd $EDDIE_DIR/Test

# Setup your virtual environment
virutalenv -p /usr/bin/python2.7 .venv
source $EDDIE_DIR/Test/.venv/bin/activate

# Install current Python requirements
pip install --requirement requirements.txt
```

## Execution of AUX Key Event Tests

```shell session
pytest --verbose --capture=no \
       --junitxml=aux_keys.xml \
       --device-id=<ADB Device ID> \
       --router=<Router ID> \
       test_*.py
```

## List of Files

cli_key_helper.py                = Helper functions for cli key scenarios.

test_aux_key_scenario.py         = Aux key test scenarios.

test_pts_scenario.py             = System Info scenarios.

### Details of various KeyEvents

KEY_PRESS = 0: Actions for immediate key presses.

KEY_RELEASE = 1, (Can have multiple such entries in the configuration for the same key, action will be different on different hold time, which is defined in timeOutList. If a Action is already translated fo this key on press, this KeyEvent will be ignored as Illegal, we have KEY_EVENT_RELEASE_ALWAYS for translating Actions for release on a particular key)

KEY_PRESS_HOLD = 2, (Action triggered when a key is pressed and held for a duration mentioned in TimeOutList)

KEY_PRESS_HOLD_FOR_REPETITIVE = 3, (Repetitive action will be triggered when a key is pressed. Two duration value is used defined in TimeOutList. First one used for initial duration for hold and second one used for repetitive.

KEY_RELEASE_BURST = 4, To detect a burst of keys (could be same of different keys, the duration between keys will be as configured in TimeOutList
