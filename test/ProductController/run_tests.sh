# make sure to have adb set up with permissions

# Set up virtual environment
virtualenv -p /usr/bin/python ./venv
source ./venv/bin/activate
pip install -r requirements.txt

# Run tests
pytest -svx AudioSettings/test_audio_settings.py --device-id 8857f8d --network-iface eth0 --target device --email jacqueline_lagasse123@bose.com --password 02bosebose
pytest -svx AudioSettings/test_audio_settings_neg.py --device-id 8857f8d --network-iface eth0 --target device --email jacqueline_lagasse123@bose.com --password 02bosebose