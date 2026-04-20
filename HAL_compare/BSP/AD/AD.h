#ifndef __AD_H
#define	__AD_H
#include <stdint.h>


extern volatile uint16_t AD_Value[4];
extern volatile uint16_t light_data;
void AD_init(void);



#endif 

