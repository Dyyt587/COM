#ifndef TEST_H
#define TEST_H

#include "tim_pwm.h"
#include "tim.h"
class Test{
	int a;
	
	void tt(){
		tim_pwm_init(&pwm0,&htim12,TIM_CHANNEL_1,140000000);

	}
};


#endif
