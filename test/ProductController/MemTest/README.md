### Memory & Disk Usage tracking tool  

This folder contains memory consumption and graphite server utilities that assist in keeping the overall memory, disk, cpu usage of the processes running on the device in check, i.e within acceptable threshold levels.

#### Pre-test Setup

Before running the memory tests in this directory, the following must be done first:
1. You must have a Madrid/Gigya account. If you don't already have one, create one through Madrid.
2. Put the account and Professor dut on the INTEGRATION environment here: https://ingress-platform.live-aws-useast1.bose.io/dev/svc-cloudops/prod/cloudops-service/manageatp
3. Set up the Professor on that Madrid account
4. Configure iHeart radio as an MSP through Madrid (You will need an iHeart Radio account)
5. Set Kiss 108 as a preset for the 6 preset slots once out-of-box setup is done

#### Test Setup

Fields in conf_memoryConsumption.py with "SET ME" need to be specified before running the tests.

'Device_Param': 'SEND_TO' - Put in an email to send memory results to

'Gigya_Account': 'Email' - Email for the Madird/Gigya account the Professor is on

'Gigya_Account': 'Password' - Password for the Madird/Gigya account

'Source_Data': 'sourceAccount' - Do the following:
1. On Linux, run the commands:
```
adb shell ifconfig # Take note of the IP address of your Professor
google-chrome --ignore-certificate-errors --disable-b-security --user-data-dir # You will need to download Google Chrome to run this
```
2. Copy/Paste this URL into Chrome and plug in your Professor's IP address: file:///scratch/bose-web-test-page/index.html?<IP Address>
3. Press "ACCOUNT LOGIN" and log into your Madrid account
4. Press "CONNECT"
5. Select "Method" as "GET"
6. Type /system/sources into the "Path" and press "SEND"
7. Find the entry for iHEART Radio (It should list "IHEART" as "sourceName" and copy/paste the contents for the "accountId" found for that entry 
into the config file field.

**To run the Memory Consumption Test:**

```
 pytest -vs test_memoryConsumption.py --network-interface <wlan0, eth0> --device <adb device id> --IsMandatoryDataPush=True --test-duration=<# between 1 and 29>
```
**System Metrics**:
**Memory / CPU Capture Details**:
1. Used cat /proc/meminfo to capture Memory Free and Memory Used with below formula
    * Memory Free = MemFree + Inactive(anon) + Inactive(File) + SReclaimable
    * Memory Used = MemTotal – (MemFree + Inactive(anon) + Inactive(File) + SReclaimable)
2. Used 'top –n 1' command to capture CPU %
3. Use df -h command to capture Disk Usage 
4. Top 10 Process - ps -o comm,pid,ppid,time,vsz,stat,rss

**Test Flow 1:**
1. Perform a S/W update; grabs the latest build, also updates Riviera-HSP - TBD
2. Capture system metrics for 5 minutes before performing any operation. (Memory/CPU usage before any operation)
3. plays Tune in with different presets (1-6 every 5 mins) station using playTuneInMusic for 30mins -- In parallel, collect system metrics.
4. After 30 mins, stop playing Tune-In.
5. Script pushed data collected to different dashboards. (Grafana)

**Test Flow 2:**
1. Perform a S/W update; grabs the latest build, also updates Riviera-HSP - TBD
2. Capture system metrics for 5 minutes before performing any operation. (Memory/CPU usage before any operation)
3. Kick off Stress Test for 30 mins -- In parallel, collect system metrics.
4. After 30 mins, stop the stress test process
5. Script pushed data collected to different dashboards. (Grafana)


**What happens after?**
1. Threshold levels are set at Grafana, to monitor for overusage of CPU / memory
2. Webhooks have been set to publish those notifications on slack/email.

**Useful Links:**
1. [Tune-In - Memory, CPU, Disk Space] <http://usvassgdb.bose.com/dashboard/db/memory-cpu-usage-overview-eco2?orgId=1&from=1509564862361&to=1509569495699>

2. [Tune-In - Top 10 Process] <http://usvassgdb.bose.com/dashboard/db/memory-cpu-usage-overview-eco2?orgId=1&from=1509564862361&to=1509569495699>

3. [Stress Test - Memory, CPU, Disk Space] <http://usvassgdb.bose.com/dashboard/db/webkitbrowser-memory-cpu-usage-overview-eco2?orgId=1&from=1511796850215&to=1511800450215>

4. [Stress Test - Top 10 Process] <http://usvassgdb.bose.com/dashboard/db/top-10-process-webkitbroswer-memory-consumption-eco2?orgId=1&from=1511789680361&to=1511800480362&var-Process=All>
    
