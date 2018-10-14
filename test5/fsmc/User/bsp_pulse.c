/*
*********************************************************************************************************
*
*	ģ������ : ������������ģ��
*	�ļ����� : bsp_key.c
*	��    �� : V1.0
*	˵    �� : ɨ�������������������˲����ƣ����а���FIFO�����Լ�������¼���
*				(1) ��������
*				(2) ��������
*				(3) ������
*				(4) ����ʱ�Զ�����
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-02-01 armfly  ��ʽ����
*		V1.1    2013-06-29 armfly  ����1����ָ�룬����bsp_Idle() ������ȡϵͳ������ϼ���������
*								   ���� K1 K2 ��ϼ� �� K2 K3 ��ϼ�������ϵͳ����
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "bsp_pulse.h"
int32_t time1;
int32_t time;

/* STM32_V5 */
	/*
		������STM32-V5 �������߷��䣺
			K1 ��      : PI8   (�͵�ƽ��ʾ����)
			K2 ��      : PC13  (�͵�ƽ��ʾ����)
			K3 ��      : PI11  (�͵�ƽ��ʾ����)

	*/

	/* �����ڶ�Ӧ��RCCʱ�� */


//	#define GPIO_PORT_K1    GPIOI
//	#define GPIO_PIN_K1	    GPIO_Pin_8


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



void EXTI_PI8_Config(void)
{
	
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;  

	time = 0;
	time1 = 0;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
  //NVIC
  
  //?????
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* �������������� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_Init(GPIOI,&GPIO_InitStructure);
  
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  //?????????
  //GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOI, EXTI_PinSource8);
	
  EXTI_InitStructure.EXTI_Line = EXTI_Line8;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
	NVIC_Configuration();
}

void EXTI9_5_IRQHandler(void)
{
		if(EXTI_GetITStatus(EXTI_Line8) != RESET) 
			{ 
				
				static int32_t time2 = 0;
				//EXTI->IMR&=~(1<<8); 
				time2 = bsp_GetRunTime();
				time = time2 - time1;
				time1 = time2;
				EXTI_ClearITPendingBit(EXTI_Line8);
				//ENABLE_INT();
			}
}
