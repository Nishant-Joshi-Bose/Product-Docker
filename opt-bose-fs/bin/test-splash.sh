#!/bin/bash

FB_DEVICE=/dev/fb1
SLEEP_TIME=0.1

function splash_down()
{
    for ((i=1; i<=15; i++))
    do
        FILE_NAME=${i}.ppm.raw
        #fbsplash -s ${FILE_NAME} -d ${FB_DEVICE}
        cat ${FILE_NAME} > ${FB_DEVICE}
        sleep ${SLEEP_TIME}
    done
} ## splash down

function splash_up()
{
    for ((i=16; i>=2; i--))
    do
        FILE_NAME=${i}.ppm.raw
        #fbsplash -s ${FILE_NAME} -d ${FB_DEVICE}
        cat ${FILE_NAME} > ${FB_DEVICE}
        sleep ${SLEEP_TIME}
    done
} ## splash up

if [ -z "$1" ]; then
    SLEEP_TIME=0.033
else
    SLEEP_TIME=$1
fi

echo "=============================="
echo "Animating at: ${SLEEP_TIME}..."
echo "=============================="
while :
do
    splash_down
    splash_up
done

