#include "Pot.h"
#include "AD.h"

void Pot_Init(void)
{
    AD_Init();
}

/* 电位器只保留原始 AD 值。 */
uint16_t Pot_GetData(void)
{
    return AD_GetPotRaw();
}
