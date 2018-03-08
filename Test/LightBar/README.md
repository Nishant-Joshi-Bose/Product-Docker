Install Python modules
----------------------

virtualenv -p /usr/bin/python <virtual dir>

<virtual dir>/bin/activate

pip install -r requirements.txt

### Commands to perform the lightbar animation

```shell session
$ pytest -vs test_lightbar_scenario.py --target=device --network-iface eth0 --lpm-port <lpm port of device>
```

#### Input information of Lightbar Animation
```shell session
nextValue: Animation pattern names
transition: transition value of animation pattern
repeat: Value for animation pattern to be repeated or not
data: Animation pattern data
expected Output: Animation pattern expected Output
```
