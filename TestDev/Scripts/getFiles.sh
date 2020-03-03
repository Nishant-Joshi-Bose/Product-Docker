#!/bin/bash
# Get files/directories used by product controller
# and copy into CastleProducts/builds/Coverage/x86_64/Files
# as a staging directory for the Docker container.

set -x
THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
REPO_DIR="$THIS_DIR/../.."     #Component top level directory
X86_64_DIR="$REPO_DIR/builds/Coverage/x86_64"
FILES_STAGE_DIR="$X86_64_DIR/Files"

#   Verify a target build was done
PRODUCT_UPDATE_ZIP_PATH="$REPO_DIR/builds/Release/product_update.zip"
if [ ! -f $PRODUCT_UPDATE_ZIP_PATH ]; then
    echo "product_update.zip not found. Please build for target"
    exit 1
fi

#   Populate the Files directory
#cp -r $THIS_DIR/../Files $X86_64_DIR
rm -rf $FILES_STAGE_DIR
mkdir  -p $FILES_STAGE_DIR/opt/Bose/etc
cp -r $THIS_DIR/../Files/persist $FILES_STAGE_DIR

#   Extract files needed by Professor productcontroller from build
TMP_DIR=$(mktemp -d -t product_update-XXXXXXXXXX)
#TMP_DIR=FooTmp
echo unpacking product_update.zip into $TMP_DIR
unzip -d $TMP_DIR  $PRODUCT_UPDATE_ZIP_PATH  product.ipk 
cd $TMP_DIR
ar x product.ipk data.tar.gz
#tar xzf data.tar.gz --files-from $THIS_DIR/filelist -C $FILES_STAGE_DIR/opt/Bose
#tar xzf data.tar.gz --files-from $THIS_DIR/filelist  #this extracts ok to TMP_DIR
echo Destination Directory: $FILES_STAGE_DIR/opt/Bose
tar xz  --directory $FILES_STAGE_DIR/opt/Bose --file data.tar.gz --files-from $THIS_DIR/filelist

#rm -rf $TMP_DIR
