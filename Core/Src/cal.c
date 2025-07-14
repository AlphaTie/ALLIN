#include "cal.h"
#include "main.h"
#include <stdio.h>
//This file contains the calculate functions

float vpp2dbm(float vpp)
{
    // Convert Vpp to dBm
    // Formula: dBm = 10 * log10((Vpp^2) / (50^2))
    // Assuming a 50-ohm system
    return 10 * log10((vpp * vpp) / (50 * 50));

}

float dbm2vpp(float dbm)
{
    // Convert dBm to Vpp
    // Formula: Vpp = sqrt((10^(dBm/10)) * (50^2))
    // Assuming a 50-ohm system
    return sqrt(pow(10, dbm / 10) * (50 * 50));
}

uint16_t Vpp2AmpCtrl(float vpp)
{
    // Convert Vpp to AD9959 amplitude control value
    // Assuming a 10-bit DAC, Vpp range is 0-250mVpp
    // Formula: AmpCtrl = Vpp * 1000 / 260
    uint16_t Ctrl = (uint16_t)(vpp * 1000 / 235) + 1;
    if(Ctrl > 1023) {
        Ctrl = 1023; // Max value for 10-bit DAC
    }   
    return Ctrl;
}

uint16_t Vrms2AmpCtrl(float vrms)
{
    // Convert Vrms to AD9959 amplitude control value
    // Assuming a 10-bit DAC, Vrms range is 0-250mVrms
    // Formula: AmpCtrl = Vrms * 1000 / 260
		printf("Vrms INPUT:%f\n",vrms);
    uint16_t Ctrl = (uint16_t)(vrms * 2.828427 * 1000 / 235 ) + 1 ;
    if(Ctrl > 1023 ) {
        Ctrl = 1023 ; // Max value for 10-bit DAC
    }   
		printf("Vrms OUTPUT:%f\n",Ctrl*235/2828.427);
    return Ctrl;
}


uint16_t dbm2AmpCtrl(float dbm)
{
    return Vpp2AmpCtrl(dbm2vpp(dbm));
}

uint16_t Voffset2DacCtrl(float vpp)
{
    // Convert Vpp to AD9959 DAC output value
    // Assuming a 12-bit DAC, Vpp range is 0-3300mVpp
    // Formula: DacCtrl = Vpp * 4096 / 3300
    uint16_t Ctrl = (uint16_t)(vpp * 4096 / 3300);
    if(Ctrl > 4095) {
        Ctrl = 4095; // Max value for 12-bit DAC
    }   
    return Ctrl;
   
}

