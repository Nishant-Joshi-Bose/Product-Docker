#! /bin/bash
#
# Script to generate all telemetric json reports in the TELEMETRY_DOC_DIR. 
# Each report name is prefixed with the mac address and the date it was collected.
# After the structure of the json message is generated all the scripts in the telemetric folder are run to populate the payload of the message.
#

processes_to_track="APServer|AVS|A4V|Bose|BT|BLESetup|CLIServer|ClockSync|Deezer|Device|Eddie|FrontDoor|IHeart|Iot|LPMService|Passport|Professor|NetManger|Server|Software|Spotify|Telnet|STSCertified|UpnpSource|WebServer|Vfe"

BASEDIR=$(dirname $(readlink -f $0))
. $BASEDIR/common-telemetry.sh
echo $BASEDIR
echo $TELEMETRY_DOC_DIR

[[ -d  "$TELEMETRY_DOC_DIR" ]] || mkdir -p "$TELEMETRY_DOC_DIR"

##
## Check to see if we've built up too many files that haven't been uploaded
## successfully to the server yet
##
CNT=$(ls "$TELEMETRY_DOC_DIR" | wc -l) # count the number of files
[[ $CNT -gt 72 ]] && exit 0  # don't let things build up tooo much


cd ${SHELBY_TELEMETRY_HOME:-$BASEDIR}

function get_macaddr {
    local re="(HWaddr|ether) ([A-Fa-f0-9:]*)"
    [[ $1 =~ $re ]]
    local MACADDR=${BASH_REMATCH[2]//:/}
    echo ${MACADDR^^}
}

function get_ipaddr {
    local re="(inet addr:|inet )([0-9\.]*)"
    [[ $1 =~ $re ]]
    echo ${BASH_REMATCH[1]}
}

##
## get macaddr
##
ifconfiginfo=$(ifconfig)
macaddr=$(get_macaddr "$ifconfiginfo")
ipaddr=$(get_ipaddr "$ifconfiginfo")
epochdate=$(date +%s --utc)
date=$(date +"%Y-%m-%dT%H:%M:%SZ" -d @$epochdate)
datestr=$(date +"%Y%m%d-%H%M%SZ" -d @$epochdate)

# debug
#echo $macaddr  
#echo $date


exec 6>$TELEMETRY_DOC_DIR/report_${macaddr}_${datestr}.json
#printf '<telemetry_report version="4" macaddr="%s" ipaddr="%s" date="%s">\n' "$macaddr" "$ipaddr" "$date" >&6
printf '{\n' >&6
printf '  "envelope": {\n' >&6
printf '    "protocolVersion": "2.0",\n' >&6
printf '    "payloadType": "telemetry",\n' >&6
printf '    "payloadProtocolVersion": "1.0",\n' >&6
printf '    "senderID": "%s",\n' $macaddr >&6
printf '    "time": "%s"\n' $date >&6
printf '  },\n' >&6
printf '  "payload": [\n' >&6
for src in `ls $BASEDIR/telemetries/*.sh`
do
   bash $src ${macaddr} ${date} $TELEMETRY_DOC_DIR $processes_to_track >&6
done
printf "  ]\n" >&6
printf "}\n" >&6

exec 6>&-


# debug
#cat $TELEMETRY_DOC_DIR/report_${macaddr}_${datestr}.json
