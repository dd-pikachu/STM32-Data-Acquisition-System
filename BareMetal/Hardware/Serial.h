#ifndef __Serial_H
#define __Serial_H

void Serial_Init(void);
void Serial_SendString(char* String);
void Serial_SendNum(uint32_t Num, uint8_t Length);
uint8_t Read_Buffer(uint8_t *data);

#endif
