#!/bin/bash
# Initialize the AudioPath hardware.

if (( $# > 0 )); then
    echo >&2 "$0: unexpected argument '$1'"
    exit 1
fi

# Hack to work around a systemd boot ordering dependency (CASTLE-2106).
i=0
while ! amixer cset name="SLIMBUS_6_RX Audio Mixer MultiMedia1" "1"; do
    if (( ( i += 1 ) == 10 )); then
        echo "$0: giving up"
        break
    fi
    echo "$0: amixer failed, will retry"
    sleep 1
done

amixer cset name="SLIMBUS_6_RX Audio Mixer MultiMedia1" "1"
amixer cset name="SLIM RX2 MUX" "AIF4_PB"
amixer cset name="SLIM RX3 MUX" "AIF4_PB"
amixer cset name="SLIM_6_RX Channels" "Two"
amixer cset name="RX INT1_2 MUX" "RX2"
amixer cset name="RX INT2_2 MUX" "RX3"
amixer cset name="RX INT1 DEM MUX" "CLSH_DSM_OUT"
amixer cset name="RX INT2 DEM MUX" "CLSH_DSM_OUT"
#disable low latency channel per Dominic Arcoraci's suggestion
#amixer cset name='PCM_Dev 12 Topology Playback' 'LL'

amixer cset name="MultiMedia1 Mixer SLIM_0_TX" "1"
amixer cset name="ADC2 Volume" "12"
amixer cset name="ADC3 Volume" "12"
amixer cset name="AIF1_CAP Mixer SLIM TX7" "1"
amixer cset name="AIF1_CAP Mixer SLIM TX8" "1"
amixer cset name="SLIM_0_TX Channels" "Two"
amixer cset name="SLIM TX7 MUX" "DEC7"
amixer cset name="SLIM TX8 MUX" "DEC8"
amixer cset name="ADC MUX7" "AMIC"
amixer cset name="ADC MUX8" "AMIC"
amixer cset name="AMIC MUX7" "ADC2"
amixer cset name="AMIC MUX8" "ADC3"

amixer cset name="SEC_TDM_RX_0 Audio Mixer MultiMedia1" 1
amixer cset name="SEC_TDM_RX_0 Channels" "Eight"
amixer cset name="MultiMedia1 Mixer SEC_TDM_TX_0" 1
amixer cset name="SEC_TDM_TX_0 Channels" "Eight"

amixer cset name="MultiMedia2 Mixer PRI_TDM_TX_0" "1"
amixer cset name="PRI_TDM_TX_0 Channels" "Eight"
amixer cset name="PRI_TDM_TX_0 SampleRate" "KHZ_16"
