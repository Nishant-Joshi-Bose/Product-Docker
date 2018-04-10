#### Diagnostics Integrated Test 

This folder contains Eddie Diagnostics Information functions

**To Clone the project :**
```
git clone git@github.com:BoseCorp/Eddie.git
cd Test/mfgdata
```
**How to run the test:**
pytest -sv test_diagnostics_mfgdata.py --driver=chrome --network-iface="wlan0" --device-id=${DEVICE_ID}
