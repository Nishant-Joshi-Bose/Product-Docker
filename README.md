SoundTouch Professor
====================

This repo contains the source code and tools specific to the SoundTouch Professor product.

##### Table of Contents  
[Getting Started](#start)  

<a name="start"/>

### Getting Started

Checkout CastleTools.git and Professor.git:
```shell session
$ cd /scratch
$ git clone git@github.com:BoseCorp/CastleTools.git
$ PATH=$PATH:/scratch/CastleTools/bin   # add this to your ~/.profile, ~/.bash_profile or ~/.login
$ git clone git@github.com:BoseCorp/Professor.git
```

You should update your CastleTools workspace on a regular basis.
```shell session
$ cd /scratch/CastleTools
$ git pull
```

Build the .ipk package file containing the SoundTouch software.
```shell session
$ cd /scratch/Professor
$ make
```

Make sure your Professor unit is accessible via adb.
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
$ adb push /scratch/Professor/builds/Release/professor.ipk /tmp/professor.ipk
$ adb shell opkg install -d bose --force-reinstall /tmp/professor.ipk
$ adb shell reboot
```

You'll get a notification if your Professor unit is running old Riviera software:
```shell session
...
Built for Riviera-HSP: 0.5-9-geee2c72
Installed Riviera-HSP: 0.5-7-g856bf73
...
```

To reflash the Riviera HSP:
```shell session
$ cd /scratch/Professor
$ components install
$ adb shell reboot bootloader       # reboot the Professor unit into the fastboot bootloader
$ sudo fastboot devices              # make sure the unit is in the bootloader
$ cd components/Riviera-HSP/images
$ sudo ./fastboot.sh
$ sudo fastboot reboot
```
