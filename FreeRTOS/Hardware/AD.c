
// Configure the ADC to be triggered by the TIM3 TRGO event

#include "stm32f10x.h"                  // Device header

void AD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	// Configure PA0 as the ADC analog input
	GPIO_InitTypeDef GPIO_IninStruct;
	GPIO_IninStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_IninStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_IninStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_IninStruct);
	
	// Configure TIM3 to generate an update event every 1 ms and trigger the ADC through TRGO
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 100-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 720-1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
	
	
	// Configure the ADC to be triggered by TIM3 TRGO every 1 ms
	ADC_InitTypeDef ADC_InitStruct;
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_NbrOfChannel = 1;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	ADC_Init(ADC1, &ADC_InitStruct);
	ADC_Cmd(ADC1, ENABLE);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1,ADC_SampleTime_55Cycles5);
	
	// Calibrate the ADC
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1) != RESET) ;
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1) != RESET) ;
	
	ADC_DMACmd(ADC1, ENABLE);
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	
}


// Wait for the ADC conversion to complete and read the conversion result
uint16_t ADC_GetValue(void)
{
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    return ADC_GetConversionValue(ADC1);
}


