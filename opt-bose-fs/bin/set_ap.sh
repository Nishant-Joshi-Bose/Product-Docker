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

# AIQ -> APQ (2 channels)
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

amixer cset name="SLIM RX2 MUX" "AIF4_PB"


# APQ -> Shark  Q (8 channels [2-audio, 4-announce, 2-spare])
amixer cset name="SEC_TDM_RX_0 Audio Mixer MultiMedia1" 1
amixer cset name="SEC_TDM_RX_0 Channels" "Eight"

# Shark -> APQ (8 channels [2-HDMI, 3-AEC, 1-AEC ctrl])
amixer cset name="MultiMedia1 Mixer SEC_TDM_TX_0" 1

# MICS -> APQ (8 channels MIC data for Vfe)
amixer cset name="MultiMedia2 Mixer SEC_TDM_TX_0" "1"
amixer cset name="SEC_TDM_TX_0 Channels" "Eight"
amixer cset name="SEC_TDM_TX_0 SampleRate" "KHZ_16"
