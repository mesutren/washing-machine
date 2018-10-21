/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   TCP Client例程
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "stm32f4xx.h"
#include "bsp_ad7606.h"
#include "./Bsp/led/bsp_led.h" 
#include "./Bsp/usart/bsp_debug_usart.h"
#include "./Bsp/systick/bsp_timer.h"
#include "./Bsp/key/bsp_key.h"
#include "lwip/tcp.h"
#include "netconf.h"
#include "LAN8742A.h"
#include "tcp_echoclient.h"
#include "bsp_pulse.h"
#include "D12V.h"
#include "bsp_basic_tim.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern int32_t LocalTime;
extern __IO uint8_t EthLinkStatus;
extern uint8_t flag;

//__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	/* 初始化LED */
	//LED_GPIO_Config();
	
	
	/* 初始化调试串口，一般为串口1 */
	Debug_USART_Config();
	
	/* 初始化系统滴答定时器 */	
	bsp_InitTimer();
	
	bsp_InitAD7606();
	AD7606_Reset();
	
		
	D12Vconfig();

	
	printf("**********************************\r\n");	
	printf("LAN8720A Ethernet Demo\r\n");
	
	/* Configure ethernet (GPIOs, clocks, MAC, DMA) */
  ETH_BSP_Config();	
  printf("LAN8720A BSP INIT AND COMFIGURE SUCCESS\r\n");
	
  /* Initilaize the LwIP stack */
  LwIP_Init();	

  //IP地址和端口可在netconf.h文件修改，或者使用DHCP服务自动获取IP(需要路由器支持)
  printf("本地IP和端口: %d.%d.%d.%d\r\n",IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
  printf("远端IP和端口: %d.%d.%d.%d:%d\r\n",DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2, DEST_IP_ADDR3,DEST_PORT);
  bsp_StartAutoTimer(1, 1000);
	    if(flag==0)
		{
			if (EthLinkStatus == 0)
			{
				tcp_echoclient_connect();
        printf("connect to tcp server\r\n");
				flag = 1;
			}
		}
		
		EXTI_PF7_Config();
	TIMx_Configuration();

while(1)
	{

		
			if (bsp_CheckTimer(1))
			{
				/* 每1000ms 进来一次. 由软件启动转换 */
						AD7606_ReadNowAdc();		/* 读取采样结果 */
						AD7606_StartConvst();
						printf("%d\r\n",count);
						

				/* 启动下次转换 */
				//tcp_echoclient_disconnect();
				//tcp_echoclient_connect();
				ucRefresh = 1;	/* 刷新显示 */
							
				tcp_temprature_send();
			}
		if(ucRefresh == 1)
		{

					
			/* 打印ADC采样结果 */
			printf("Range = %d, OS = %d, ", g_tAD7606.ucRange, g_tAD7606.ucOS);
			printf("CH1 = %6d, CH2 = %6d, CH3 = %6d, CH4 = %6d, ",
				g_tAD7606.sNowAdc[0], g_tAD7606.sNowAdc[1], g_tAD7606.sNowAdc[2], g_tAD7606.sNowAdc[3]);
			printf("CH5 = %6d, CH6 = %6d, CH7 = %6d, CH8 = %6d\r\n",
				g_tAD7606.sNowAdc[4], g_tAD7606.sNowAdc[5], g_tAD7606.sNowAdc[6], g_tAD7606.sNowAdc[7]);

			
			
			printf("CH1high = %d, CH1low = %d, CH2high = %d, CH2low = %d, ",
				g_tAD7606.tcpadc[0], g_tAD7606.tcpadc[1], g_tAD7606.tcpadc[2], g_tAD7606.tcpadc[3]);
			printf("CH3high = %d, CH3low = %d, CH4high = %d, CH4low = %d,\r\n",
				g_tAD7606.tcpadc[4], g_tAD7606.tcpadc[5], g_tAD7606.tcpadc[6], g_tAD7606.tcpadc[7]);
			printf("CH5high = %d, CH5low = %d, CH6high = %d, CH6low = %d, ",
				g_tAD7606.tcpadc[8], g_tAD7606.tcpadc[9], g_tAD7606.tcpadc[10], g_tAD7606.tcpadc[11]);
			printf("CH7high = %d, CH7low = %d, CH8high = %d, CH8low = %d\r\n ",
				g_tAD7606.tcpadc[12], g_tAD7606.tcpadc[13], g_tAD7606.tcpadc[14], g_tAD7606.tcpadc[15]);
			
			printf("freq= %d\r\n", freq);
			//tcp_echoclient_disconnect();
			ucRefresh = 0;
			
		}
		/* check if any packet received */
    if (ETH_CheckFrameReceived())
    { 
      /* process received ethernet packet */
      LwIP_Pkt_Handle();
    }
    /* handle periodic timers for LwIP */
    LwIP_Periodic_Handle(LocalTime);
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
