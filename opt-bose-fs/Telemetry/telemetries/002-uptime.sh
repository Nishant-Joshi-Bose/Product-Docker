#! /bin/bash
# 
# Script to get uptime of the device, by logging the current date
# The data is passed as the json payload to the run-telemetry script
#

DATE=$1

function get_uptime {
    local re="up +(0?([0-9]+) +min|0?([0-9]{1,2}):0?([0-9]{1,2})|0?([0-9]+) days?, +0?([0-9]{1,2}):0?([0-9]{1,2})|0?([0-9]+) days?, +([0-9]{1,2}) +min)"
    [[ $1 =~ $re ]] # $(uptime)

    if [[ -n ${BASH_REMATCH[8]} ]]
    then
	local D=${BASH_REMATCH[8]}
	local M=${BASH_REMATCH[9]}
	echo $(( $D*24*3600 + $M*60 ))
	return
    fi

    if [[ -n ${BASH_REMATCH[5]} ]]
    then
	local D=${BASH_REMATCH[5]}
	local H=${BASH_REMATCH[6]}
	local M=${BASH_REMATCH[7]}
	echo $(( $D*24*3600 + $H*3600 + $M*60 ))
    	return
    fi

    if [[ -n ${BASH_REMATCH[3]} ]]
    then
	local H=${BASH_REMATCH[3]}
	local M=${BASH_REMATCH[4]}
	echo $(( $H*3600 + $M*60 ))
    	return
    fi

    if [[ -n ${BASH_REMATCH[2]} ]]
    then
	local M=${BASH_REMATCH[2]}
	echo $(( $M*60 ))
    	return
    fi
}

utime="$(uptime)"
printf '      {\n'
printf '        "time": "%s",\n' $DATE
printf '        "metricType": "device:uptime",\n'
printf '        "metricValue": %d\n' $(get_uptime "$utime")
printf '      }\n'