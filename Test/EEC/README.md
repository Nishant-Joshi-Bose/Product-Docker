## Eddie Energy Efficiency Compliance Timer Tests
This Folder contains integrated Energy Efficiency Compliance Timer tests(Low Power and Network Standby functionality)

## Setting Up Test Environment

Clone the Eddie Repository:
```bash
git clone git@github.com:BoseCorp/Eddie.git
cd Eddie
```
## Setting up the virtual environment

```
virutalenv -p /usr/bin/python2.7 .venv
source $EDDIE_DIR/Test/.venv/bin/activate
```

## Installing required packages
```
pip install -q pip==9.0.3
pip install git+ssh://git@github.com/BoseCorp/CastleTestUtils.git@master
```

## Install current Python requirements
```
pip install --requirement requirements.txt
```

## Run the test
```
cd Test/EEC
pytest -sv test_eec_timer.py --device-id=${DEVICE_ID} --lpm-port=${LPM_PORT} --network-iface=${inf} --target="device" --router=${ROUTER}
```

