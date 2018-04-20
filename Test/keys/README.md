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

## Execution of Key Event Tests

```bash
pytest -vs \
       --apq-port <apq_port of target device> \
       --junitxml=output.xml \
       test_clikey.py
```

## List of Files under Test/utils :


configure.py       = Configurable parameters used in CLI test scripts (test_clikey.py,keyEvent.py)

keyeventlist.py    = Positive and Negative scenarios for CLI  key events

KeyEvent.py        = Key events based on key pressed for CLI test script(test_clikey.py) 

test_clikey.py     = Automated script for CLI keys pressed

requirements.txt   = List of required python modules  



## Details of various KeyEvent value used in `Test/utils/KeyEvent.py`:

KEY_PRESS = 0: Actions for immediate key presses.

KEY_RELEASE = 1, (Can have multiple such entries in the configuration for the same key, action will be different on different hold time, which is defined in timeOutList. If a Action is already translated fo this key on press, this KeyEvent will be ignored as Illegal, we have KEY_EVENT_RELEASE_ALWAYS for translating Actions for release on a particular key)

KEY_PRESS_HOLD = 2, (Action triggered when a key is pressed and held for a duration mentioned in TimeOutList)

KEY_PRESS_HOLD_FOR_REPETITIVE = 3, (Repetitive action will be triggered when a key is pressed. Two duration value is used defined in TimeOutList. First one used for intial duration for hold and second one used for repetitive.

KEY_RELEASE_BURST = 4, To detect a burst of keys (could be same of different keys, the duration between keys will be as configured in TimeOutList

