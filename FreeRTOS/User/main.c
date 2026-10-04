#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "Task.h"
#include "AD.h"
#include "MyDMA.h"
#include "DataStorage.h"
#include "EnLED.h"
#include "Key.h"
#include "W25Q64.h"
#include "Serial.h"
#include "OLED.h"

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;

    taskDISABLE_INTERRUPTS();

    while (1)
    {
    }
}

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	AD_Init();
	OLED_Init();
	MyDMA_Init();
	W25Q64_Init();
	DataStorage_Init();
	Key_Init();
	EnLED_Init();
	Serial_Init();

	OLED_ShowString(1,1,"Data Grouph: ");
	xTaskCreate(DataSaveTask, "DataSaveTask", 256, NULL, 1, NULL);
	xTaskCreate(vKeyScanTask, "KeyScan", 128, NULL, 2, NULL);
	xTaskCreate(vKeyEnLEDTask,"EnLEDTask",128,NULL,1,NULL);
	xTaskCreate(xSerialTask, "xSerialTask", 128, NULL, 1, NULL);
	vTaskStartScheduler();
	while(1)
	{
	}
}
