#!/bin/bash
# This is an example script that could be run automatically on entry
# to the docker container. See the: rundocker.sh command to enable
# running this script or any other using the docker: --entrypoint
# command line option.
echo docker-entrypoint: Now run containersetup.sh
/scratch/Scripts/containersetup.sh
