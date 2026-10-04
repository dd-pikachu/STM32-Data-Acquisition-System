// Key detection

#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "semphr.h"

SemaphoreHandle_t xKeySemphr;

void Key_Init(void)
{
	xKeySemphr = xSemaphoreCreateBinary();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct .GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

}

void Key_GetState(void)
{
	static uint8_t previous = 1;
	uint8_t current;
	current = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);
	if(current == Bit_RESET && previous != current)
	{
		xSemaphoreGive(xKeySemphr);
	}
	previous = current;
}

