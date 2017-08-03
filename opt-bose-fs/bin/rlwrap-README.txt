rlwrap - readline wrapper.

Add command-line editing and history to the Shelby CLI.

See TI-HSP/fs/bin/tap.

------------------------------------------------------------------------------

For cross compiling:

 sysroots=/scratch/qc-sdk-8017-0.2/sysroots
 toolchain=arm-oemllib32-linux-
 cc="${toolchain}gcc --sysroot=$sysroots/aarch64-oe-linux"
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
$
$ cd src
$ arm-linux-gnueabihf-gcc -DDATADIR=\"/usr/local/share\"  -g -O2   -o rlwrap main.o signals.o readline.o pty.o completion.o term.o ptytty.o utils.o string_utils.o malloc_debug.o /scratch/libreadline.a -lutil -lcurses
$
$ ${toolchain}strip rlwrap
$
$ file rlwrap
rlwrap: ELF 32-bit LSB executable, ARM, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.32, stripped
$
