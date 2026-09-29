#ifndef __DATASTORAGE_H
#define __DATASTORAGE_H

#include "stm32f10x.h"

void DataStorage_Init(void);
void DataStorage_Save(void);
uint32_t DataStorage_GetCurrentAddress(void);
uint32_t DataStorage_GetDataCount(void);
void DataStorage_Read(uint32_t Group, uint32_t Index, uint16_t *Data);

#endif
