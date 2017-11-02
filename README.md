#### Eddie Jenkins Smoke Test Status
<a title='Jenkins build status for smoke tests against Eddie' href='http://41c679de.ngrok.io/view/Eddie/job/Eddie_Smoke_Tests/'><img src='http://41c679de.ngrok.io/view/Eddie/job/Eddie_Smoke_Tests/badge/icon'></a>

<!-- ngrok is used for secure tunnel so our jenkins server behind our firewall can be accessed from GitHub. When the tests are added and a pull request is submitted an automatic jenkins build is initiated. When that build is successful or failed it will automatically get updated in the Readme. We are using a jenkins plugin that uses API's to update the status of the jenkins build.-->

SoundTouch Eddie
================

This repo contains the source code and tools specific to the SoundTouch Eddie product.

##### Table of Contents  
[Getting Started](#start)  
[Reflash Riviera-HSP](#hsp)  
[Update LPM via APQ using IPK](#lpm)   
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
$ adb shell /opt/Bose/bin/rw        # make the file systems writeable
$ adb shell opkg remove SoundTouch  # this too may fail
$ adb push builds/Release/product.ipk /tmp/product.ipk
$ adb shell opkg install -d bose /tmp/product.ipk
$ adb shell reboot
```
(But see `putipk` below for a simpler way.)

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
$ cd `components get Riviera-HSP-Images installed_location`/images
$ sudo ./fastboot.sh
$ sudo fastboot reboot
```

<a name="lpm"/>

### Updating LPM from APQ using IPK

To update LPM from APQ:
```shell session
$ cd /scratch/
$ cp /home/softlib/verisoft/Eddie/Continuous/master/latest/eddie_lpm_updater_*.ipk eddie_lpm_updater.ipk
$ adb push /scratch/eddie_lpm_updater.ipk /dev/shm/
$ adb shell /opt/Bose/bin/rw
$ adb shell opkg remove eddie_lpm_updater
$ adb shell opkg -d bose install /dev/shm/eddie_lpm_updater.ipk
```
Once installation completes, unplug and replug power supply to Eddie board. Please note that power cycle is must for LPM firmware update to complete.

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
