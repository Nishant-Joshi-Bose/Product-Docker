#!/bin/bash
# Get files/directories used by product controller from an actual target
# NOTE: Since files change from release to release, ensure you have installed
#   a very recent or identical build on the target before running this script

# Put the directories into the .../CastleProducts/builds/Coverage/x86_64/Files directory

THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
X86_64_DIR="$THIS_DIR/../../builds/Coverage/x86_64"
rm -rf $X86_64_DIR/Files
mkdir -p $X86_64_DIR/Files
cd $X86_64_DIR/Files
adb pull /persist ./persist
adb pull /mnt/nv ./mnt/nv
adb pull /opt/Bose/etc ./opt/Bose/etc
