// Implement SPI communication using software GPIO control for communication with the W25Q64
// SPI Mode 0: CPOL = 0, SCK remains low when idle;
// CPHA = 0, data is sampled on the rising edge and changed on the falling edge.
#include "stm32f10x.h"                  // Device header

void Set_SS(uint8_t Binary)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction) Binary);
}

void Set_SCK(uint8_t Binary)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_5, (BitAction) Binary);
}

uint8_t Read_MISO(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
}

void Set_MOSI(uint8_t Binary)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_7, (BitAction) Binary);
}

void MySPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	Set_SS(1);
	Set_SCK(0);
}

void MySPI_Start(void)
{
	Set_SS(0);
}

void MySPI_Stop(void)
{
	Set_SS(1);
}

uint8_t MySPI_Swap(uint8_t SendData)
{
	uint8_t ReceiveData = 0;
	for(uint8_t i = 0; i < 8; i++)
	{
		Set_MOSI(SendData & (0x80>>i));
		Set_SCK(1);
		if(Read_MISO() == SET)
		{
			ReceiveData |= (0x80>>i);
		}
		Set_SCK(0);
	}
	return ReceiveData;
}
