#ifndef __W25Q64_H
#define __W25Q64_H

void W25Q64_Init(void);
void W25Q64_ReadID(uint8_t *MID, uint16_t *DID);
void W25Q64_ReadData(uint32_t Address, uint8_t *Array, uint32_t count);
void W25Q64_WriteData(uint32_t Address, uint8_t *Array, uint32_t count);
void W25Q64_SectorErase(uint32_t Address);
void W25Q64_WriteData_16b(uint32_t Address, uint16_t *Array, uint32_t count);
void MyDMA_Restart(void);
void W25Q64_ChipErase(void);
void W25Q64_ReadData_16b(uint32_t Address, uint16_t *Array, uint32_t count);

#endif 
