# Set up virtual environment
virtualenv -p /usr/bin/python ./venv
source ./venv/bin/activate
pip install -r requirements.txt

# Run tests
pytest -sv AudioSettings/test_audio_settings.py --device-id 8857f8d --network-iface eth0 --target device --email professor_product_controller@bose.com --password 02bosebose
