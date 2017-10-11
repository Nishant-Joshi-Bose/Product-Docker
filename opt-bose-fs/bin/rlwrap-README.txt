rlwrap - readline wrapper.

Add command-line editing and history to the SoundTouch CLI.

See opt-bose-fs/bin/tap

------------------------------------------------------------------------------

For cross compiling:

 # components get Riviera-Toolchain installed_location
 # for example:
 toolchain=/scratch/components-cache/Release/master/1.1/Riviera-Toolchain

 sysroots=$toolchain/sdk/sysroots
 cross=arm-oemllib32-linux-
 cc="${cross}gcc --sysroot=$sysroots/aarch64-oe-linux"
 PATH=$PATH:$sysroots/i686-oesdk-linux/usr/bin/arm-oemllib32-linux

------------------------------------------------------------------------------

Get rlwrap source code:

$ cd /scratch # for example
$ git clone https://github.com/hanslub42/rlwrap.git
$ cd rlwrap
$ git checkout ef6cbb3b187d4478aacdae36ac126fcc9ef82b83
$

------------------------------------------------------------------------------

Cross compile rlwrap:

$ autoreconf --install
$ ./configure --host=arm-linux CC="$cc"
$ make
$ ${cross}strip rlwrap
$
$ file rlwrap
rlwrap: ELF 32-bit LSB executable, ARM, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.32, stripped
$
