Install Python modules
----------------------

```
$ sudo pip install -r requirements.txt
```

### Commands to perform the lightbar animation

##### For getting the animation value that is playing:
```shell session
$ pytest -vs test_localui_lightbar_script.py::Test_lightbar::test_getActiveAnimation \
--ip-address=<ip-address> --LPM_port <lpm_port>
```

##### To delete the animation being played following command:
```shell session
$ pytest -vs test_localui_lightbar_script.py::Test_lightbar::test_deleteActiveAnimation \
--ip-address=<ip-address> --LPM_port <lpm_port>
```

##### To run the negative test scenarios for lightbar animation:
```shell session
$ pytest -vs test_localui_lightbar_script.py::Test_lightbar::test_playInValidAnimationScenarios \
--ip-address=<ip-address> --LPM_port <lpm_port>
```

##### To run the positive test scenarios for lightbar animation:
```shell session
$ pytest -vs test_localui_lightbar_script.py::Test_lightbar::test_playValidAnimationScenarios --ip-address=<ip-address> --LPM_port <lpm_port>
```

