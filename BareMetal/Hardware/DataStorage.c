//统筹存储任务，DMA产生中断，主函数调用DataStorage_Save()来存储数据。
//同时保留了相关参数，用于读取数据

#include "stm32f10x.h"                  // Device header

#include "AD.h" 
#include "MyDMA.h"
#include "OLED.h"
#include "W25Q64.h"

static uint32_t FlashAddress = 0;

void DataStorage_Init(void)
{
	W25Q64_ChipErase();
}

void DataStorage_Save(void)
{
	if(FlashAddress >= 8 * 1024 * 1024)
			{
				OLED_ShowString(4,1,"FULL!!!");
				return;
			}
	W25Q64_WriteData_16b(FlashAddress, MyDMA_GetDataBuffer(), MyDMA_GetDataLength());
	FlashAddress += MyDMA_GetDataLength() * sizeof(uint16_t);
	MyDMA_Restart();
}

//获取已写入的地址
uint32_t DataStorage_GetCurrentAddress(void)
{
	return FlashAddress;
}

//获取写入的组数
uint32_t DataStorage_GetDataCount(void)
{
	return FlashAddress / (MyDMA_GetDataLength() * sizeof(uint16_t));
}

//读某一组某个值，每组64个Index(0~63);
void DataStorage_Read(uint32_t Group, uint32_t Index, uint16_t *Data)
{
	uint32_t Address=((Group-1) * MyDMA_GetDataLength() +  Index) * sizeof(uint16_t);
	W25Q64_ReadData_16b(Address, Data, 1);
}
