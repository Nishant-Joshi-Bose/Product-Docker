# Eddie Factory Default Tests

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

## Individual Factory Default Test Cases

### To test success scenario of factory default:
```bash
pytest -vs \
       --target=device \
       --device-id=<ADB device id> \
       --router <router in conf_wifiProifles.ini> \
       test_factory_default.py::test_factory_default_success
```

### Cancellation of factory default by releasing factory-key-combination:
```bash
pytest -vs \
       --target=device \
       --device-id=<ADB device id> \
       test_factory_default.py::test_factory_default_cancel_event
```

### Cancellation of factory default by key press (e.g. BT, VolUp, MFB):
```bash
pytest -vs \
         --target=device \
         --device-id=<ADB device id> \
         test_factory_default.py::test_factory_default_cancel_with_button_event
```

## List of Files under Test/factoryDefault :

`conftest.py` = Contains fixtures used in factory default test script (test_factory_default.py)

`Configs/conf_wifiProfiles.ini` = Contains routers information with ssid, security-type, password

`factory_utils.py` = Contains general functions used in test script(test_factory_default.py)

`test_factory_default.py` = Automated script for factory default feature
