#ifndef _UAV_PUMP_H
#define _UAV_PUMP_H

#include "main.h"
#include "tim.h"


#define UAV_PUMP_TIM            &htim3
#define UAV_PUMP_TIM_CH         TIM_CHANNEL_3
#define UAV_PUMP_TIM_CCR        TIM3->CCR3



void PumpDisconnectDetection(void);


#endif
