#!/bin/bash
# Get files/directories used by product controller
# and copy into CastleProducts/builds/Coverage/x86_64/Files

THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
X86_64_DIR="$THIS_DIR/../../builds/Coverage/x86_64"
rm -rf $X86_64_DIR/Files
cp -r $THIS_DIR/../Files $X86_64_DIR
