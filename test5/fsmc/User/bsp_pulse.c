/*
*********************************************************************************************************
*
*	模块名称 : 独立按键驱动模块
*	文件名称 : bsp_key.c
*	版    本 : V1.0
*	说    明 : 扫描独立按键，具有软件滤波机制，具有按键FIFO。可以检测如下事件：
*				(1) 按键按下
*				(2) 按键弹起
*				(3) 长按键
*				(4) 长按时自动连发
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2013-02-01 armfly  正式发布
*		V1.1    2013-06-29 armfly  增加1个读指针，用于bsp_Idle() 函数读取系统控制组合键（截屏）
*								   增加 K1 K2 组合键 和 K2 K3 组合键，用于系统控制
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "bsp_pulse.h"
int32_t time1;
int32_t time;

/* STM32_V5 */
	/*
		安富莱STM32-V5 按键口线分配：
			K1 键      : PI8   (低电平表示按下)
			K2 键      : PC13  (低电平表示按下)
			K3 键      : PI11  (低电平表示按下)

	*/

	/* 按键口对应的RCC时钟 */


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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* 设为输入口 */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 无需上下拉电阻 */
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
