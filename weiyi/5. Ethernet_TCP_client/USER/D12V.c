#include "./Bsp/systick/bsp.h"
#include "D12V.h"

void D12Vconfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
/*?????? GPIO ??*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 ;
/*?????????????*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
/*??????? 50MHz */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
/*?????,??? GPIO*/
  GPIO_Init(GPIOC,&GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_8);
	GPIO_SetBits(GPIOC, GPIO_Pin_9);
}
