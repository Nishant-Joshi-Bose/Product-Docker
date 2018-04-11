#### Diagnostics Integrated Test 

This folder contains Eddie Diagnostics Information functions

**How to run the test:**
# Setting up the environment
virtualenv -p /usr/bin/python2.7 ./eddievenv
. ./eddievenv/bin/activate
pip install pip==9.0.3

# Installing local CastleTestUtils
pip install -q git+ssh://git@github.com/BoseCorp/CastleTestUtils.git@81ce6faa86406d74841c02d4005ee11c9ae50e08
pip install -q -r Test/requirements.txt

#Running the Test
cd Test/mfgdata
pytest -sv test_diagnostics_mfgdata.py --driver chrome --target=device --network-iface=${inf} --device-id=${DEVICE_ID} --junitxml=output.xml

#Teardown virtual-environment
deactivate