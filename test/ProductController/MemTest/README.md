#### Memory & Disk Usage tracking tool  

This folder contains memory consumption and graphite server utilities that assist in keeping the overall memory, disk, cpu usage of the processes running on the device in check, i.e within acceptable threshold levels.

**To run the Memory Consumption Test:**

```
 git clone git@github.com:BoseCorp/CastleTestUtils.git 
 cd CastleTestUtils/MemoryUtils
 pytest -vs test_memoryConsumption.py --network-interface wlan0 --device 84a9567 --IsMandatoryDataPush=True --test-duration=30
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
    
