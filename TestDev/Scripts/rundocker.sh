#!/bin/bash
THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

X86_64_DIR="$THIS_DIR/../../builds/Coverage/x86_64"
REPO_DIR="$THIS_DIR/../../"
FILES_DIR="$THIS_DIR/../../builds/Coverage/x86_64/Files"

docker run -it \
-v $REPO_DIR:/scratch/CastleProducts \
-v $FILES/mnt:/mnt \
-v $FILES/opt:/opt \
-v $FILES/persist:/persist \
-v /scratch/components-cache:/scratch/components-cache \
-v $THIS_DIR:/scratch/Scripts \
b0a9a495a8b5 bash
