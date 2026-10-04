// W25Q64 SPI Flash driver. 
// The ADC uses 12-bit resolution, so ADC samples are stored as 16-bit values 
// to simplify data storage and retrieval.

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
#define W25Q64_PAGE_SIZE 256

void W25Q64_Init(void)
{
	MySPI_Init();
}

// Enable write operations before programming or erasing Flash.

void W25Q64_Write_ENABLE(void)
{
	MySPI_Start();
	MySPI_Swap(Write_Enable);
	MySPI_Stop();
}

// Wait until the Flash finishes its internal program or erase operation. 
// The BUSY bit (bit 0) of the Status Register remains set while 
// the Flash is performing an internal operation.

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

// Read the JEDEC ID of the W25Q64.
// MID: Manufacturer ID 
// DID: Device ID

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


// Read a sequence of bytes from Flash. 
// Address: Starting Flash address 
// Array: Destination buffer 
// count: Number of bytes to read

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

// Write a sequence of bytes to Flash using Page Program.  
// Note: The caller must ensure that the data does not cross 
// a page boundary when using this function directly.

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

// Write 16-bit data to Flash. // // The function automatically handles page boundaries because // the W25Q64 Page Program command cannot write across a page boundary.
// Address: Starting Flash address in bytes 
// Array: Source buffer containing 16-bit data 
// count: Number of 16-bit values to write

void W25Q64_WriteData_16b(uint32_t Address, uint16_t *Array, uint32_t count)
{
	uint32_t RemainingBytes =count *sizeof(uint16_t);
	uint32_t DataIndex = 0;
	
	while(RemainingBytes > 0)
	{
		uint32_t PageRemain = W25Q64_PAGE_SIZE - (Address % W25Q64_PAGE_SIZE);
		uint32_t WriteBytes;
        if(RemainingBytes < PageRemain)
        {
            WriteBytes = RemainingBytes;
        }
        else
        {
            WriteBytes = PageRemain;
        }
		W25Q64_Write_ENABLE();
		MySPI_Start();
		
		MySPI_Swap(Page_Program);
		MySPI_Swap(Address>>16);
		MySPI_Swap(Address>>8);
		MySPI_Swap(Address);
		
		for(uint16_t i = 0; i < WriteBytes/2; i ++)
		{
			MySPI_Swap(Array[DataIndex + i] >> 8);
            MySPI_Swap(Array[DataIndex + i]);
		}
		MySPI_Stop();
		W25Q64_Wait_Buzy();
		
		// 更新剩余数据
        Address += WriteBytes;
        RemainingBytes -= WriteBytes;
        DataIndex += WriteBytes / 2;
	}
	
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
