#!/bin/bash -e
#
# This script creates that full package (all image files) for fastboot update, on Riviera platform
#
# Assumptions and pre-requisites:
#   o Latest version of image files from Riviera HSP (except Bose partition) can be fetched from softlib space. Riviera HSP update package assumed to have fastboot.sh 
#   o The sandbox from which this script is run should have all Bose binaries built (i.e. "make sdk=qc8017_32 or make sdk=qc8017_64")
#   o make_ext4fs tools are available at /tools/hepdsw/bin OR at location accessible via PATH 


function usage
{
    cat <<EOF
Usage: $this_script <configuration> <sdk> with_lpm

configuration = Release/Debug
sdk = qc8017_32
with_lpm - optional string, if not given, it is assumed not to include lpm package

e.g. $this_script Release qc8017_32 with_lpm
EOF
}

function print_debug 
{
	if [ $debug -eq 1 ]; then
		tput setaf 5; echo "$@"; tput sgr0
	fi
}

function print 
{
	tput setaf 2; echo "$@"; tput sgr0
}

function print_warn 
{
	tput setaf 4; echo "$@"; tput sgr0
}

function print_error
{
	tput setaf 1; echo "$@" 2>&1; tput sgr0
}


############### INITALIZE
this_script=$(basename "$0")
cfg=$1
sdk=$2
workspace=$(cd "$(dirname "$0")"/.. && pwd) || exit
mydir=$workspace/install
configdir=$workspace/config
utildir=$workspace/install/utils
CP="cp -PR --preserve=mode,timestamps,links"
cd "$mydir"

