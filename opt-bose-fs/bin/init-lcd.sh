#!/bin/bash

cd "$(dirname "$0")" || exit

if (( $# > 0 )); then
    echo >&2 "${0##*/}: unexpected argument '$1'"
    exit 1
fi

FRONT_BACK_PORCH_MIN=b2,01,01,00,01,01
FRONT_BACK_PORCH_MAX=b2,FF,FF,00,FF,FF
FRONT_BACK_PORCH=${FRONT_BACK_PORCH_MAX}
LINE_ADDRESS_ORDER_TOP_BOTTOM=36,C0
LINE_ADDRESS_ORDER_BOTTOM_TOP=36,10
LINE_ADDRESS_ORDER=${LINE_ADDRESS_ORDER_TOP_BOTTOM}
ROTATION=180
ETH0_IP_ADDRESS=$(ifconfig eth0 | grep "inet addr" | cut -d ':' -f 2 | cut -d ' ' -f 1)
WLAN0_IP_ADDRESS=$(ifconfig wlan0 | grep "inet addr" | cut -d ':' -f 2 | cut -d ' ' -f 1)
IP_ADDRESS=${WLAN0_IP_ADDRESS}
EDDIE_VERSION=$(awk '{print $NF}' /opt/Bose/etc/FS_VERSION)

if ! [ "$IP_ADDRESS" ]; then
    IP_ADDRESS=${ETH0_IP_ADDRESS}
fi

if ! [ "$IP_ADDRESS" ]; then
    IP_ADDRESS="NO:NETWORK"
fi

TE_SPEED_MAX=c6,00 ## 119 Hz
TE_SPEED_MIN=c6,1f ## 38  Hz
TE_SPEED_45=c6,19
TE_SPEED_40=c6,1e
TE_SPEED=${TE_SPEED_40}

DEBUG=0      ## 7 full debug
TE_ON_OFF=1  ## 1->on, i->off

FB_FILE_SYS_DIR=/sys/devices/soc/7af6000.spi/spi_master/spi6/spi6.1/graphics/fb1

if [ ! -e /dev/fb2 ]; then
    ln -s /dev/fb1 /dev/fb2
fi

if [ ! -e /etc/directfbrc ]; then
    ln -s /opt/Bose/etc/directfbrc /etc/directfbrc
fi

if [ -e ${FB_FILE_SYS_DIR}/rotation ]; then
    echo ${ROTATION} > ${FB_FILE_SYS_DIR}/rotation
fi

echo "================================================ ${0##*/}"
echo "TE SPEED          : ${TE_SPEED}"
echo "TE ON/OFF         : ${TE_ON_OFF}"
echo "ROTATION          : ${ROTATION}"
echo "IP ADDRESS        : ${IP_ADDRESS}"
echo "EDDIE VERSION     : ${EDDIE_VERSION}"
#echo "DEBUG             : ${DEBUG}"
#echo "FRONT/BACK PORCH  : ${FRONT_BACK_PORCH}"
#echo "LINE ADDRESS ORDER: ${LINE_ADDRESS_ORDER}"
echo "================================================"

echo ${TE_ON_OFF}          > ${FB_FILE_SYS_DIR}/te
echo ${TE_SPEED}           > ${FB_FILE_SYS_DIR}/send_command
#echo ${DEBUG}              > ${FB_FILE_SYS_DIR}/debug
#echo ${FRONT_BACK_PORCH}   > ${FB_FILE_SYS_DIR}/send_command
#echo ${LINE_ADDRESS_ORDER} > ${FB_FILE_SYS_DIR}/send_command

if is-development; then
    echo "Development mode splash screen..."
    ./show_splash ./bose.jpg ${IP_ADDRESS} ${EDDIE_VERSION} ## will exit with -1 to avoid the screen to be blank by the directfb destructor
else
    echo "Not development mode so no splash screen"
fi
