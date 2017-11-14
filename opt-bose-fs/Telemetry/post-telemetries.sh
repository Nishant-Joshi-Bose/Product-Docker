#!/bin/bash

BASEDIR=$(dirname $(readlink -f $0))
. $BASEDIR/common-telemetry.sh

##
## runs a query to see if we're currently in use
##
function detectStreaming {
 local re="Current State: *'([^']+)'"

 data=$(curl http://localhost:8090/pdo -d system 2>/dev/null)
 [[  $data =~ $re ]]
 if [[ ${BASH_REMATCH[1]} == 'Standby' ]]
 then
     return 1 # true/success in shell-land
 else
     return 0
 fi
}

files=`ls $TELEMETRY_DOC_DIR/*.json 2>/dev/null`

# Set defaults if not defined in environment variables
: ${TELEMETRY_HOST:=https://bose-test.apigee.net/svc-dataplatform-telemetrics/int-ip2/telemetrics}
: ${TELEMETRY_API_KEY:=12345XXYYZZ}

for report in $files 
do

  result=$(curl -X POST \
    -H "x-telemetry-api-key: ${TELEMETRY_API_KEY}" \
    -H 'Content-Type: application/json' \
    -H 'Accept: application/json' \
    -m 5 \
    --write-out %{http_code} \
    --output /dev/null \
    --data-binary @$report \
    ${TELEMETRY_HOST}/telemetry/metrics 2>/dev/null)

    logger -t TELEMETRY "transferred $report with result $? $result"
    if [ $result -eq 200 ]; then
	   rm $report
    fi
    # if we've failed we'll come around and try it again

done
