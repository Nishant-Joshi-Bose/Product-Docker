#!/bin/bash
# Initialize the AudioPath hardware.

if (( $# > 0 )); then
    echo >&2 "$0: unexpected argument '$1'"
    exit 1
fi

function cset {
    amixer --quiet cset "$@"
}

# Hack to work around a systemd boot ordering dependency (CASTLE-2106).
i=0
while ! cset name="SLIMBUS_6_RX Audio Mixer MultiMedia1" "1"; do
    if (( ( i += 1 ) == 10 )); then
        echo "$0: giving up"
        break
    fi
    echo "$0: amixer failed, will retry"
    sleep 1
done

cset name="MultiMedia1 Mixer SLIM_0_TX" "1"
cset name="AIF1_CAP Mixer SLIM TX1" "1"
cset name="AIF1_CAP Mixer SLIM TX0" "1"
cset name="SLIM_0_TX Channels" "Two"
cset name="SLIM TX1 MUX" "DEC1"
cset name="SLIM TX0 MUX" "DEC0"
cset name="ADC MUX1" "AMIC"
cset name="ADC MUX0" "AMIC"
cset name="AMIC MUX1" "ADC3"
cset name="AMIC MUX0" "ADC2"

cset name="SLIM RX2 MUX" "AIF4_PB"
cset name="SLIM RX3 MUX" "AIF4_PB"
cset name="SLIM_6_RX Channels" "Two"
cset name="RX INT1_2 MUX" "RX2"
cset name="RX INT2_2 MUX" "RX3"
cset name="RX INT3_2 MUX" "RX2"
cset name="RX INT4_2 MUX" "RX3"
cset name="RX INT1 DEM MUX" "CLSH_DSM_OUT"
cset name="RX INT2 DEM MUX" "CLSH_DSM_OUT"
