Install Python modules
----------------------

```
$ sudo pip install -r ../requirements.txt
```

### Commands to perform the factory default test cases

##### To test success scenario of factory default:
```shell session
$ pytest -vs test_factory_default.py::test_factory_default_success --target=device --device-id=<ADB device id> --router <router in conf_wifiProifles.ini>
```

##### To test cancellation of factory default by releasing factory-key-combination:
```shell session
$ pytest -vs test_factory_default.py::test_factory_default_cancel_event --target=device --device-id=<ADB device id>
```

##### To test cancellation of factory default by pressing of another key(BT or VolUp or MFB):
```shell session
$ pytest -vs test_factory_default.py::test_factory_default_cancel_with_button_event --target=device --device-id=<ADB device id>
```

### List of Files under Test/factoryDefault :


conftest.py                     = Contains fixtures used in factory default test script (test_factory_default.py)

Configs/conf_wifiProfiles.ini   = Contains routers information with ssid, security-type, password

factory_utils.py                = Contains general functions used in test script(test_factory_default.py)

test_factory_default.py         = Automated script for factory default feature


