/*
* @file main.c 
* @brief Main application entry and task loop 
* 
* @details 
* This project implements a data acquisition and storage system based on 
* STM32F103. ADC sampling is triggered periodically by TIM3 and transferred 
* to memory using DMA. The sampled data can be stored in W25Q64 Flash and 
* queried through the serial interface. 
* 
* The main loop handles DMA completion, serial command processing, and 
* key input. PWM is used to control the LED brightness according to the 
* acquired ADC data. */

#include "stm32f10x.h"                  // Device header
#include "MyDMA.h"
#include "OLED.h"
#include "DataStorage.h"
#include "Command.h"
#include "Serial.h"
#include "AD.h" 
#include "W25Q64.h"
#include "EnLED.h"
#include "Key.h"

int main(void)
{
	uint8_t Byte;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	AD_Init();
	MyDMA_Init();
	W25Q64_Init();
	
	OLED_Init();
	DataStorage_Init();
	Command_Init();
	EnLED_Init();
	Key_Init();
	
	while(1)
	{
		// Process completed ADC data transferred by DMA
		if(MyDMA_GetFlag() == 1)
		{
			OLED_ShowString(1,1,"Data Grouph: ");
			OLED_ShowNum(2,1, DataStorage_GetDataCount(), 6);
			DataStorage_Save();
			if(EnLED_GetState() == 1)
			{
				EnLED_SetDuty(MyDMA_GetData()*100/4095);
				OLED_ShowString(4,1,"Duty: ");
				OLED_ShowNum(4,7, MyDMA_GetData()*100/4095, 3);
			}
			OLED_ShowString(3,1,"LED State: ");
			OLED_ShowNum(3,12, EnLED_GetState(), 1);
		}
		// Process received serial data and parse commands
		if(Read_Buffer(&Byte))
		{
			Command_InputByte(Byte);
		}
		// Check the key state and toggle the LED
		if(Key_GetState()==1)
		{
			EnLED_Set();
		}
	}
}
