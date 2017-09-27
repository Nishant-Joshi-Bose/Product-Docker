# -*- sh -*-

if ! [ "${PROFILE-}" ]; then
    source ~/.bash_profile
    return
fi

if [ "${PS1-}" ]; then # interactive shells
    alias info="curl -s localhost:8090/info | xmllint --format -"
    alias l="less -M"
    alias g="grep -E"
    alias lf="ls -F"
    alias j=jobs
    alias d3="dmesg -n3" # reduce logging to console
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
    PS1='$(ps1)\$ '
    set -o notify
    bind "set show-all-if-ambiguous on"
    bind "set completion-ignore-case on"
fi

if [ -e /mnt/nv/bashrc ] ; then
    source /mnt/nv/bashrc
fi
