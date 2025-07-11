#include "cal.h"
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

