# Eddie LPM Tests
A suite of tests (unit and integration) that stress various parts of LPM functionality.

The scripts in this folder should be run through pytest from the project root folder (where the pytest configuration file lives).

They invoke commands over LPM tap, APQ tap, LPM Service IPC messaging, or a combination. The requirements for each are:
* LPM tap: CastleTestUtils must be in the path
* APQ tap: Device must be reachable via adb
* IPC messaging: The "Python" folder from RivieraLpmService must be in the path and renamed to
"RivieraLpmService".

Tests that use LPM tap require the command line argument `--lpm-port` (e.g., /dev/ttyUSB0).
Tests that use APQ tap or LPM IPC require the command line argument `--router <router id>`.

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

## Test Descriptions

### `test_apq_tap_examples.py`
Miscellaneous examples of tests that work over APQ tap alone.
```bash
pytest -vs \
       --target device \
       --device-id xxxxxxx \
       --lpm-port "/dev/ttyUSB0" \
       --router fakessid \
       lpm/test_apq_tap_examples.py 
```

### `test_lightbar_ipc.py`
Fully test lightbar functionality by issuing IPC commands to the LPM.

### `test_lowopower_integration.py`
Use APQ tap to put the device in lop power standby, then simulate a key press to wake the device.

### `test_lowpower_lpm_unit.py`
LPM unit test for low power standby. Invokes low power state using IPC messsaging, then LPM tap to simulate a key press. Does not involse the APQ.

### `test_lpm_tap_examples.py`
Miscellaneous examples of tests that work over LPM tap including: key input simulation and amp fault.

### `test_thermal.py`
Acquire thermal data over IPC and verify that it is reasonable.

