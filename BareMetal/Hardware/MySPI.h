#ifndef __MySPI_H
#define __MySPI_H

void MySPI_Init(void);
void MySPI_Start(void);
void MySPI_Stop(void);
uint8_t MySPI_Swap(uint8_t SendData);

#endif
