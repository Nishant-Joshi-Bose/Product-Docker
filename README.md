

SoundTouch Eddie 
================
<a title='Latest release in GitHub' target='_blank' href='https://github.com/BoseCorp/Eddie'><img src='https://bose-prod.apigee.net/core02/svc-version-badge/prod/version-badge-core/github/latest-version/Eddie/latest release/blue'></a>
[Eddie Continuous build Testing] <a title='Jenkins build status for Eddie' href='http://jnkwebhook.ngrok.io/job/EddieProduct/job/Eddie_Continuous_Build_Testing/'><img src='http://jnkwebhook.ngrok.io/job/EddieProduct/job/Eddie_Continuous_Build_Testing/badge/icon'></a> [Eddie Nightly build testing] <a title='Jenkins build status for Eddie' href='http://jnkwebhook.ngrok.io/job/Pipelines/job/Eddie-Pipeline/'><img src='http://jnkwebhook.ngrok.io/job/Pipelines/job/Eddie-Pipeline/badge/icon'></a>

<!-- ngrok is used for secure tunnel so our jenkins server behind our firewall can be accessed from GitHub. When the tests are added and a pull request is submitted an automatic jenkins build is initiated. When that build is successful or failed it will automatically get updated in the Readme. We are using a jenkins plugin that uses API's to update the status of the jenkins build.-->


This repo contains the source code and tools specific to the SoundTouch Eddie product.

![Eddie](eddie.png)

##### Table of Contents
[Software Updates](#updates)   
[Getting Started](#start)   
[External References](#links)   
[More...](#more)  

<a name="updates"/>

### Software Updates

To update the software (SoundTouch, HSP, LPM, etc) as a user, it is highly recommended to use Bonjour.
Follow the instructions found on the wiki: [Bonjour Update of Eddie](https://wiki.bose.com/display/WSSW/Bonjour+Update+of+Eddie).
For a simpler-way; see ./pushup script

Do this whenever you need to update the HSP. Thereafter you may use opkg and etc. to install newer versions of SoundTouch as described later in this document.


<a name="start"/>

### Getting Started

Checkout CastleTools.git and Eddie.git:
```shell session
$ cd /scratch
$ git clone git@github.com:BoseCorp/CastleTools.git
$ PATH=$PATH:/scratch/CastleTools/bin   # add this to your ~/.profile, ~/.bash_profile or ~/.login
$ git clone git@github.com:BoseCorp/Eddie.git
```

Make sure your Eddie unit is accessible via adb.
```shell session
$ sudo adb start-server             # must be done as root. typically once per boot of the build host
$ adb devices
List of devices attached
5166240	device

$
```

To perform Bonjour-Update; use the pushup script:

From the Eddie workspace:

```shell session
$ cd /scratch/Eddie
$ pip2.7 install -r Test/requirements.txt
$ make all-packages
```

```shell session
To update without HSP:
$ ./scripts/pushup 

To update HSP:
$ ./scripts/pushup --hsp 

When having multiple devices, without HSP:
$ ./scripts/pushup --deviceid <deviceid>

When having multiple devices, with HSP:
$ ./scripts/pushup --deviceid <deviceid> --hsp
```

In general:

```shell session
$ cd /scratch
$ git clone git@github.com:BoseCorp/CastleTestUtils.git
$ cd CastleTestUtils
$ pip2.7 install -r requirements.txt
$ ./CastleTestUtils/scripts/pushup --deviceid <device-id> --zipfile <path-to-zipfile>
```

<a name="links"/>

### External References

Eddie [Getting Started](https://wiki.bose.com/display/WSSW/Eddie+Quick+Start+Guide)

[Updating Individual Components on Eddie](https://wiki.bose.com/display/WSSW/Updating+Individual+Components+on+Eddie)

### Building different LPM BLOBs

By default the BLOB that is used to create the lpm_updater.ipk will be a Release BLOB based off of the release manifest xml located in
the LpmManifestFiles directory. If you wish to create a BLOB based on Continuous or Nightly builds of components try building with the commands below:
```shell session
# Install components and run make normally
$ make
# After running make you can now create different types of LPM BLOBs

# Create a BLOB using the LpmManifestFiles/continuous_lpm_package.xml
$ make lpmupdater-ipk BUILD_TYPE=Continuous

# Create a BLOB using the LpmManifestFiles/nightly_lpm_package.xml
$ make lpmupdater-ipk BUILD_TYPE=Nightly

# Create a BLOB using the LpmManifestFiles/release_lpm_package.xml
# Note: By default the build process will use this method.
$ make lpmupdater-ipk BUILD_TYPE=Release
```


### More...

Ask to be added to the SSG-Eddie mailing list to stay in the loop by clicking
[here](mailto:Jonathan_Cooper@bose.com?subject=Add%20me%20to%20the%20SSG-Eddie%20mailing%20list).

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

To enable development mode:

```shell session
# mount -oremount,rw /persist
# mfgdata set development true
# mount -oremount,ro /persist
```

This flag enables core dumps, telnet access and other debug features.
In particular, if a daemon dies unexpectedly, no automatic recovery
happens when development mode is enabled.
