#ifndef __LED_DRIVER_H
#define __LED_DRIVER_H
#include "stm32f4xx.h"


#define LED_ON      GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define LED_OFF     GPIO_SetBits(GPIOA,GPIO_Pin_1)
#define LED(x)      ((x==0) ? LED_OFF : LED_ON)

extern void LED_PortInit(void);


#endif
