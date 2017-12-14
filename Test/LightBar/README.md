Install Python modules
----------------------

```
$ sudo pip install -r requirements.txt
```

### Commands to perform the lightbar animation

##### For getting the animation value that is playing:
```shell session
$ pytest -vs test_localui_lightbar_script.py::Test_lightbar::test_getActiveAnimation --target device \
--network-iface eth0 --lpm_port <lpm_port>
```

##### To delete the animation being played following command:
```shell session
$ pytest -vs test_localui_lightbar_script.py::Test_lightbar::test_deleteActiveAnimation --target device \
--network-iface eth0 --lpm_port <lpm_port>
```

##### To run the negative test scenarios for lightbar animation:
```shell session
$ pytest -vs test_localui_lightbar_script.py::Test_lightbar::test_playInValidAnimationScenarios --target device \
--network-iface eth0 --lpm_port <lpm_port>
```

##### To run the positive test scenarios for lightbar animation:
```shell session
$ pytest -vs test_localui_lightbar_script.py::Test_lightbar::test_playValidAnimationScenarios --target device \
--network-iface eth0 --lpm_port <lpm_port>
```

#### Input information of Lightbar Animation
```shell session
value: Animation pattern names
transition: transition value of animation pattern
repeat: Value for animation pattern to be repeated or not
data: Animation pattern data
expected Output: Animation pattern expected Output
```

