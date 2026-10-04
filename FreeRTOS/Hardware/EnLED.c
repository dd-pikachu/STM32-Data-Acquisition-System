// Configure TIM2 OC2 to output a PWM signal to control LED brightness

#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Key.h"


volatile uint8_t LED_State ;

void EnLED_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision =TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 720-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 1000-1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit (TIM2, &TIM_TimeBaseInitStruct);
	
	
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC2Init(TIM2, &TIM_OCInitStruct);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	TIM_Cmd(TIM2, ENABLE);
	LED_State = 0;
}


void EnLED_SetDuty(uint8_t Duty){

	uint16_t ccr=0;
	if(Duty>100)
	{
		Duty=100;
	}
	ccr = (uint32_t)Duty * 720 / 100;
	TIM_SetCompare2(TIM2, ccr);
}

void EnLED_Set(void)
{
	if(LED_State == 1)
	{
		EnLED_SetDuty(0);
		LED_State = 0 ;
		OLED_ShowString(4,1,"          ");
	}
	else
	{
		LED_State = 1;
        
	}
}

uint8_t EnLED_GetState(void)
{
	return LED_State;
}

void vKeyScanTask(void *pvParameters)
{
    while(1)
    {
        Key_GetState();

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void vKeyEnLEDTask(void *pvParameters)
{
    while(1)
    {
        xSemaphoreTake(xKeySemphr, portMAX_DELAY);

        EnLED_Set();
    }
}
