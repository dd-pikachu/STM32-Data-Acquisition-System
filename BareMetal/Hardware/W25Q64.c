
// Since the ADC uses 12-bit resolution, 16-bit storage is used for ADC data.
#include "stm32f10x.h"                  // Device header
#include "MySPI.h"

#define Write_Enable 0x06
#define Read_Status_Register 0x05
#define Page_Program 0x02
#define Sector_Erase 0x20
#define JEDEC_ID 0x9F
#define Read_Data 0x03
#define empty 0xFF
#define Chip_Erase 0x60

void W25Q64_Init(void)
{
	MySPI_Init();
}

void W25Q64_Write_ENABLE(void)
{
	MySPI_Start();
	MySPI_Swap(Write_Enable);
	MySPI_Stop();
}

void W25Q64_Wait_Buzy(void)
{
	uint16_t number= 10000;
	MySPI_Start();
	while((MySPI_Swap(Read_Status_Register) &0x01) == 0x01)
	{
		number--;
		if(number == 0)
		{
			break;
		}
	}
	MySPI_Stop();
}

void W25Q64_ReadID(uint8_t *MID, uint16_t *DID)
{
	MySPI_Start();
	MySPI_Swap(JEDEC_ID);
	*MID = MySPI_Swap(empty);
	*DID = MySPI_Swap(empty);
	*DID <<= 8;
	*DID |= MySPI_Swap(empty);
	MySPI_Stop();
}

void W25Q64_ReadData(uint32_t Address, uint8_t *Array, uint32_t count)
{
	MySPI_Start();
	MySPI_Swap(Read_Data);
	MySPI_Swap(Address>>16);
	MySPI_Swap(Address>>8);
	MySPI_Swap(Address);
	for(uint16_t i = 0; i < count; i ++)
	{
		Array[i] = MySPI_Swap(empty);
	}
	MySPI_Stop();
}

void W25Q64_WriteData(uint32_t Address, uint8_t *Array, uint32_t count)
{
	W25Q64_Write_ENABLE();
	MySPI_Start();
	
	MySPI_Swap(Page_Program);
	MySPI_Swap(Address>>16);
	MySPI_Swap(Address>>8);
	MySPI_Swap(Address);
	
	for(uint16_t i = 0; i < count; i ++)
	{
		MySPI_Swap(Array[i]);
	}
	MySPI_Stop();
	W25Q64_Wait_Buzy();
}

void W25Q64_SectorErase(uint32_t Address)
{
	W25Q64_Write_ENABLE();
	MySPI_Start();
	MySPI_Swap(Sector_Erase);
	MySPI_Swap(Address>>16);
	MySPI_Swap(Address>>8);
	MySPI_Swap(Address);
	MySPI_Stop();
	W25Q64_Wait_Buzy();
}

void W25Q64_ChipErase(void)
{
	W25Q64_Write_ENABLE();
	MySPI_Start();
	MySPI_Swap(Chip_Erase);
	MySPI_Stop();
	W25Q64_Wait_Buzy();
}

void W25Q64_WriteData_16b(uint32_t Address, uint16_t *Array, uint32_t count)
{
	W25Q64_Write_ENABLE();
	MySPI_Start();
	
	MySPI_Swap(Page_Program);
	MySPI_Swap(Address>>16);
	MySPI_Swap(Address>>8);
	MySPI_Swap(Address);
	
	for(uint16_t i = 0; i < count; i ++)
	{
		MySPI_Swap(Array[i] >> 8);
		MySPI_Swap(Array[i]);
	}
	MySPI_Stop();
	W25Q64_Wait_Buzy();
}

void W25Q64_ReadData_16b(uint32_t Address, uint16_t *Array, uint32_t count)
{
	MySPI_Start();
	MySPI_Swap(Read_Data);
	MySPI_Swap(Address>>16);
	MySPI_Swap(Address>>8);
	MySPI_Swap(Address);
	for(uint16_t i = 0; i < count; i ++)
	{
		Array[i] = MySPI_Swap(empty)<< 8;
		Array[i] |= MySPI_Swap(empty);
	}
	MySPI_Stop();
}
