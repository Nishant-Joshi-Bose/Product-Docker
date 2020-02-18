#!/bin/bash

# Set up the virtual environment within the docker container and
# use pytest to orchestrate the test
# This script should be run from within the container
# It will be located at /scratch/Scripts

cd /scratch/CastleProducts/TestDev
#rm -rf venv     #this is probably unnecessary but cleaner?
virtualenv -p /usr/bin/python2.7 ./venv  --no-setuptools
source venv/bin/activate
rm -rf ~/.cache/pip
./venv/bin/pip install setuptools==44.0.0
make
pip install -qr requirements.txt
#pip install /scratch/Mock/CastleTestUtils/dist/*
# The following definition is only needed for professor (and will not effect eddie)
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(components get protobuf-x86_64 installed_location)/lib

# enable Core dump, assuming this container was started with:
#   docker run --privileged ..
echo "/scratch/core.%E.%p" >/proc/sys/kernel/core_pattern

pytest -sv test_start_product_controller.py
