/**
  ******************************************************************************
  * File Name          : TIM.c
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */
void delay_ms(int32_t nms)
{
 int32_t temp;
 SysTick->LOAD = 8000*nms;
 SysTick->VAL=0X00;
 SysTick->CTRL=0X01;
  do
  {
       temp=SysTick->CTRL;
  }
     while((temp&0x01)&&(!(temp&(1<<16))));

     SysTick->CTRL=0x00;
     SysTick->VAL =0X00;
 }

/* USER CODE END 0 */

TIM_HandleTypeDef htim2;

/* TIM2 init function */
void MX_TIM2_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 79;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_TIM_MspPostInit(&htim2);

}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* tim_pwmHandle)
{

  if(tim_pwmHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
}
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(timHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspPostInit 0 */

  /* USER CODE END TIM2_MspPostInit 0 */
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM2 GPIO Configuration    
    PA1     ------> TIM2_CH2 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM2_MspPostInit 1 */

  /* USER CODE END TIM2_MspPostInit 1 */
  }

}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* tim_pwmHandle)
{

  if(tim_pwmHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
int tone[]={247,262,294,330,349,392,440,294,523,587,659,698,784,1000};//音调对应的频�?

int yindiao[]=             //音调
{
	5,5,6,8,7,6,5,6,13,13,
	5,5,6,8,7,6,5,3,13,13,
	2,2,3,5,3,5,6,3,2,1,
	6,6,5,6,5,3,6,5,13,13,
	5,5,6,8,7,6,5,6,13,13,
	5,5,6,8,7,6,5,3,13,13,
	2,2,3,5,3,5,6,3,2,1,
	6,6,5,6,5,3,6,1,
	13,8,9,10,10,9,8,10,9,8,6,
	13,6,8,9,9,8,6,9,8,6,5,
	13,2,3,5,5,3,5,5,6,8,7,6,
	6,10,9,9,8,6,5,6,8
};

int dtime[]=               //节拍
{
	2,4,2,2,2,2,2,8,4, 4,
	2,4,2,2,2,2,2,8,4, 4,
	2,4,2,4,2,2,4,2,2,8,
	2,4,2,2,2,2,2,8,4 ,4,
	2,4,2,2,2,2,2,8,4, 4,
	2,4,2,2,2,2,2,8,4, 4,
	2,4,2,4,2,2,4,2,2,8,
	2,4,2,2,2,2,2,8,
	4, 2,2,2, 4, 2,2,2, 2,2,8,
	4, 2,2,2, 4, 2,2,2, 2,2,8,
	4, 2,2,2,4,2,2,5,2,6,2,4,
	 2,2 ,2,4,2,4,2,2,12

};

void sound(int freq)
{
	if(freq!=1000)
	{
		int Arr;

		TIM_OC_InitTypeDef sConfigOC = {0};
		Arr = 500000/freq;
		htim2.Init.Period = Arr;//改变频率来改变计数器周期
		HAL_TIM_PWM_Init(&htim2);
		sConfigOC.Pulse = Arr/4;
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
		{
			Error_Handler();
		}
		HAL_TIM_MspPostInit(&htim2);

	}
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);  //使能PWM定时�?
}


void buzzer_quit()
{
	HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_2);//停止PWM定时�?
}

void play_mus()//播放音乐
{
	int i;
	for(i=0;i<sizeof(yindiao);i++)
	{
			sound(tone[yindiao[i]]);
			delay_ms(150*dtime[i]);
			buzzer_quit();
			delay_ms(20);
	}
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
