#!/bin/bash
THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

X86_64_DIR="$THIS_DIR/../../builds/Coverage/x86_64"
REPO_DIR="$THIS_DIR/../../"
FILES_DIR="$THIS_DIR/../../builds/Coverage/x86_64/Files"

docker run \
--entrypoint /scratch/Scripts/docker-entrypoint.sh \
-v $REPO_DIR:/scratch/CastleProducts \
-v $FILES_DIR/mnt:/mnt \
-v $FILES_DIR/opt:/opt \
-v $FILES_DIR/persist:/persist \
-v /scratch/components-cache:/scratch/components-cache \
-v $THIS_DIR:/scratch/Scripts \
nonprod-registry.bose.io/svc-auto-nucleus/auto-nucleus:master.5.118 bash
