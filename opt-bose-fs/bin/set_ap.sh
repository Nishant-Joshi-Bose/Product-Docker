#!/bin/bash
# Initialize the AudioPath hardware.

if (( $# > 0 )); then
    echo >&2 "$0: unexpected argument '$1'"
    exit 1
fi

# Hack to work around a systemd boot ordering dependency (CASTLE-2106).
i=0
while ! amixer cset name="MultiMedia5 Mixer SLIM_0_TX" "1"; do         
    if (( ( i += 1 ) == 10 )); then
        echo "$0: giving up"
        break
    fi
    echo "$0: amixer failed, will retry"
    sleep 1
done

# APQ <- SLIMbus <- WDC CODEC (1 channel AIQ)
amixer cset name="MultiMedia5 Mixer SLIM_0_TX" "1"
amixer cset name="AIF1_CAP Mixer SLIM TX7" "1"    
amixer cset name="SLIM_0_TX Channels" "One"   
amixer cset name="SLIM TX7 MUX" "DEC7"        
amixer cset name="ADC MUX7" "AMIC"            
amixer cset name="AMIC MUX7" "ADC4"           
amixer cset name="ADC4 Volume" "20"        
                                              
# Hack to work around a systemd boot ordering dependency (CASTLE-2106).
# Make sure TDM dependencies are OK.          
i=0                                                          
while ! amixer cset name="SEC_TDM_RX_0 Audio Mixer MultiMedia1" "1"; do
        if (( ( i += 1 ) == 10 )); then                      
                echo "$0: giving up"                         
                break                                        
        fi                                                   
        echo "$0: amixer failed, will retry"                 
        sleep 1                                              
done                                                         
                                                             
                                                             
# APQ -> SEC_TDM -> SHARK (8 channels [2-audio, 4-announce, 2-spare])
amixer cset name="SEC_TDM_RX_0 Audio Mixer MultiMedia1" "1"  
amixer cset name="SEC_TDM_RX_0 Channels" "Eight"             
amixer cset name="PCM_Dev 1 Topology Playback" "LL"
                                                             
# APQ <- SEC_TDM <- SHARK (8 channels [2-HDMI, 3-AEC, 1-AEC ctrl])
amixer cset name="MultiMedia1 Mixer SEC_TDM_TX_0" "1"        
                                                           
amixer cset name="PCM_Dev 1 Topology Capture" "LL"

# MICS -> APQ (8 channels MIC data for Vfe)                  
#amixer cset name="MultiMedia2 Mixer SEC_TDM_TX_0" "1"    
#amixer cset name="SEC_TDM_TX_0 Channels" "Eight"       
#amixer cset name="SEC_TDM_TX_0 SampleRate" "KHZ_16" 
