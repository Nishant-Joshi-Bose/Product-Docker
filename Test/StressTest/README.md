# Eddie Stress Tests

## Synopsis

This test module will cover Stress tests on the Eddie product.

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

## Execution of Tests

Running the tests requires configuration information that should be known prior to startup.
The test execution is dependent upon the ADB Device ID.
An option argument for the Router can be used. List of routers can be found in `$EDDIE_DIR/Test/Configs/conf_wifiProfiles.ini`. You may add your router or ommit it to not activate a WiFi connection at the conclusion of the test.

```bash
cd $EDDIE_DIR/Test/StressTest
pytest --junit=stress_test.xml \
       --router=<Router Config in Configs/conf_wifiProfiles.ini> \
       --device-id=<adb-device-id> \
       --count=<No of times test nedd to be repeated>
```

This will run all `test_*` inside the `StressTest` folder and generate JUnit style output to
`stress_test.xml`.

### Test Descriptions

`test_stress_test.py` test suite can be found in the `StressTest` test module.

`test_stress_test.py` contains stress tests for Eddie product.
The following self descriptive test names can be found in the suite:

- `test_stress_first_boot_greetings`

This will Factory Default the attached device, then assert that it has completed its initial OOB setup after rebooting.
