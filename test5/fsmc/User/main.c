/*
*********************************************************************************************************
*
*	ģ������ : ������ģ�顣
*	�ļ����� : main.c
*	��    �� : V1.2
*	˵    �� : ������
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2013-02-01 armfly  �׷�
*		v1.0    2013-02-01 armfly  ����BSP,���Ӳ�����ʾ
*		V1.2    2014-02-28 armfly  �����̼��⵽V1.3.0
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"		/* ���Ҫ��ST�Ĺ̼��⣬�����������ļ� */
#include "app_tcpnet_lib.h"
#include "bsp_pulse.h"
/* ���������������̷������� */
#define EXAMPLE_NAME	"V5-120_AD7606��8ͨ��16λͬ��ADC������"
#define EXAMPLE_DATE	"2014-02-28"
#define DEMO_VER		"1.2"

/* �������ļ��ڵ��õĺ������� */
static void PrintfLogo(void);
static void DispMenu(void);
extern int32_t time;
/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: c�������
*	��    �Σ���
*	�� �� ֵ: �������(���账��)
*********************************************************************************************************
*/
int main(void)
{
	uint8_t ucKeyCode;
	uint8_t ucRefresh = 0;
	int32_t freq = 0;

	/*
		����ST�̼���������ļ��Ѿ�ִ����CPUϵͳʱ�ӵĳ�ʼ�������Բ����ٴ��ظ�����ϵͳʱ�ӡ�
		�����ļ�������CPU��ʱ��Ƶ�ʡ��ڲ�Flash�����ٶȺͿ�ѡ���ⲿSRAM FSMC��ʼ����

		ϵͳʱ��ȱʡ����Ϊ72MHz�������Ҫ���ģ������޸ģ�
		\Libraries\CMSIS\CM3\DeviceSupport\ST\STM32F10x\system_stm32f10x.c
		������ϵͳʱ�ӵĺꡣ
	*/

	bsp_Init();

/* AD7606������ͨ����ģʽ */

	bsp_InitAD7606();	/* ����AD7606���õ�GPIO */

	EXTI_PI8_Config();

	bsp_StartAutoTimer(1, 1000);	/* ����1��1000ms���Զ���װ�Ķ�ʱ�� */
	AD7606_StartConvst();		/* ����1��ת�� */
	ucRefresh = 0;
	while (1)
	{
		bsp_Idle();		/* ����ʱִ�еĺ���,����ι��. ��bsp.c�� */

		if (ucRefresh == 1)
		{
			ucRefresh = 0;
			 freq = 1000/time;
			/* ��ӡADC������� */
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
				/* ÿ��500ms ����һ��. ���������ת�� */
				AD7606_ReadNowAdc();		/* ��ȡ������� */
				AD7606_StartConvst();		/* �����´�ת�� */

				ucRefresh = 1;	/* ˢ����ʾ */
			}
		}
}


/*
*********************************************************************************************************
*	�� �� ��: DispMenu
*	����˵��: ��ʾ������ʾ�˵�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispMenu(void)
{
	printf("\r\n������������ϵİ�����ҡ��:\r\n");
	printf("K1��       : �л�����(5V��10V)\r\n");
	printf("K2��       : ����FIFO����ģʽ\r\n");
	printf("K3��       : ���������ʱ�ɼ�ģʽ\r\n");
	printf("ҡ�����¼� : ���ڹ���������\r\n");
	printf("\r\n");
}

/*
*********************************************************************************************************
*	�� �� ��: PrintfLogo
*	����˵��: ��ӡ�������ƺ����̷�������, ���ϴ����ߺ󣬴�PC���ĳ����ն�������Թ۲���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	printf("*************************************************************\n\r");
	printf("* ��������   : %s\r\n", EXAMPLE_NAME);	/* ��ӡ�������� */
	printf("* ���̰汾   : %s\r\n", DEMO_VER);		/* ��ӡ���̰汾 */
	printf("* ��������   : %s\r\n", EXAMPLE_DATE);	/* ��ӡ�������� */

	/* ��ӡST�̼���汾����3���������stm32f10x.h�ļ��� */
	printf("* �̼���汾 : V%d.%d.%d (STM32F4xx_StdPeriph_Driver)\r\n", __STM32F4XX_STDPERIPH_VERSION_MAIN,
			__STM32F4XX_STDPERIPH_VERSION_SUB1,__STM32F4XX_STDPERIPH_VERSION_SUB2);
	printf("* \n\r");	/* ��ӡһ�пո� */
	printf("* QQ    : 1295744630 \r\n");
	printf("* Email : armfly@qq.com \r\n");
	printf("* Copyright www.armfly.com ����������\r\n");
	printf("*************************************************************\n\r");
}
