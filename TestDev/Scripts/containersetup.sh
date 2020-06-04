#!/bin/bash

# Set up the virtual environment within the docker container and
# use pytest to orchestrate the test
# This script should be run from within the container
# It will be located at /scratch/Scripts

cd /scratch/CastleProducts/TestDev
#   I need a way to ensure the python environment is clean.
#   Perhaps python is smart enough but I'd like to verify.
#   For now I'm commenting the next two lines assuming python is smart
#   
# rm -rf venv
# rm -rf ~/.cache/pip

virtualenv -p /usr/bin/python3 ./venv3
source venv3/bin/activate
make
pip install brainstem --trusted-host ingress-platform.live-aws-useast1.bose.io/dev/svc-core-devops-nexus/prod/core-devops-nexus-core --extra-index-url https://ingress-platform.live-aws-useast1.bose.io/dev/svc-core-devops-nexus/prod/core-devops-nexus-core/repository/pypi-all/simple
pip install pyadb --trusted-host ingress-platform.live-aws-useast1.bose.io/dev/svc-core-devops-nexus/prod/core-devops-nexus-core --extra-index-url https://ingress-platform.live-aws-useast1.bose.io/dev/svc-core-devops-nexus/prod/core-devops-nexus-core/repository/pypi-all/simple

# Choose to install CastleTestUtils from the cloud or your local version by uncommenting
#   one of the next two lines:
pip install -qr requirements.txt                #install from the cloud
#pip install /scratch/CastleTestUtils/dist/*    #install locally

# The following definition is only needed for professor (and will not effect eddie)
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(components get protobuf-x86_64 installed_location)/lib


# enable Core dump, assuming this container was started with:
#   docker run --privileged ..
echo "/scratch/core.%E.%p" >/proc/sys/kernel/core_pattern

# Choose to run the product controller with or without strace by uncommenting
# one of the lines below. If you choose to run strace, the output will be in
# the CastleProducts/TestDev directory

#strace -o strace -ff pytest -sv test_start_product_controller.py
pytest -sv test_start_product_controller.py