RIVIERA_HSP_CONFIG=$configdir/HSP-Version.txt
LPM_CONFIG=$configdir/LPM-Version.txt
SOUNDTOUCH_CONFIG=$configdir/SoundTouch-Version.txt
# Read HSP Path
HSP_PACKAGE=`cat $RIVIERA_HSP_CONFIG`
LPM_PACKAGE=`cat $LPM_CONFIG`
SOUNDTOUCH_PACKAGE=`cat $SOUNDTOUCH_CONFIG`
HSP_PACKAGE_FILES=Riviera-HSP-*/images/*

# Modify later with version number
EDDIE_PACKAGE_DIRNAME=eddie-package
EDDIE_PACKAGE_TEMPPATH=/dev/shm/$EDDIE_PACKAGE_DIRNAME
EDDIE_FLASH_SCRIPT=$utildir/scripts/eddie_flash.py

BOSEFS_PARTITION_NAME=bose
BOSEFS_EXT4_NAME=bose.ext4
BOSEFS_BUILTFILES_PATH=$EDDIE_PACKAGE_TEMPPATH/bose

PERSIST_PARTITION_NAME=bose-persist
PERSIST_EXT4_NAME=bose-persist.ext4
PERSIST_BUILTFILES_PATH=$EDDIE_PACKAGE_TEMPPATH/bose-persist

# Initialize 
cfg=Release
sdk=qc8017_32
debug=0
with_lpm=0

for idx in "$@"
do
case $idx in
    -c=*|--cfg=*)
    cfg=`echo $idx | sed 's/[-a-zA-Z0-9]*=//'`
    ;;
    -s=*|--sdk=*)
    sdk=`echo $idx | sed 's/[-a-zA-Z0-9]*=//'`
    ;;
    -d|--debug)
    debug=1
    set -x
    ;;
    --with-lpm)
    with_lpm=1
    ;;
    --hsp-path=*)
    HSP_PACKAGE=`echo $idx | sed 's/[-a-zA-Z0-9]*=//'`
    ;;
    --lpm-path=*)
    LPM_PACKAGE=`echo $idx | sed 's/[-a-zA-Z0-9]*=//'`
    ;;
    --soundtouch-path=*)
    SOUNDTOUCH_PACKAGE=`echo $idx | sed 's/[-a-zA-Z0-9]*=//'`
    ;;
    *)
    # unknown option
    ;;
esac
done

EDDIE_PACKAGE_NAME=eddie_${sdk}_${cfg}.tar.gz
OUTPUT_PACKAGE=$mydir/../$EDDIE_PACKAGE_NAME
OUTPUT_SCRIPT=$mydir/../eddie_flash.py

print "Using: "
print "   HSP_Package: $HSP_PACKAGE"
print "   SOUNDTOCH_Package: $SOUNDTOUCH_PACKAGE"
if [ $with_lpm -eq 1 ]; then
    print "   LPM_Package: $HSP_PACKAGE"
fi
print "   Eddie_Package: $mydir/$EDDIE_PACKAGE_NAME"

############### INITALIZE END

function error_exit
{
	print_error "$@"
	if [ $debug -eq 0 ]; then 
		clean_package_space
	else
		set +x
	fi
	exit 1
}

function proper_exit
{
	print "$@"
	if [ $debug -eq 0 ]; then 
		rm -rf $EDDIE_PACKAGE_TEMPPATH  > /dev/null 2>&1
	fi
    set +x
	exit 0
}

# IPK Full path, Extraction path
function extract_ipk
{
    ipk_name=$1
    extract_path=$2
    print "Extracting IPK: $ipk_name at $extract_path"    
	if ! [ -f  $ipk_name ]; then
		error_exit $FUNCNAME " IPK $ipk_name not found."
	fi
    
    if ! [ -d $extract_path ]; then
        mkdir -p $extract_path || error_exit $FUNCNAME " Not able to create directory $extract_path for IPK extraction"
    fi
    $CP $ipk_name $extract_path/
    cd $extract_path/
    ar x $ipk_name
    rm -f $extract_path/*.ipk
    tar xzf $extract_path/data.tar.gz -C $extract_path
    #tar xzf $extract_path/control.tar.gz
    rm -f $extract_path/control.tar.gz $extract_path/data.tar.gz $extract_path/debian-binary    
}
#################### QC-HSP PACKAGES ###################
function create_hsp_package
{
	mkdir -p  $EDDIE_PACKAGE_TEMPPATH || error_exit $FUNCNAME " Not able to create directory $EDDIE_PACKAGE_TEMPPATH for HSP extraction"
	print "Extracting latest HSP package - image files from $HSP_PACKAGE."
	if ! [ -f $HSP_PACKAGE ]; then
		error_exit $FUNCNAME " HSP Package $HSP_PACKAGE not found. "
	fi	
	# Extract QC_HSP image and ext4 files at Fastboot output
	tar -C $EDDIE_PACKAGE_TEMPPATH -xf $HSP_PACKAGE $HSP_PACKAGE_FILES || error_exit $FUNCNAME " Extraction of latest Riviera HSP failed."
	# Move files from $EDDIE_PACKAGE_TEMPPATH/$RIVIERA_PACKAGE_UPDATEFILES_PATH to outside
	mv $EDDIE_PACKAGE_TEMPPATH/$HSP_PACKAGE_FILES $EDDIE_PACKAGE_TEMPPATH/ || error_exit $FUNCNAME " Unable to move Riviera HSP prebuilt Update images / EXT4s to $EDDIE_PACKAGE_TEMPPATH."
	rm -rf $EDDIE_PACKAGE_TEMPPATH/$HSP_PACKAGE_FILES  > /dev/null 2>&1
}

#################### EXT4 ###################
# $ext4_name (fullpath), $fs_extract_path, $partition_name, $newfiles
function create_ext4_package
{
	ext4_name=$1
	fs_extract_path=$2
	partition_name=$3
	newfiles=$4
    
    #ext4_full=$EDDIE_PACKAGE_TEMPPATH/$ext4_name
    print "Trying to make Partition $ext4_name at $fs_extract_path using packages at $newfiles"
    
	if ! [ -f  $ext4_name ]; then
		error_exit $FUNCNAME " Base EXT4 $ext4_name not found."
	fi
	if [ -d $newfiles ] || [ -f $newfiles ]; then
		# Create EXT4 out of Bose binaries built. For now, we are assuming that bin folder in images does have proper relative paths - like bin/opt/Bose/lib or bin/opt/Bose/bin e.g.. 
		# If not, this script will first of all have to create bosefs just like TI's rootfs and then create a EXT4 FS out of it.
		#extract empty / core Bose FS from $EDDIE_PACKAGE_TEMPPATH/$BOSEFS_NAME. -- Ignore error
		$utildir/lin/simg2img $ext4_name $ext4_name.raw || echo ""
		#find size in bytes of $ext4_full.raw to be used during rebuilding EXT4	=33554432 
		if ! [ -f  $ext4_name.raw ]; then
			error_exit $FUNCNAME " Not able to create intermediate RAW file $ext4_name.raw, using simg2img."
		fi
		FS_SIZE=$(stat -c%s "$ext4_name.raw")
		if [ $? != 0 ]; then
			error_exit $FUNCNAME " Not able to find size of $ext4_name from $ext4_name.raw."
		fi

		mv $ext4_name $ext4_name.orig  > /dev/null 2>&1

		#sudo
		mkdir -p $fs_extract_path || error_exit $FUNCNAME " Not able to create directory $fs_extract_path for $ext4_name extraction."
		#TODO mount requires root - This step is not required, if we decide that HSP's BoseFS is empty.
		#sudo mount -t ext4 -o loop $EDDIE_PACKAGE_TEMPPATH/$ext4_name.raw $fs_extract_path/ || error_exit $FUNCNAME " Unable to mount RAW EXT4 $ext4_name.raw"
		#sudo chmod -R 666 $fs_extract_path
		
		#overwrite base EXT4 with newly generated Bose Application files (which should be output of make)
		#sudo
		$CP $newfiles $fs_extract_path/		
		#make new EXT4 by extracting copied IPKs
		extract_ipk $newfiles $fs_extract_path
		
		#sudo
		$utildir/lin/make_ext4fs -s -l $FS_SIZE -a $partition_name $ext4_name $fs_extract_path/ || echo ""
		if ! [ -f  $ext4_name ]; then
			#sudo umount $fs_extract_path/
			error_exit $FUNCNAME " Updated EXT4 $ext4_name not found - make_ext4fs failed. Check if total size of package exceeds MAX EXT4 Size: $FS_SIZE."
		fi
		#sudo umount $fs_extract_path/
		# remove intermediate files
		rm -rf $fs_extract_path		
		rm -f $ext4_name.raw  > /dev/null 2>&1
		rm -f $ext4_name.orig   > /dev/null 2>&1
	else
		print_warn "Directory of build $newfiles does not exist. Using FS from Riviera HSP as-is."
	fi
}

function create_lpm_package 
{
	#$mkdir -p $EDDIE_PACKAGE_TEMPPATH/lpm_package || error_exit $FUNCNAME " Not able to create directory $EDDIE_PACKAGE_TEMPPATH/lpm_package for LPM update"
	if ! [ -f $LPM_PACKAGE ]; then
		error_exit $FUNCNAME " LPM firmware $LPM_PACKAGE not found. "
	fi
	print_debug "copy $LPM_PACKAGE $EDDIE_PACKAGE_TEMPPATH"

	$CP $LPM_PACKAGE $EDDIE_PACKAGE_TEMPPATH/

	# Copy utility to flash LPM package via serial port (tapload) in package
	#$CP $utildir/* $EDDIE_PACKAGE_TEMPPATH/ -- If we assume this is at same location as in utildir, no need of separate copy
}

function clean_package_space
{
	#################### CLEAN ###################
	# Delete fastboot output package if already present
	rm -rf $EDDIE_PACKAGE_TEMPPATH > /dev/null 2>&1
	rm -f $OUTPUT_PACKAGE > /dev/null 2>&1
	rm -f $OUTPUT_SCRIPT > /dev/null 2>&1
}

function create_final_package
{
	# Copy flashing utility
	$CP $utildir $EDDIE_PACKAGE_TEMPPATH/

	#Zip the package and copy our fastboot flash script at same location
	cd $EDDIE_PACKAGE_TEMPPATH/..
	tar -cf $EDDIE_PACKAGE_NAME ./$EDDIE_PACKAGE_DIRNAME/*
	mv $EDDIE_PACKAGE_TEMPPATH/../$EDDIE_PACKAGE_NAME $OUTPUT_PACKAGE
	$CP $EDDIE_FLASH_SCRIPT $OUTPUT_SCRIPT
	
	print "Final Package created at $OUTPUT_PACKAGE with script $OUTPUT_SCRIPT"
}

print "Cleaning Package Space."
clean_package_space
print "Creating HSP Package."
create_hsp_package
print "Creating Bose Partition."
# $ext4_name (fullpath), $fs_extract_path, $partition_name, $newfiles
# TODO: We may have to change this later to get path of all IPKs
create_ext4_package $EDDIE_PACKAGE_TEMPPATH/$BOSEFS_EXT4_NAME $EDDIE_PACKAGE_TEMPPATH/$BOSEFS_PARTITION_NAME $BOSEFS_PARTITION_NAME $SOUNDTOUCH_PACKAGE

#print "Creating Bose Persistance Partition."
#create_ext4_package $EDDIE_PACKAGE_TEMPPATH/$PERSIST_EXT4_NAME $EDDIE_PACKAGE_TEMPPATH/$PERSIST_PARTITION_NAME $PERSIST_PARTITION_NAME $SOUNDTOUCH_PERSIST_PACKAGE

#################### COPY LPM IMAGE AND FLASH utility ###################
if [ $with_lpm -eq 1 ]; then
 	print "Adding LPM Package."
 	create_lpm_package 
fi

#################### MAKE FINAL PACKAGE and VERIFY ###################
print "Creating Final Package for full update."
create_final_package

# Remove intermediate data and Exit
proper_exit "Package $EDDIE_PACKAGE_NAME created successfully".


