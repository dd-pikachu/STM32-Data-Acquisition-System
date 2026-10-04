#include "stm32f10x.h"
#include "OLED_Font.h"

/* Pin configuration */
#define OLED_W_SCL(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_8, (BitAction)(x))
#define OLED_W_SDA(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_9, (BitAction)(x))

/* Pin initialization */
void OLED_I2C_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  Generate an I2C start condition
  * @param  None
  * @retval None
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}

/**
  * @brief  Generate an I2C stop condition
  * @param  None
  * @retval None
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  Send one byte through I2C
  * @param  Byte Byte to be transmitted
  * @retval None
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(!!(Byte & (0x80 >> i)));
		OLED_W_SCL(1);
		OLED_W_SCL(0);
	}
	OLED_W_SCL(1);	// Additional clock cycle; ACK is not handled
	OLED_W_SCL(0);
}

/**
  * @brief  Write a command to the OLED
  * @param  Command Command to be written
  * @retval None
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		// Slave address
	OLED_I2C_SendByte(0x00);		// Control byte for command
	OLED_I2C_SendByte(Command); 
	OLED_I2C_Stop();
}

/**
  * @brief  Write data to the OLED
  * @param  Data Data to be written
  * @retval None
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		// Slave address
	OLED_I2C_SendByte(0x40);		// Control byte for data
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
  * @brief  Set the OLED cursor position
  * @param  Y Y-coordinate from the top-left corner, range: 0~7
  * @param  X X-coordinate from the top-left corner, range: 0~127
  * @retval None
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					// Set Y position
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	// Set the upper 4 bits of X
	OLED_WriteCommand(0x00 | (X & 0x0F));			// Set the lower 4 bits of X
}

/**
  * @brief  Clear the OLED display
  * @param  None
  * @retval None
  */
void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}

/**
  * @brief  Display a single character on the OLED
  * @param  Line Line position, range: 1~4
  * @param  Column Column position, range: 1~16
  * @param  Char Character to be displayed, range: printable ASCII characters
  * @retval None
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		// Set cursor to the upper half
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			// Display the upper half
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	// Set cursor to the lower half
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		// Display the lower half
	}
}

/**
  * @brief  Display a string on the OLED
  * @param  Line Starting line position, range: 1~4
  * @param  Column Starting column position, range: 1~16
  * @param  String String to be displayed, consisting of printable ASCII characters
  * @retval None
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  Calculate the power of a number
  * @retval Return value is X raised to the power of Y
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  Display an unsigned decimal number on the OLED
  * @param  Line Starting line position, range: 1~4
  * @param  Column Starting column position, range: 1~16
  * @param  Number Number to be displayed, range: 0~4294967295
  * @param  Length Number of digits to display, range: 1~10
  * @retval None
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  Display a signed decimal number on the OLED
  * @param  Line Starting line position, range: 1~4
  * @param  Column Starting column position, range: 1~16
  * @param  Number Number to be displayed, range: -2147483648~2147483647
  * @param  Length Number of digits to display, range: 1~10
  * @retval None
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  Display an unsigned hexadecimal number on the OLED
  * @param  Line Starting line position, range: 1~4
  * @param  Column Starting column position, range: 1~16
  * @param  Number Number to be displayed, range: 0~0xFFFFFFFF
  * @param  Length Number of digits to display, range: 1~8
  * @retval None
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  Display an unsigned binary number on the OLED
  * @param  Line Starting line position, range: 1~4
  * @param  Column Starting column position, range: 1~16
  * @param  Number Number to be displayed, range: 0~1111 1111 1111 1111
  * @param  Length Number of digits to display, range: 1~16
  * @retval None
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
  * @brief  Initialize the OLED
  * @param  None
  * @retval None
  */
void OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			// Power-on delay
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			// Initialize the I2C interface
	
	OLED_WriteCommand(0xAE);	// Turn off the display
	
	OLED_WriteCommand(0xD5);	// Set display clock divider and oscillator frequency
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	// Set multiplex ratio
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	// Set display offset
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	// Set display start line
	
	OLED_WriteCommand(0xA1);	// Set horizontal display direction: 0xA1 normal, 0xA0 reversed
	
	OLED_WriteCommand(0xC8);	// Set vertical display direction: 0xC8 normal, 0xC0 reversed

	OLED_WriteCommand(0xDA);	// Set COM pin hardware configuration
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	// Set contrast control
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	// Set pre-charge period
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	// Set VCOMH deselect level
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	// Enable or disable entire display

	OLED_WriteCommand(0xA6);	// Set normal or inverted display

	OLED_WriteCommand(0x8D);	// Set charge pump
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	// Turn on the display
		
	OLED_Clear();				// Clear the OLED display
}
