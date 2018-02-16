# LPM Tests Using Pytest Framework
A suite of tests (unit and integration) that stress various parts of LPM functionality.

The scripts in this folder should be run through pytest from the project root folder (where the pytest configuration file lives).
For example:
```
pytest -s Test/lpm/test_lightbar_ipc.py --ip-address 10.60.33.62
```

They invoke commands over LPM tap, APQ tap, LPM Service IPC messaging, or a combination. The requirements for each are:
* LPM tap: CastleTestUtils must be in the path
* APQ tap: Device must be reachable via adb
* IPC messaging: The "Python" folder from RivieraLpmService must be in the path and renamed to
"RivieraLpmService".

Tests that use LPM tap require the command line argument `--lpm-port` (e.g., /dev/ttyUSB0).
Tests that use APQ tap or LPM IPC require the command line argument `--ip-address` (IP of test system).

# Test Descriptions

##### test_apq_tap_examples.py
Miscellaneous examples of tests that work over APQ tap alone.

##### test_lightbar_ipc.py
Fully test lightbar functionality by issuing IPC commands to the LPM.

##### test_lowopower_integration.py
Use APQ tap to put the device in lop power standby, then simulate a key press to wake the device.

##### test_lowpower_lpm_unit.py
LPM unit test for low power standby. Invokes low power state using IPC messsaging, then LPM tap to simulate a key press. Does not involse the APQ.

##### test_lpm_tap_examples.py
Miscellaneous examples of tests that work over LPM tap including: key input simulation and amp fault.