# -*- sh -*-

export PROFILE=1
export PATH=/opt/Bose/bin:/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin
export HISTFILE=/mnt/nv/.bash_history

[ "${TERM-console}" = console ] && export TERM=ansi

# xterm-256color causes Python to output escape sequences.
# `python -c "" | wc -c` should print zero (wtf...)
[ "${TERM-}" = xterm-256color ] && export TERM=xterm

if [ "${PS1-}" ]; then # interactive shells
    # If the terminal size is unknown, run `resize` to detect it.
    if ! [ "$(stty -F /dev/tty size 2>/dev/null)" ]; then
        eval $(command resize -u)
    fi

    source ~/.bashrc

    date
    (
        name=$(xmllint --xpath '//SystemConfiguration/DeviceName/text()' \
               /mnt/nv/product-persistence/SystemConfigurationDB.xml)
        if [ "$name" ]; then
            echo "Device name: \"$name\""
        fi
        jq -r '"Product version: \(.long)"' /opt/Bose/etc/BoseVersion.json
    ) 2>/dev/null
    awk '$1 == "HSP" {$1="Riviera-HSP:"; print}' /etc/riviera-version

    echo Secure boot: \
        $(awk '$NF == "0" {print "disabled"}
               $NF == "1" {print "enabled"}' /sys/kernel/hsp/qfprom_secboot)

    validate-mfgdata
fi

if [ -e /mnt/nv/bash_profile ] ; then
    source /mnt/nv/bash_profile
fi

if [ -e /mnt/nv/run_logread ]; then
    if [ -s /mnt/nv/run_logread ]; then
        source /mnt/nv/run_logread
    else
        logread -f &
    fi
fi
