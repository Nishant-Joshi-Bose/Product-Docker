# -*- sh -*-

if ! [ "${PROFILE-}" ]; then
    source ~/.bash_profile
    return
fi

if [ "${PS1-}" ]; then # interactive shells
    alias l="less -M"
    alias g="grep -E"
    alias lf="ls -F"
    alias j=jobs
    alias d3="dmesg -n3" # reduce logging to console
    alias bs="tap-command product boot_status"
    function h { history "$@" | less; }
    function resize { eval $(command resize -u); }
    function ll { ls -la "$@" | less -ME; }
    function cls { tput clear "$@" && tput cup 9998 0; }
    function ps1 {
        local e=$? i=
        [ $e -eq 0 ] || echo -n "(exit=$e)"
        set -- $(jobs -pr) # running jobs
        case $# in
            (0) : ;;
            (1) i="${i}R" ;;
            (*) i="${i}R$#" ;;
        esac
        set -- $(jobs -ps) # stopped jobs
        case $# in
            (0) : ;;
            (1) i="${i}S" ;;
            (*) i="${i}S$#" ;;
        esac
        [ "$i" ] && echo -n "<$i>"
    }
    unitlabel=$(jq -r </persist/mfg_data.json 2>/dev/null \
      '"\(.productType)-\(.snMacEth [9:] | split(":") | join(""))"'
    )
    PS1='$(ps1)$unitlabel\$ '
    set -o notify
    bind "set show-all-if-ambiguous on"
    bind "set completion-ignore-case on"
fi

if [ -e /mnt/nv/bashrc ] ; then
    source /mnt/nv/bashrc
fi
