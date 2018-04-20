#! /bin/bash
#
# Script to generate all telemetric json reports in the TELEMETRY_DOC_DIR.
# Each report name is prefixed with the mac address and the date it was collected.
# After the structure of the json message is generated all the scripts in the telemetric folder are run to populate the payload of the message.
#

processes_to_track="APServer|AVS|A4V|Bose|BT|BLESetup|CLIServer|ClockSync|Deezer|Device|Eddie|FrontDoor|GalapagosClient|HTTPProxy|IHeart|Iot|LPMService|Passport|Professor|NetManger|Server|Software|Spotify|SWUpdateWebServer|STS|Telnet|STSCertified|UpnpSource|Vfe"

BASEDIR=$(dirname $(readlink -f $0))
. $BASEDIR/common-telemetry.sh
#echo $BASEDIR
#echo $TELEMETRY_DOC_DIR

[[ -d  "$TELEMETRY_DOC_DIR" ]] || mkdir -p "$TELEMETRY_DOC_DIR"

##
## Check to see if we've built up too many files that haven't been uploaded
## successfully to the server yet
##
CNT=$(ls "$TELEMETRY_DOC_DIR" | wc -l) # count the number of files
[[ $CNT -gt 72 ]] && exit 0  # don't let things build up tooo much

cd ${SHELBY_TELEMETRY_HOME:-$BASEDIR}

productID=$(mfgdata get guid)
productType=$(mfgdata get productType)
productName=$(mfgdata get productName)
productSerialNumber=$(mfgdata get snSystem)
softwareversion=$(jq -r '"\(.short)-\(.build_number)+\(.abbrev_commit)"' /opt/Bose/etc/BoseVersion.json)
# Leaving bosepersonid as a place holder for now, will be added later
bosepersonid="00000000-0000-0000-0000-000000000000"

epochdate=$(date +%s --utc)
date=$(date +"%Y-%m-%dT%H:%M:%SZ" -d @$epochdate)
datestr=$(date +"%Y%m%d-%H%M%SZ" -d @$epochdate)

exec 6>$TELEMETRY_DOC_DIR/report_${productID}_${datestr}.json
printf '{\n' >&6
printf '  "envelope": {\n' >&6
printf '    "protocolVersion": "2.0",\n' >&6
printf '    "payloadType": "telemetry",\n' >&6
printf '    "payloadProtocolVersion": "1.0",\n' >&6
printf '    "uniqueID": "%s",\n' $productID >&6
printf '    "time": "%s"\n' $date >&6
printf '  },\n' >&6
printf '  "payload": {\n' >&6

printf '    "originatorInfo": {\n' >&6
printf '      "softwareVersion": "%s",\n' $softwareversion >&6
printf '      "productSerialNumber": "%s",\n' $productSerialNumber >&6
printf '      "productType": "%s",\n' $productType >&6
printf '      "productName": "%s %s %s %s",\n' $productName >&6
printf '      "productID": "%s",\n' $productID >&6
printf '      "bosePersonID": "%s"\n' $bosepersonid >&6
printf '    },\n' >&6
printf '    "metrics": [\n' >&6
for src in `ls $BASEDIR/telemetries/*.sh`
do
    bash $src ${date} $TELEMETRY_DOC_DIR "$processes_to_track" >&6
done
printf "    ]\n" >&6
printf "  }\n" >&6
printf "}\n" >&6

exec 6>&-

# debug
#cat $TELEMETRY_DOC_DIR/report_${productID}_${datestr}.json
