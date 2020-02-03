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
#pip install -qr requirements.txt
pip install /scratch/Mock/CastleTestUtils/dist/*
# The following definition is only needed for professor (and will not effect eddie)
#   You need to find it before you create the container so by running:
#   component ... to find the libprotobuf for x86_64 and add this path
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/scratch/components-cache/Conan/397/bose-stable/2.4.2-62+g0d99523/protobuf-x86_64/lib
pytest -sv test_start_product_controller.py
