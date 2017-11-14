#! /bin/bash

MACADDR=$1
DATE=$2

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
#printf "<uptime uptime=\"%d\"/>\n" $(get_uptime "$utime")
#printf "<uptime_str>$utime</uptime_str>\n"
printf '    {\n'
printf '        "time": "%s",\n' $DATE
printf '        "type": "metric",\n'
printf '        "originatorID": "%s",\n' $MACADDR
printf '        "data": {\n'
printf '            "metricType": "device/uptime",\n'
printf '            "metricValue": %d\n' $(get_uptime "$utime")
printf '        }\n'
printf '    }\n'
