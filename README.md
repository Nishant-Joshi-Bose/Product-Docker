SoundTouch Eddie
================

This repo contains the source code and tools specific to the SoundTouch Eddie product.

##### Table of Contents  
[Getting Started](#start)  
[Reflash Riviera-HSP](#hsp)  
[More...](#more)  

<a name="start"/>

### Getting Started

Checkout CastleTools.git and Eddie.git:
```shell session
$ cd /scratch
$ git clone git@github.com:BoseCorp/CastleTools.git
$ PATH=$PATH:/scratch/CastleTools/bin   # add this to your ~/.profile, ~/.bash_profile or ~/.login
$ git clone git@github.com:BoseCorp/Eddie.git
```

Build the .ipk package file containing the SoundTouch software.
```shell session
$ cd /scratch/Eddie
$ make
```

Make sure your Eddie unit is accessible via adb.
```shell session
$ sudo adb start-server             # must be done as root. typically once per boot of the build host
$ adb devices
List of devices attached
5166240	device

$
```

Install the .ipk file you built.
```shell session
$ adb shell /opt/Bose/bin/stop      # generally it's okay if this fails
$ adb shell opkg remove SoundTouch  # this too may fail
$ adb push builds/Release/product.ipk /tmp/product.ipk
$ adb shell opkg install -d bose /tmp/product.ipk
$ adb shell reboot
```

You'll get a notification if your Riviera unit is running old Riviera software:
```shell session
...
Built for Riviera-HSP: 0.5-9-geee2c72
Installed Riviera-HSP: 0.5-7-g856bf73
...
```

To update the HSP, see the next section.

<a name="hsp"/>

### Reflashing the Riviera HSP

To reflash the Riviera HSP:
```shell session
$ cd /scratch/Eddie
$ components install
$ adb shell reboot bootloader       # reboot the Riviera unit into the fastboot bootloader
$ sudo fastboot devices             # make sure the unit is in the bootloader
$ cd components/Riviera-HSP/images
$ sudo ./fastboot.sh
$ sudo fastboot reboot
```

### More...

To rebuild the .ipk file and install via adb in one step:

```shell session
$ ./scripts/putipk jobs=4
```

Access the APQ console via the tap cable.

```shell session
$ cat /etc/minirc.usb0
pr port             /dev/ttyUSB0
pu baudrate         115200
pu bits             8
pu parity           N
pu stopbits         1
pu minit
pu mreset
pu mhangup
pu rtscts           No
pu logfname         /dev/null
$ minicom -w -C minicom.cap usb0
```

Use `dmesg` to see if your tap cable is actually USB0.

If you use `adb shell` to login, you won't have the usual environment by default.
To setup the usual environment:

```shell session
$ adb shell
/ # HOME=/opt/Bose exec bash -login
Sat Sep  2 12:10:12 UTC 2017
Device name: "Bose SoundTouch C7E3A2"
mc1014468@hepdsw64.bose.com 2017-08-31T08:40:21 master 0.0.1-1+3e07c68
#
# type start
start is /opt/Bose/bin/start
#
```

Certain important error and status messages go only to the console.
You generally won't see this information via `adb shell`.
