#ifndef __CAL_H
#define __CAL_H 

#include "stm32f4xx_hal.h"
#include "arm_math.h"

//declare functions
float vpp2dbm(float vpp);
float dbm2vpp(float dbm);
uint16_t Vpp2AmpCtrl(float vpp);
uint16_t Vrms2AmpCtrl(float vrms);
uint16_t dbm2AmpCtrl(float dbm);


#endif