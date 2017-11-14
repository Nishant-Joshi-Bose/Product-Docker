#!/bin/bash
MACADDR=$1
DATE=$2
TELEMETRY_DOC_DIR=$3

#Core dumps to search for
re="APServer|AVS|A4V|Bose|BT|BLESetup|CLIServer|ClockSync|Deezer|Device|Eddie|FrontDoor|IHeart|Iot|LPMService|Passport|Professor|NetManger|Server|Software|Spotify|Telnet|STSCertified|UpnpSource|WebServer|Vfe" 

crash_log=$TELEMETRY_DOC_DIR"/crashes_posted.txt" #Create a log to remember what core dump info has already been posted
if [ ! -e "$crash_log" ] ; then
    touch "$crash_log"
fi

find /data/logs -name "*core*" | egrep "$re" | while read procspec
do
  if ! grep -q "$procspec" "$crash_log"; then #Only post core dump info that has not been posted yet
      echo $procspec >> $crash_log
      process=$(echo $procspec | egrep -o "$re" )
      DATE=$(echo $procspec | cut -d"-" -f6)
      DATE=$(date +"%Y-%m-%dT%H:%M:%SZ" -d @$DATE) 
      metric="Crash_Signal"
      value=$(echo $procspec | cut -d"-" -f7 | cut -d"." -f1) #Number of signal causing dump
      printf '    {\n'
      printf '        "time": "%s",\n' $DATE
      printf '        "type": "metric",\n'
      printf '        "originatorID": "%s",\n' $MACADDR
      printf '        "data": {\n'
      printf '            "metricType": "%s/%s",\n' ${process} ${metric}
      printf '            "metricValue": %d\n' ${value}
      printf '        }\n'
      printf '    },\n'
  fi
done
