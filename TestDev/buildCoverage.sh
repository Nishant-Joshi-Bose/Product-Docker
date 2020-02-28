#!/bin/bash
# Build for Coverage. Use this after doing an x86_64 build
#   The Coverage build allows you to run the CastleProducts component in a
#   docker container

THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
REPO_DIR="$THIS_DIR/../"
cd $REPO_DIR
make sdk=x86_64 cfg=Coverage

