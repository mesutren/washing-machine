/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块。
*	文件名称 : main.c
*	版    本 : V1.2
*	说    明 : 主程序
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2013-02-01 armfly  首发
*		v1.0    2013-02-01 armfly  升级BSP,增加操作提示
*		V1.2    2014-02-28 armfly  升级固件库到V1.3.0
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"		/* 如果要用ST的固件库，必须包含这个文件 */
#include "app_tcpnet_lib.h"
#include "bsp_pulse.h"
/* 定义例程名和例程发布日期 */
#define EXAMPLE_NAME	"V5-120_AD7606（8通道16位同步ADC）例程"
#define EXAMPLE_DATE	"2014-02-28"
#define DEMO_VER		"1.2"

/* 仅允许本文件内调用的函数声明 */
static void PrintfLogo(void);
static void DispMenu(void);
extern int32_t time;
/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参：无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
int main(void)
{
	uint8_t ucKeyCode;
	uint8_t ucRefresh = 0;
	int32_t freq = 0;

	/*
		由于ST固件库的启动文件已经执行了CPU系统时钟的初始化，所以不必再次重复配置系统时钟。
		启动文件配置了CPU主时钟频率、内部Flash访问速度和可选的外部SRAM FSMC初始化。

		系统时钟缺省配置为72MHz，如果需要更改，可以修改：
		\Libraries\CMSIS\CM3\DeviceSupport\ST\STM32F10x\system_stm32f10x.c
		中配置系统时钟的宏。
	*/

	bsp_Init();

/* AD7606进入普通工作模式 */

	bsp_InitAD7606();	/* 配置AD7606所用的GPIO */

	EXTI_PI8_Config();

	bsp_StartAutoTimer(1, 1000);	/* 启动1个1000ms的自动重装的定时器 */
	AD7606_StartConvst();		/* 启动1次转换 */
	ucRefresh = 0;
	while (1)
	{
		bsp_Idle();		/* 空闲时执行的函数,比如喂狗. 在bsp.c中 */

		if (ucRefresh == 1)
		{
			ucRefresh = 0;
			 freq = 1000/time;
			/* 打印ADC采样结果 */
			printf("Range = %d, OS = %d, ", g_tAD7606.ucRange, g_tAD7606.ucOS);
			printf("CH1 = %6d, CH2 = %6d, CH3 = %6d, CH4 = %6d, ",
				g_tAD7606.sNowAdc[0], g_tAD7606.sNowAdc[1], g_tAD7606.sNowAdc[2], g_tAD7606.sNowAdc[3]);
			printf("CH5 = %6d, CH6 = %6d, CH7 = %6d, CH8 = %6d\r\n",
				g_tAD7606.sNowAdc[4], g_tAD7606.sNowAdc[5], g_tAD7606.sNowAdc[6], g_tAD7606.sNowAdc[7]);



			
			
/*			printf("CH1high = %d, CH1low = %d, CH2high = %d, CH2low = %d, ",
				g_tAD7606.tcpadc[0], g_tAD7606.tcpadc[1], g_tAD7606.tcpadc[2], g_tAD7606.tcpadc[3]);
			printf("CH3high = %d, CH3low = %d, CH4high = %d, CH4low = %d,\r\n",
				g_tAD7606.tcpadc[4], g_tAD7606.tcpadc[5], g_tAD7606.tcpadc[6], g_tAD7606.tcpadc[7]);
			printf("CH5high = %d, CH5low = %d, CH6high = %d, CH6low = %d, ",
				g_tAD7606.tcpadc[8], g_tAD7606.tcpadc[9], g_tAD7606.tcpadc[10], g_tAD7606.tcpadc[11]);
			printf("CH7high = %d, CH7low = %d, CH8high = %d, CH8low = %d\r\n ",
				g_tAD7606.tcpadc[12], g_tAD7606.tcpadc[13], g_tAD7606.tcpadc[14], g_tAD7606.tcpadc[15]);*/
			
			printf("freq= %d\r\n", freq);
			
		}
		
			if (bsp_CheckTimer(1))
			{
				/* 每隔500ms 进来一次. 由软件启动转换 */
				AD7606_ReadNowAdc();		/* 读取采样结果 */
				AD7606_StartConvst();		/* 启动下次转换 */

				ucRefresh = 1;	/* 刷新显示 */
			}
		}
}


/*
*********************************************************************************************************
*	函 数 名: DispMenu
*	功能说明: 显示操作提示菜单
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispMenu(void)
{
	printf("\r\n请操作开发板上的按键和摇杆:\r\n");
	printf("K1键       : 切换量程(5V或10V)\r\n");
	printf("K2键       : 进入FIFO工作模式\r\n");
	printf("K3键       : 进入软件定时采集模式\r\n");
	printf("摇杆上下键 : 调节过采样参数\r\n");
	printf("\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: PrintfLogo
*	功能说明: 打印例程名称和例程发布日期, 接上串口线后，打开PC机的超级终端软件可以观察结果
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	printf("*************************************************************\n\r");
	printf("* 例程名称   : %s\r\n", EXAMPLE_NAME);	/* 打印例程名称 */
	printf("* 例程版本   : %s\r\n", DEMO_VER);		/* 打印例程版本 */
	printf("* 发布日期   : %s\r\n", EXAMPLE_DATE);	/* 打印例程日期 */

	/* 打印ST固件库版本，这3个定义宏在stm32f10x.h文件中 */
	printf("* 固件库版本 : V%d.%d.%d (STM32F4xx_StdPeriph_Driver)\r\n", __STM32F4XX_STDPERIPH_VERSION_MAIN,
			__STM32F4XX_STDPERIPH_VERSION_SUB1,__STM32F4XX_STDPERIPH_VERSION_SUB2);
	printf("* \n\r");	/* 打印一行空格 */
	printf("* QQ    : 1295744630 \r\n");
	printf("* Email : armfly@qq.com \r\n");
	printf("* Copyright www.armfly.com 安富莱电子\r\n");
	printf("*************************************************************\n\r");
}
