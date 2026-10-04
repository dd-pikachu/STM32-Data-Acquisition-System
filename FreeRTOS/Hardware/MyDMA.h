#ifndef __MyDMA_H
#define __MyDMA_H

#include "FreeRTOS.h"
#include "semphr.h"

void MyDMA_Init(void);
uint16_t MyDMA_GetData(void);
void MyDMA_Restart(void);
uint16_t *MyDMA_GetDataBuffer(void);
uint8_t MyDMA_GetDataLength(void);
extern SemaphoreHandle_t xSaveSemphr;

#endif
