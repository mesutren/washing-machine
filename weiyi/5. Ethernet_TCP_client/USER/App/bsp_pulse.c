
#include "./Bsp/systick/bsp.h"
#include "bsp_pulse.h"
#include "tcp_echoclient.h"
uint32_t time1;
uint16_t time;
uint32_t freqcal;
uint8_t freq ;

//gpio pf7


static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd =ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}



void EXTI_PF7_Config(void)
{
	
	EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;


	time = 0;
	time1 = 0;
	freq = 1 ;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
  //NVIC
  
  //?????
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* 设为输入口 */
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 无需上下拉电阻 */
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOF,&GPIO_InitStructure);
  
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource7);
	
  EXTI_InitStructure.EXTI_Line = EXTI_Line7;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
	NVIC_Configuration();
}

void EXTI9_5_IRQHandler(void)
{
		if(EXTI_GetITStatus(EXTI_Line7) != RESET) 
			{ 
				
				static int32_t time2 = 0;
				//EXTI->IMR&=~(1<<8); 
				time2 = bsp_GetRunTime();
				time = (time2 - time1) & 0xFFFF;
				time1 = time2;
				freqcal = 1000/time;
				freq =(uint8_t)(freqcal & 0xFF);
				tcp_pulse_send();
				EXTI_ClearITPendingBit(EXTI_Line7);
				//ENABLE_INT();
			}
}
