// Data storage management task. 
// The DMA completion interrupt releases the semaphore, and the storage task 
// stores the acquired ADC data into the W25Q64 Flash memory. 
// The module also maintains storage-related parameters for subsequent data retrieval.

#include "stm32f10x.h"                  // Device header
#include "AD.h" 
#include "MyDMA.h"
#include "OLED.h"
#include "W25Q64.h"
#include "EnLED.h"
#include "FreeRTOS.h"
#include "semphr.h"

static SemaphoreHandle_t xFlashMutex; //互斥信号量
static uint32_t FlashAddress = 0;

void DataStorage_Init(void)
{
	xFlashMutex = xSemaphoreCreateMutex();
	if(xFlashMutex == NULL)
    {
        // Mutex创建失败
        while(1);
    }
	W25Q64_ChipErase();
}

void DataStorage_Save(void)
{
	if(FlashAddress >= 8 * 1024 * 1024)
			{
				OLED_ShowString(4,1,"FULL!!!");
				return;
			}
	// Acquire exclusive access to the W25Q64 Flash memory.
	if(xSemaphoreTake(xFlashMutex, portMAX_DELAY) == pdTRUE)
    {
		W25Q64_WriteData_16b(FlashAddress, MyDMA_GetDataBuffer(), MyDMA_GetDataLength());
		
		// Advance the Flash address by the number of bytes written.
		FlashAddress += MyDMA_GetDataLength() * sizeof(uint16_t);
		// Release the Flash resource.
		xSemaphoreGive(xFlashMutex);
	}
	// Restart DMA for the next data acquisition cycle.
	MyDMA_Restart();
}

// Get the current Flash write address.
uint32_t DataStorage_GetCurrentAddress(void)
{
	return FlashAddress;
}


uint32_t DataStorage_GetDataCount(void)
{
	return FlashAddress / (MyDMA_GetDataLength() * sizeof(uint16_t));
}

// Read one data sample from a specified group. 
// Each group contains 64 samples, with Index ranging from 0 to 63.
void DataStorage_Read(uint32_t Group, uint32_t Index, uint16_t *Data)
{
	uint32_t Address=((Group-1) * MyDMA_GetDataLength() +  Index) * sizeof(uint16_t);
	if(xSemaphoreTake(xFlashMutex, portMAX_DELAY) == pdTRUE)
    {
        W25Q64_ReadData_16b(Address, Data, 1);

        xSemaphoreGive(xFlashMutex);
    }
}

void DataSaveTask(void * pvParameters)
{
	//OLED_ShowString(2,1,"Task Start");
	while(1)
	{
		xSemaphoreTake(xSaveSemphr, portMAX_DELAY);
		//OLED_ShowString(3,1,"DMA OK");
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
}
