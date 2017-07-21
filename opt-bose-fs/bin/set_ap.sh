#!/bin/bash
# Initialize the AudioPath hardware.

if (( $# > 0 )); then
    echo >&2 "$0: unexpected argument '$1'"
    exit 1
fi

amixer cset name="SLIMBUS_6_RX Audio Mixer MultiMedia1" "1"
amixer cset name="SLIM RX2 MUX" "AIF4_PB"
amixer cset name="SLIM RX3 MUX" "AIF4_PB"
amixer cset name="SLIM_6_RX Channels" "Two"
amixer cset name="RX INT1_2 MUX" "RX2"
amixer cset name="RX INT2_2 MUX" "RX3"
amixer cset name="RX INT1 DEM MUX" "CLSH_DSM_OUT"
amixer cset name="RX INT2 DEM MUX" "CLSH_DSM_OUT"

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
