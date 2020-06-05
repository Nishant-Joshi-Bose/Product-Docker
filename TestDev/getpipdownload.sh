#!/bin/bash
# Use this to download the two external requirements for the CastleTestUtils package.
# This could replace the two lines in the containersetup.sh which download them within
# the docker container. Note this asks for particular versions whereas the line in
# containersetup.sh, I assume, get the latest version.
pip download --trusted-host ingress-platform.live-aws-useast1.bose.io/dev/svc-core-devops-nexus/prod/core-devops-nexus-core/ --extra-index-url https://ingress-platform.live-aws-useast1.bose.io/dev/svc-core-devops-nexus/prod/core-devops-nexus-core/repository/pypi-all/simple brainstem==2.7.6 pyadb==0.1.6
