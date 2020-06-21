#!/bin/bash
THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

X86_64_DIR="$THIS_DIR/../../builds/Coverage/x86_64"
REPO_DIR="$THIS_DIR/../../"
FILES_DIR="$THIS_DIR/../../builds/Coverage/x86_64/Files"

# Running a privileged docker container allows writting to the /proc file system
# This is useful for example to run strace on the product controller.
# To enable it uncomment the following line:
#prives="--privileged"

# The command below contains several optional lines.
# Optional lines are the ones starting with:`#
# and ending with:` \
# These lines are ignored (i.e. expanded to a single space) by the
# bash interpreter. To enable the line, remove the:`#
# and the second:`
# and be sure to leave the training:\


# The options allow:
# 1.  Automatically invoking a script (e.g. docker-entrypoint.sh) on entry
#       into the docker container
# 2.  Allowing other directories to be mapped into the docker container.
#       e.g. 
# To have this script automatically invoke a script upon entry into
# the docker container, e.g. docker-entrypoint.sh

# Note that the hash used in the last line of the below docker command
# is the id of the docker container being run.
# For a list of docker containers currently available
# on your system type:
#        docker container list

docker run ${prives} -it \
`#--entrypoint /scratch/Scripts/docker-entrypoint.sh` \
-v $REPO_DIR:/scratch/CastleProducts \
-v $FILES_DIR/mnt:/mnt \
-v $FILES_DIR/opt:/opt \
-v $FILES_DIR/persist:/persist \
-v /scratch/components-cache:/scratch/components-cache \
-v $THIS_DIR:/scratch/Scripts \
`#-v /scratch/CastleTestUtils:/scratch/CastleTestUtils` \
`#-v /scratch/AudioSource:/scratch/AudioSource` \
b0a9a495a8b5 bash
