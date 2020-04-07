#!/bin/bash

# Set up the virtual environment within the docker container and
# use pytest to orchestrate the test
# This script should be run from within the container
# It will be located at /scratch/Scripts

cd /scratch/CastleProducts/TestDev
#rm -rf venv     #this is probably unnecessary but cleaner?
#virtualenv -p /usr/bin/python2.7 ./venv  --no-setuptools --required for python2.7
virtualenv -p /usr/bin/python3 ./venv3
source venv3/bin/activate
# rm -rf ~/.cache/pip -- commented for now
#./venv/bin/pip install setuptools==44.0.0 -- required for python 2.7
make
pip install brainstem --trusted-host ingress-platform.live-aws-useast1.bose.io/dev/svc-core-devops-nexus/prod/core-devops-nexus-core --extra-index-url https://ingress-platform.live-aws-useast1.bose.io/dev/svc-core-devops-nexus/prod/core-devops-nexus-core/repository/pypi-all/simple
pip install pyadb --trusted-host ingress-platform.live-aws-useast1.bose.io/dev/svc-core-devops-nexus/prod/core-devops-nexus-core --extra-index-url https://ingress-platform.live-aws-useast1.bose.io/dev/svc-core-devops-nexus/prod/core-devops-nexus-core/repository/pypi-all/simple
pip install -qr requirements.txt

# The following definition is only needed for professor (and will not effect eddie)
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(components get protobuf-x86_64 installed_location)/lib
./venv3/bin/python3 -m pytest -sv test_start_product_controller.py
