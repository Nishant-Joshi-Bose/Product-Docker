#!/bin/bash
# 
# Script to get processes specified in re, and capture there Vm and Threads data 
# The data is passed as the payload json to the run-telemetry script
#

MACADDR=$1
DATE=$2

#Processes to search for
re=$4

re2="([^ ]*) +([0-9]+)"
re3="(.+):\s+([0-9]+)( kB){0,1}"
ps -eocomm,pid | egrep "$re" | while read procspec
do
    [[ $procspec =~ $re2 ]]
    process=${BASH_REMATCH[1]}
    pid=${BASH_REMATCH[2]}
    cat /proc/$pid/status | egrep "^(Vm|Threads).*$" | while read -r proc
    do
     [[ $proc =~ $re3 ]]
      metric=${BASH_REMATCH[1]}
      value=${BASH_REMATCH[2]}
      printf '    {\n'
      printf '        "time": "%s",\n' $DATE
      printf '        "type": "metric",\n'
      printf '        "originatorID": "%s",\n' $MACADDR
      printf '        "data": {\n'
      printf '            "metricType": "%s/%s",\n' ${process} ${metric}
      printf '            "metricValue": %d\n' ${value}
      printf '        }\n'
      printf '    },\n'
    done
done
