#!/bin/bash
# Build for running in a docker container
# Currently must do an sdk=qc8017_32 build to create product_update.zip
#   which contains the files which are placed in /opt/Bose/etc on target
#   by Scripts/getFiles.sh.

THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
REPO_DIR="$THIS_DIR/../"
cd $REPO_DIR

make sdk=x86_64 cfg=Coverage
TestDev/Scripts/getFiles.sh

