Professor
=========
<a title='Latest release in GitHub' target='_blank' href='https://github.com/BoseCorp/Professor'><img src='https://bose-prod.apigee.net/core02/svc-version-badge/prod/version-badge-core/github/latest-version/Professor/latest release/blue'></a>

This repo contains the source code and tools specific to the Professor product.

For updates about new releases of this software, you can subscribe [here](https://platform.bose.io/dev/svc-embedded-releases/stable-test/web-server/).

For more information, see the [Professor wiki page](https://wiki.bose.com/display/A4V/Professor).

![Professor](professor.png)

##### Table of Contents
[Getting Started](#start)  
[Compiling Professor](#compile)  
[Installing Professor](#install)  
[Reflash Riviera-HSP](#hsp)  
[PTS Server Links](#pts)  
[More...](#more)  
[Useful Links](#useful)  

<a name="start"/>

### Getting Started

Checkout CastleTools.git and Professor.git:
```shell session
$ cd /scratch
$ git clone git@github.com:BoseCorp/CastleTools.git
$ PATH=$PATH:/scratch/CastleTools/bin   # add this to your ~/.profile, ~/.bash_profile or ~/.login
$ git clone git@github.com:BoseCorp/Professor.git
```

<a name="compile">

### Compiling Professor

How you compile the software will depend largely on how you plan to flash it to the device. Several options are listed below.

#### 'make update-zip' for Bonjour Update

Build the Professor product_update.zip to install over ethernet.
```shell session
$ cd /scratch/Professor
$ make packages-gz update-zip
```

<a name="compile">

### Installing Professor

There are a number of different ways in which you can flash the software to your device, several of which are listed below.
Regardless of the method you used, you can verify that your update was successful by running the following command to check the version:

```shell session
adb shell LD_LIBRARY_PATH=/opt/Bose/update/opkg /opt/Bose/update/opkg/opkg -f /mnt/nv/update/opkg.conf --volatile-cache  --add-arch armv7a-vfp-neon:100  list
```

QFIL/VIP can be used to recover or update the system, however it is highly recommended to use Bonjour when possible.

If you are unable to update with Bonjour, follow the instructions on the wiki for [How to recover Professor with VIP](http://wiki.bose.com/display/A4V/How+to+recover+Professor+with+VIP)

If the device is unresponsive after updating, perhaps the [How to tapload Professor / Ginger-Cheevers](http://wiki.bose.com/pages/viewpage.action?pageId=42180408) wiki article will be helpful.
If you are having issues taploading (specifically with sending binaries to Professor) ensure that the tap cable is connected directly to your machine. Some USB hubs send erroneous characters before the binary file, causing the write to timeout prematurely. 

#### Bonjour

Method 1:

Power on the device and attach an ethernet cable. Once the device is booted, use adb to query the ip address of the eth0 network interface:
```shell session
$ adb kill-server
$ sudo adb start-server
$ adb shell
# ifconfig
eth0      Link encap:Ethernet  HWaddr 04:B0:5E:56:76:FC
          inet addr:10.60.5.51  Bcast:10.60.46.255  Mask:255.255.255.0
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:8922 errors:0 dropped:155 overruns:0 frame:20241
          TX packets:40851 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:1617211 (1.5 MiB)  TX bytes:4253570 (4.0 MiB)
          Interrupt:137

lo        Link encap:Local Loopback
          inet addr:127.0.0.1  Mask:255.0.0.0
          inet6 addr: ::1/128 Scope:Host
          UP LOOPBACK RUNNING  MTU:65536  Metric:1
          RX packets:91517 errors:0 dropped:0 overruns:0 frame:0
          TX packets:91517 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:0
          RX bytes:31124620 (29.6 MiB)  TX bytes:31124620 (29.6 MiB)
```

Connect to your device through a web browser by going to http://<inet_addr>:17008/update.html replacing <inet_addr> with the ip address of the eth0 interface listed in `ifconfig`

The following page should load:
![Choose File](choose_file.png)

Click the "Choose File" button and select an update zip file, which can be found in any Electric Commander build (e.g., \\\solid\softlib\verisoft\Professor\Release\master\0.3.2-891+2fe56de\HSP-2.1\product_update.zip) or built using the instructions above. 

After the transfer is completed, you should see a page that looks like the following:
![Update Finished](update_finished.png)

Your device will reboot twice, and, after it has finished, your update should be complete.

Method 2:

To perform Bonjour-Update; use the pushup script:

From the Professor workspace:

```shell session
$ cd /scratch/Professor
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

#### Putipk_ota Script

Make sure your Professor unit is accessible via adb.
```shell session
$ sudo adb start-server             # must be done as root. typically once per boot of the build host
$ adb devices
List of devices attached
5166240   device

$
```

Use the putipk_ota script to install the .ipk package you built.
```shell session
$ sudo ./scripts/putipk_ota builds/Release/product.ipk
```

Alternatively, you can use the putipk_ota script without specifying a file, and it will rebuild the .ipk for you.
```shell session
$ sudo ./scripts/putipk_ota
```

<a name="hsp"/>

### Reflashing the Riviera HSP

To reflash the Riviera HSP from Linux:
```shell session
$ cd /scratch/Professor
$ components install
$ adb shell reboot bootloader       # reboot the Riviera unit into the fastboot bootloader
$ sudo fastboot devices             # make sure the unit is in the bootloader
$ cd `components get Riviera-HSP-Images installed_location`
$ sudo ./fastboot.sh
$ sudo fastboot reboot
```

To reflash the Riviera HSP from Windows Command Prompt:
```shell session
$ adb reboot bootloader             # reboot the Riviera unit into the fastboot bootloader
$ fastboot devices                  # check that the fastboot device is present
$ fastboot.sh                       # run the fastboot script

*** See the Fastboot section of https://wiki.bose.com/display/BC/1.+Flashing+the+SNAP+Board for Fastboot
    installation instructions.
```

<a name="lpm"/>

### Updating LPM from APQ using IPK

To update LPM from APQ:
```shell session
$ cd /scratch/Professor
$ ./scripts/putlpm  # Makes lpm_updater.ipk based on package in components.json and installs
$ ./scripts/putlpm ./builds/Release/lpm_updater.ipk # Install LPM ipk generated by `make package`
$ ./scripts/putlpm <path-to-lpm-ipk> # Install a specific LPM ipk
```

### Building different LPM BLOBs

By default the BLOB that is used to create the lpm_updater.ipk will be a Release BLOB based off of the release manifest xml located in
the LpmManifestFiles directory. If you wish to create a BLOB based on Continuous or Nightly builds of components try building with the commands below:
```shell session
# Create a BLOB using the LpmManifestFiles/continuous_lpm_package.xml
$ make lpmupdater-ipk BUILD_TYPE=Continuous

# Create a BLOB using the LpmManifestFiles/nightly_lpm_package.xml
$ make lpmupdater-ipk BUILD_TYPE=Nightly

# Create a BLOB using the LpmManifestFiles/release_lpm_package.xml
# Note: By default the build process will use this method.
$ make lpmupdater-ipk BUILD_TYPE=Release
```

<a name="pts"/>

### PTS Server Links

The PTS Server is a tiny embedded web server.  The server was originally
intended for information to investigate customer problems reported to the
technical support team.  The server has evolved to have a broader purpose.

For security reasons, some endpoints are accessible only via certain
interfaces.  For testing, you can disable this
restriction: `touch /mnt/nv/product-persistence/anyiface`.

| Interface | Endpoint | Description |
| -------- | -------- | ----------- |
| usb | /diag | Quality Audit |
| any | /logread.txt | A snapshot of the system log ring buffer * |
| any | /logread.txt.gz | Same as /logread.txt but compressed * |
| any | /lpmlogread.txt | Live feed of all LPM logs *** |
| any | /pts.txt | Various Linux and system status information (e.g., ifconfig) * |
| any | /pts.dat | Same as /pts.txt but encrypted ** |
| any | /network.txt | Various network and WAN information * |
| any | /network.dat | Same as /network.txt but encrypted ** |
| any | /recovery.txt | Recovery mode logs for HSP update ** |
| any | /recovery.dat | Same as /recovery.txt but encrypted * |
| any | /logread.dat | Same as /logread.txt but encrypted ** |
| usb | /reflash | Force the unit into reflash mode (QFIL) |
| usb | /bose-version | /opt/Bose/etc/BoseVersion.json |
| usb | /kernel-version | /etc/riviera-version |
| usb | /validate-mfgdata | Check the manufacturing data |
| usb | /controller-version | The LPM version strings |
| usb | /clear-first-greeting | Clear the flag indicating the unit's first boot |
| usb | /device-id | Displays the public device ID. Used on the mfg line after user code is put on the system to verify the device ID with Galapagos. |
| any | /opensource | List the licenses of open source software used in the system |
| any | /service | Remanufacturing. Only if the unit is in service mode |
| any | /dev | Developer links * |
| usb, wlan1 | /, /index.html | Wi-Fi setup |

\* Only if the unit is in development mode

\** See [decrypt-logs](https://github.com/BoseCorp/CastleTools/blob/master/bin/decrypt-logs) to decrypt

\*** See [Riviera LPM Logging](https://wiki.bose.com/display/BC/Riviera+LPM+Logging) for more information on retrieving LPM logs.

The USB IP address is 203.0.113.1.
For example: http://203.0.113.1/pts.txt

<a name="more"/>

### More...

[Join the Professor Slack channel.](https://bosessg.slack.com/messages/CBMRG7ATV)

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

<a name="useful"/>
### Useful Links

[Professor/Ginger-Cheevers Slack channel](https://profgc.slack.com/)

[Professor Wiki](https://wiki.bose.com/display/A4V/Professor)

[How-to & Board Setups](https://wiki.bose.com/pages/viewpage.action?pageId=33361043)

[How to recover Professor with VIP](https://wiki.bose.com/display/A4V/How+to+recover+Professor+with+VIP)

[How to tapload Professor / Ginger-Cheevers](https://wiki.bose.com/pages/viewpage.action?pageId=42180408)
