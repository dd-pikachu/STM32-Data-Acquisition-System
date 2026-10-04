#ifndef __KEY_H
#define __KEY_H

#include "FreeRTOS.h"
#include "semphr.h"

void Key_Init(void);
void Key_GetState(void);
extern SemaphoreHandle_t xKeySemphr;

#endif 
