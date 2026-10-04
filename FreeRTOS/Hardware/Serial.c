// Implement serial communication using software-defined protocol

#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "semphr.h"
#include "Command.h"

#define Max_RingBuffer_Size 64
uint8_t Ring_Buffer[Max_RingBuffer_Size];
volatile uint8_t f = 0;
volatile uint8_t r = 0;
SemaphoreHandle_t xSerialSemphr;

void Serial_Buffer(uint8_t data)
{
	if ((r + 1) % Max_RingBuffer_Size ==f)
	{
		return ;
	}
	Ring_Buffer [r] = data;
	r = (r + 1) % Max_RingBuffer_Size;
}

uint8_t Read_Buffer(uint8_t *data)
{
	if (f ==r)
	{
		return 0;
	}
	*data = Ring_Buffer [f];
	f = (f + 1) % Max_RingBuffer_Size;
	return 1;
}

void Serial_Init(void)
{
	xSerialSemphr = xSemaphoreCreateBinary();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP ;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU ;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStruct);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel =USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =5;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority =0;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART1, ENABLE);
}

void  Serial_SendByte(uint16_t Byte)
{
	USART_SendData(USART1, Byte);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void  Serial_SendString(char* String)
{
	uint8_t i=0;
	for (i=0; String[i] != '\0'; i++)
	{
		Serial_SendByte(String[i]);
	}
}

uint16_t Pow(uint8_t x,uint8_t y)
{
	uint16_t Sum=1;
	while(y--)
	{
		Sum *= x;
	}
	return Sum;
}

void  Serial_SendNum(uint32_t Num, uint8_t Length)
{
	uint8_t i=0;
	for (i=0; i<Length; i++)
	{
		Serial_SendByte((Num / Pow(10,(Length-1-i))) % 10 +'0');
	}
}

void USART1_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		Serial_Buffer(USART_ReceiveData(USART1));
		xSemaphoreGiveFromISR (xSerialSemphr, &xHigherPriorityTaskWoken);
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void xSerialTask(void * pvParameters)
{
	uint8_t data;
	while(1)
	{
		xSemaphoreTake(xSerialSemphr, portMAX_DELAY);
		while (Read_Buffer(&data))
        {
            Command_InputByte(data);
        }
	}
}
