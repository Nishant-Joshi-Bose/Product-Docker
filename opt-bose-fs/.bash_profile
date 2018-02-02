# -*- sh -*-

export PROFILE=1
export PATH=/opt/Bose/bin:/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin
export HISTFILE=/mnt/nv/.bash_history

[ "${TERM-console}" = console ] && export TERM=ansi

# xterm-256color causes Python to output escape sequences.
# `python -c "" | wc -c` should print zero (wtf...)
[ "${TERM-}" = xterm-256color ] && export TERM=xterm

if [ "${PS1-}" ]; then # interactive shells
    source ~/.bashrc
    date
    (
        name=$(xmllint --xpath '//SystemConfiguration/DeviceName/text()' \
               /mnt/nv/product-persistence/SystemConfigurationDB.xml)
        if [ "$name" ]; then
            echo "Device name: \"$name\""
        fi
        cat /opt/Bose/etc/FS_VERSION*
    ) 2>/dev/null

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
