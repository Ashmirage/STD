#ifndef __POT_H
#define	__POT_H
#include <stdint.h>


//void Pot_Init(void);
	
uint16_t Pot_GetData(void);

//得到电位器的模拟电压值
float Pot_GetVoltage(void);


#endif

