#!/bin/bash
#
# Script to get processes that crashed in the re field, based on the core dump files generated
# After the crash is posted it is stored to crash_log, in order to avoid duplicate posts the crash_log is checked before posting new data
# The data is passed as the json payload to the run-telemetry script
#

DATE=$1
TELEMETRY_DOC_DIR=$2

#Core dumps to search for
re=$3

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
      printf '      {\n'
      printf '        "time": "%s",\n' $DATE
      printf '        "metricType": "%s:%s",\n' ${process} ${metric}
      printf '        "metricValue": %d\n' ${value}
      printf '      },\n'
  fi
done
