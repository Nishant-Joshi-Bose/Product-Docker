#!/bin/bash
#
# Script to post all telemetric json reports in the TELEMETRY_DOC_DIR to the cloud, after the report is posted successfully it is deleted.
# If posting the report fails, it is not deleted and the script will try post it again next time post-telemetries is run.
#

BASEDIR=$(dirname $(readlink -f $0))
. $BASEDIR/common-telemetry.sh

##
## runs a query to see if we're currently in use
##
function detectStreaming
{
     #Tap command over telnet to get product stae
    data="$((echo "getproductstate";sleep 5;echo "exit" 2>/dev/null) | telnet localhost 17000 2>/dev/null | tee  /dev/stderr 2>/dev/null)"
    if [ `echo $data | grep -c "STREAMING" ` -gt 0 ]
    then
        return 1 # true/success in shell-land
    else
        return 0
    fi
}

#detectStreaming && exit 0 # if we're streaming, postpone posting to the server

files=`ls $TELEMETRY_DOC_DIR/*.json 2>/dev/null`

# Data collection endpoint should be obtained by service discovery from HTTPProxy
productID=$(mfgdata get guid)
TELEMETRY_SERVICE_DISCOVERY=http://data-collector

: ${TELEMETRY_HOST:=$TELEMETRY_SERVICE_DISCOVERY}
: ${UNIQUE_ID:=$productID}

for report in $files
do

  result=$(curl -x localhost:8888 POST \
    -H 'Content-Type: application/json' \
    -H 'Accept: application/json' \
    -m 5 \
    --write-out %{http_code} \
    --output /dev/null \
    --data-binary @$report \
    ${TELEMETRY_HOST}/data/telemetry/${UNIQUE_ID} 2>/dev/null)

    # A 503 might be returned by HTTP proxy which gets added to the 200 code after the retry
    if [ $result -eq 200 ] || [ $result -eq 503200 ]; then
	   rm $report
    fi
    # if we've failed we'll come around and try it again

done
