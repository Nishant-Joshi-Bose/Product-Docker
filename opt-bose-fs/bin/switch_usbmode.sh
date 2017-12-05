#!/bin/bash
# Utility for switching usb mode between networking( p2p or client ) and adb.

export PATH=/opt/Bose/bin:/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin

log () {
    local pri="$1"; shift

    # warning does to stderr, too.  info does not.
    local s=-s; [ "$pri" = info ] && s=

    logger $s -t "$tag" -p "$pri" "$*"
}

if (( $# > 1 )); then
    echo >&2 "$0: unexpected argument '$2'"
    echo >&2 "Usage: $0 [p2p/client]"
    exit 1
fi

function do_p2p {
    log info "Switching to USB p2p mode"
    killall dnsmasq 2> /dev/null
    /sbin/usb/compositions/9057
    # wait for composition script to execute
    sleep 3 
    ip link set rndis0 down
    ip link set rndis0 name usb2
    ip link set usb2 up
    ip addr add 203.0.113.1/24 dev usb2
    dnsmasq -i usb2 --no-daemon --no-resolv --no-poll --dhcp-range=203.0.113.2,203.0.113.5,12h &
}

function do_client {
    log info "Switching to USB client mode"
    killall dnsmasq 2> /dev/null
    /sbin/usb/compositions/9057
    # wait for composition script to execute
    sleep 3 
}

function do_adb {
    log info "Switching to adb mode"
    killall dnsmasq 2> /dev/null
    /sbin/usb/compositions/901D
    # wait for composition script to execute
    sleep 3 
}

op=$1; shift

case "$op" in
    (p2p)
        do_p2p
        ;;
    (client)
        do_client
        ;;
    (*)
        if ! is-development; then
            do_client
        else
            do_adb
        fi
        ;;
esac
