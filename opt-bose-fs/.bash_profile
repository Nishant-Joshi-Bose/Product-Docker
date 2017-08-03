# -*- sh -*-

export PROFILE=1
export PATH=/opt/Bose/bin:/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin
export HISTFILE=/mnt/nv/.bash_history

[ "${TERM-console}" = console ] && export TERM=ansi

if [ "${PS1-}" ]; then # interactive shells
    source ~/.bashrc
    date
    (
        name=$(xmllint --xpath '//SystemConfiguration/DeviceName/text()' \
               /mnt/nv/BoseApp-Persistence/1/SystemConfigurationDB.xml)
        if [ "$name" ]; then
            echo "Device name: \"$name\""
        fi
        cat /opt/Bose/etc/FS_VERSION*
    ) 2>/dev/null
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
