#include "stm32f10x.h"
#include "tim3.h"
#include "cabinet.h"

extern volatile u8  ENERGY_STATE;  

// psc [15; 0] ����ƵֵΪ2��16�η�-1
void TIM3_Init(u32 arr,	u32 psc)
{
	
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
	
		//��ʱ��TIM3��ʼ��
		TIM_TimeBaseStructure.TIM_Period = arr; 
		TIM_TimeBaseStructure.TIM_Prescaler =psc; 
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	 
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 


		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
		NVIC_Init(&NVIC_InitStructure);  


		TIM_Cmd(TIM3, DISABLE);  	
}


void TIM3_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{			
		ENERGY_STATE=1;
		TIM_ClearFlag(TIM3,TIM_IT_Update);
	}
	
}


