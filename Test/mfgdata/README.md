#### Diagnostics Integrated Test 

This folder contains Eddie Diagnostics Information functions

**How to run the test:**
# Setting up the environment
virtualenv -p /usr/bin/python2.7 ./eddievenv
. ./eddievenv/bin/activate
pip install pip==9.0.3

#Install chromedriver on your Ubuntu machine
wget https://chromedriver.storage.googleapis.com/2.35/chromedriver_linux64.zip
unzip chromedriver_linux64.zip
sudo mv chromedriver /usr/bin/chromedriver
sudo chown root:root /usr/bin/chromedriver
sudo chmod +x /usr/bin/chromedriver

#Install Xvfb to run chrome browser in headless mode on your Ubuntu Machine
sudo apt-get install xdg-utils
sudo apt install xvfb
Xvfb :99 & (Note: If it is already running your system, please kill the process)
export DISPLAY=:99
pip install xvfbwrapper

# Installing CastleTestUtils locally 
pip install -q git+ssh://git@github.com/BoseCorp/CastleTestUtils.git@81ce6faa86406d74841c02d4005ee11c9ae50e08 
pip install -q -r Test/requirements.txt

#Running the Test
cd Test/mfgdata
pytest -sv test_diagnostics_mfgdata.py --driver chrome --target=device --network-iface=${inf} --device-id=${DEVICE_ID} --junitxml=output.xml

#Teardown virtual-environment
deactivate
