// Serial command definition and parsing 
// Parse commands and parameters, then execute the corresponding operations

#include "stm32f10x.h"                  // Device header
#include "Serial.h"
#include <string.h>
#include "W25Q64.h"
#include "DataStorage.h"
#include "MyDMA.h"
#include "EnLED.h"

#define COMMAND_BUFFER_SIZE 32
char Command_Buffer[COMMAND_BUFFER_SIZE];
uint8_t Command_index=0;

void Command_Init(void)
{
	Serial_Init();
}

// Parse a decimal string and convert it to an unsigned integer 
// Return 1 if the conversion succeeds, otherwise return 0
uint8_t Parse_Decimal(const char *String, uint32_t *Value)
{
    uint32_t Num = 0;

    if(*String == '\0')
    {
        return 0;
    }

    while(*String != '\0')
    {
        if(*String < '0' || *String > '9')
        {
            return 0;
        }

        Num = Num * 10 + (*String - '0');

        String++;
    }

    *Value = Num;

    return 1;
}

// Parse the received command and execute the corresponding operation
void Command_Parse(char *Command)
{
	char *Argv[16];
	uint8_t Argc=0;
	char *Token;
	
	// Split the command string into individual arguments
	Token = strtok(Command, " ");
	while(Token != NULL && Argc < 16)
	{
		Argv[Argc] = Token;
		Argc++;
		
		Token = strtok(NULL, " ");
	}
	
	if(  Argc == 3 && strcmp(Argv[0], "History")==0 && strcmp(Argv[1], "Index")==0 && strcmp(Argv[2], "Show")==0)
	{
		Serial_SendString("The size of each grouph is: ");
		Serial_SendNum(MyDMA_GetDataLength(), 2);
		Serial_SendString("\r\n");
		Serial_SendString("Now the grouph is: ");
		Serial_SendNum(DataStorage_GetDataCount(), 6);
		Serial_SendString("\r\n");
    }
	else if(  Argc == 4 && strcmp(Argv[0], "History")==0 && strcmp(Argv[1], "data")==0)
	{
		uint32_t Group=0;
		uint32_t Index=0;
		uint16_t data=0;
        if(Parse_Decimal(Argv[2], &Group) && Parse_Decimal(Argv[3], &Index))
		{
			Serial_SendString("The History Data is: ");
			DataStorage_Read(Group, Index, &data);
			Serial_SendNum(data, 2);
			Serial_SendString("\r\n");
		}
		else
		{
			Serial_SendString("ERROR!");
		}
    }
	else if(  Argc == 2 && strcmp(Argv[0], "LED")==0 && strcmp(Argv[1], "State")==0)
	{

		Serial_SendString("The LED State is: ");
		Serial_SendNum(EnLED_GetState(), 2);
		Serial_SendString("\r\n");
    }else if(  Argc == 2 && strcmp(Argv[0], "LED")==0 && strcmp(Argv[1], "ON")==0)
	{
		if(EnLED_GetState() == 1)
		{
			Serial_SendString("The LED Already Turn On! ");
		}
		else
		{
			EnLED_Set();
		}
    }
	else if(  Argc == 2 && strcmp(Argv[0], "LED")==0 && strcmp(Argv[1], "OFF")==0)
	{
		if(EnLED_GetState() == 0)
		{
			Serial_SendString("The LED Already Turn Off! ");
		}
		else
		{
			EnLED_Set();
		}
    }
	else if(strcmp(Command,"HELP") == 0)
	{
		Serial_SendString("Commands\r\n");
		Serial_SendString("LED ON\r\n");
		Serial_SendString("LED OFF \r\n");
		Serial_SendString("LED State\r\n");
		Serial_SendString("History data\r\n");
		Serial_SendString("History Index Show\r\n");
		Serial_SendString("HELP\r\n");
	}
	else
	{
		Serial_SendString("ERROR: UNKNOWN COMMAND\r\n");
	} 
}

// Receive one byte from the serial port and assemble a complete command
void Command_InputByte(uint8_t Byte)
{
	if(Byte=='\r' || Byte=='\n')
	{
		if(Command_index>0)
		{
			Command_Buffer[Command_index]='\0';
			Command_Parse(Command_Buffer);
			Command_index =0;
		}
		return;
	}
	// Store the received byte in the command buffer
	if(Command_index<COMMAND_BUFFER_SIZE-1)
	{
		Command_Buffer[Command_index] = (char) Byte;
		Command_index++;
	}
	else
	{
		Command_index=0;
	}
}
