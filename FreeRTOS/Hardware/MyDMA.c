// Automatically transfer ADC data using DMA
// Use a buffer to temporarily store sampled data, with 64 samples per group
// An interrupt is triggered when one group of data transfer is completed

#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "semphr.h"

SemaphoreHandle_t xSaveSemphr;
// Device header

#define DMADataSIze 64
uint16_t Data[DMADataSIze];

// Get the address of the DMA data buffer
uint16_t *MyDMA_GetDataBuffer(void)
{
    return Data;
}


// Get the number of samples in each data group
uint8_t MyDMA_GetDataLength(void)
{
    return DMADataSIze;
}


// Restart the DMA transfer
void MyDMA_Restart(void)
{
    DMA_Cmd(DMA1_Channel1, DISABLE);
    DMA_SetCurrDataCounter(DMA1_Channel1, DMADataSIze);
    DMA_Cmd(DMA1_Channel1, ENABLE);
}

void MyDMA_Init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	xSaveSemphr = xSemaphoreCreateBinary();
	
	DMA_InitTypeDef DMA_InitStruct;
	DMA_InitStruct.DMA_BufferSize = DMADataSIze;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)Data;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel1, &DMA_InitStruct);
	
	DMA_Cmd(DMA1_Channel1,ENABLE);
	
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	
	NVIC_Init(&NVIC_InitStruct);
	
}

void DMA1_Channel1_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if(DMA_GetITStatus(DMA1_IT_TC1) == SET)
	{
		xSemaphoreGiveFromISR(xSaveSemphr, &xHigherPriorityTaskWoken);
		DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


// Calculate the average value of one completed DMA data group
uint16_t MyDMA_GetData(void)
{
	uint8_t i;
	uint32_t sum = 0;
	for(i = 0; i < DMADataSIze; i++)
	{
		sum += Data[i];
	}
	sum = sum / DMADataSIze;
	return sum;
}
