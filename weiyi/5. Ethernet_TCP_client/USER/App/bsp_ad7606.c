/*
*********************************************************************************************************
*
*	ģ������ : AD7606���ݲɼ�ģ��
*	�ļ����� : bsp_ad7606.c
*	��    �� : V1.0
*	˵    �� : AD7606����STM32��FSMC�����ϡ�
*
*			������ʹ���� TIM4 ��ΪӲ����ʱ������ʱ����ADCת��
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-02-01 armfly  ��ʽ����
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "./Bsp/systick/bsp.h"
#include "bsp_ad7606.h"


/* ���ù�������GPIO: PH9 PH10 PH11 */
/* ���ù�������GPIO: PH9 PH10 PH11 */
#define OS0_1()		GPIOE->BSRRL = GPIO_Pin_2
#define OS0_0()		GPIOE->BSRRH = GPIO_Pin_2
#define OS1_1()		GPIOE->BSRRL = GPIO_Pin_3
#define OS1_0()		GPIOE->BSRRH = GPIO_Pin_3
#define OS2_1()		GPIOE->BSRRL = GPIO_Pin_4
#define OS2_0()		GPIOE->BSRRH = GPIO_Pin_4

/* ����ADת����GPIO : PH12----PE5 , PH12/TIM5_CH3/DCMI_D3 */
#define CONVST_1()	GPIOE->BSRRL = GPIO_Pin_5
#define CONVST_0()	GPIOE->BSRRH = GPIO_Pin_5

/* �����������̵�GPIO :  */
#define RANGE_1()	GPIOB->BSRRL = GPIO_Pin_0
#define RANGE_0()	GPIOB->BSRRH = GPIO_Pin_0

/* AD7606��λ���� : PI4  */
#define RESET_1()	GPIOI->BSRRL = GPIO_Pin_4
#define RESET_0()	GPIOI->BSRRH = GPIO_Pin_4

/* AD7606 FSMC���ߵ�ַ��ֻ�ܶ�������д */
//#define AD7606_RESULT()	*(__IO uint16_t *)0x6C400000
#define AD7606_RESULT()	*(__IO uint16_t *)0x68000000

AD7606_VAR_T g_tAD7606;		/* ����1��ȫ�ֱ���������һЩ���� */
AD7606_FIFO_T g_tAdcFifo;	/* ����FIFO�ṹ����� */

static 	uint8_t i=0;
uint8_t ucRefresh = 0;


static void AD7606_CtrlLinesConfig(void);
static void AD7606_FSMCConfig(void);

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitAD7606
*	����˵��: ���������ⲿSRAM��GPIO��FSMC
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitAD7606(void)
{
	AD7606_CtrlLinesConfig();
	AD7606_FSMCConfig();

	AD7606_SetOS(AD_OS_X4);		/* �޹����� */
	AD7606_SetInputRange(1);	/* 0��ʾ��������Ϊ����5V, 1��ʾ����10V */

	AD7606_Reset();

	CONVST_1();					/* ����ת����GPIOƽʱ����Ϊ�� */
}

/*
*********************************************************************************************************
*	�� �� ��: AD7606_CtrlLinesConfig
*	����˵��: ����LCD���ƿ��ߣ�FSMC�ܽ�����Ϊ���ù���
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
/*
	������STM32-V5��������߷�����

	PD0/FSMC_D2
	PD1/FSMC_D3
	PD4/FSMC_NOE		--- �������źţ�OE = Output Enable �� N ��ʾ����Ч
	PD5/FSMC_NWE		--- д�����źţ�AD7606 ֻ�ж�����д�ź�
	PD8/FSMC_D13
	PD9/FSMC_D14
	PD10/FSMC_D15

	PD14/FSMC_D0
	PD15/FSMC_D1

	PE4/FSMC_A20		--- ����Ƭѡһ������
	PE5/FSMC_A21		--- ����Ƭѡһ������
	PE7/FSMC_D4
	PE8/FSMC_D5
	PE9/FSMC_D6
	PE10/FSMC_D7
	PE11/FSMC_D8
	PE12/FSMC_D9
	PE13/FSMC_D10
	PE14/FSMC_D11
	PE15/FSMC_D12

	PG12/FSMC_NE4		--- ��Ƭѡ��TFT, OLED �� AD7606��

	�����Ŀ���IO:

	PH9/DCMI_D0/AD7606_OS0			---> AD7606_OS0		OS2:OS0 ѡ�������˲�����
	PH10/DCMI_D1/AD7606_OS1         ---> AD7606_OS1
	PH11/DCMI_D2/AD7606_OS2         ---> AD7606_OS2
	PH12/DCMI_D3/AD7606_CONVST      ---> AD7606_CONVST	����ADCת�� (CONVSTA �� CONVSTB �Ѿ�����)
	PH14/DCMI_D4/AD7606_RAGE        ---> AD7606_RAGE	����ģ���ѹ���̣�����5V������10V
	PI4/DCMI_D5/AD7606_RESET        ---> AD7606_RESET	��λ
	PI6/DCMI_D6/AD7606_BUSY         ---> AD7606_BUSY	æ�ź�	(δʹ��)

*/
static void AD7606_CtrlLinesConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ʹ��FSMCʱ�� */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

	/* ʹ�� GPIOʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG, ENABLE);

	/* ���� PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
	 PD.10(D15), PD.14(D0), PD.15(D1) Ϊ����������� */

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
	                            GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 |
	                            GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/*
		PE4/FSMC_A20		--- ����Ƭѡһ������
		PE5/FSMC_A21		--- ����Ƭѡһ������

		PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
	 	PE.14(D11), PE.15(D12)
	*/
	//GPIO_PinAFConfig(GPIOE, GPIO_PinSource4 , GPIO_AF_FSMC);
	//GPIO_PinAFConfig(GPIOE, GPIO_PinSource5 , GPIO_AF_FSMC);

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
	                            GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
	                            GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* ���� PG12 ��ΪPG10 Ϊ����������� */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	/*	���ü��������õ�GPIO
		PH9/DCMI_D0/AD7606_OS0			---> AD7606_OS0		OS2:OS0 ѡ�������˲�����
		PH10/DCMI_D1/AD7606_OS1         ---> AD7606_OS1
		PH11/DCMI_D2/AD7606_OS2         ---> AD7606_OS2
		PH12/DCMI_D3/AD7606_CONVST      ---> AD7606_CONVST	����ADCת��
		PH14/DCMI_D4/AD7606_RAGE        ---> AD7606_RAGE	����ģ���ѹ���̣�����5V������10V
		PI4/DCMI_D5/AD7606_RESET        ---> AD7606_RESET	��λ

		PI6/DCMI_D6/AD7606_BUSY			---> AD7606_BUSY    ת���������ź�
	*/
	{
		/* ʹ�� GPIOʱ�� */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOF, ENABLE);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
		GPIO_Init(GPIOE, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_Init(GPIOF, &GPIO_InitStructure);
	}
	
	

}

/*
*********************************************************************************************************
*	�� �� ��: AD7606_FSMCConfig
*	����˵��: ����FSMC���ڷ���ʱ��
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AD7606_FSMCConfig(void)
{
	FSMC_NORSRAMInitTypeDef  init;
	FSMC_NORSRAMTimingInitTypeDef  timing;

	/*
		AD7606�����Ҫ��(3.3Vʱ)��RD���źŵ͵�ƽ���������21ns���ߵ�ƽ������̿��15ns��

		������������ ������������Ϊ�˺�ͬBANK��LCD������ͬ��ѡ��3-0-6-1-0-0
		3-0-5-1-0-0  : RD�߳���75ns�� �͵�ƽ����50ns.  1us���ڿɶ�ȡ8·�������ݵ��ڴ档
		1-0-1-1-0-0  : RD��75ns���͵�ƽִ��12ns���ң��½��ز��Ҳ12ns.  ���ݶ�ȡ��ȷ��
	*/
	/* FSMC_Bank1_NORSRAM4 configuration */
	timing.FSMC_AddressSetupTime = 3;
	timing.FSMC_AddressHoldTime = 0;
	timing.FSMC_DataSetupTime = 6;
	timing.FSMC_BusTurnAroundDuration = 1;
	timing.FSMC_CLKDivision = 0;
	timing.FSMC_DataLatency = 0;
	timing.FSMC_AccessMode = FSMC_AccessMode_A;

	/*
	 LCD configured as follow:
	    - Data/Address MUX = Disable
	    - Memory Type = SRAM
	    - Data Width = 16bit
	    - Write Operation = Enable
	    - Extended Mode = Enable
	    - Asynchronous Wait = Disable
	*/
	init.FSMC_Bank = FSMC_Bank1_NORSRAM3;
	init.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	init.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	init.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	init.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	init.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	init.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	init.FSMC_WrapMode = FSMC_WrapMode_Disable;
	init.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	init.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	init.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	init.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	init.FSMC_WriteBurst = FSMC_WriteBurst_Disable;

	init.FSMC_ReadWriteTimingStruct = &timing;
	init.FSMC_WriteTimingStruct = &timing;

	FSMC_NORSRAMInit(&init);

	/* - BANK 1 (of NOR/SRAM Bank 1~4) is enabled */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);
}

/*
*********************************************************************************************************
*	�� �� ��: AD7606_SetOS
*	����˵��: ����AD7606�����˲�����Ҳ�����ù��������ʡ�
*			 ͨ������ AD7606_OS0��OS1��OS2���ߵĵ�ƽ���״̬�������������ʡ�
*			 ����ADת��֮��AD7606�ڲ��Զ�ʵ��ʣ�������Ĳɼ���Ȼ����ƽ��ֵ�����
*
*			 ����������Խ�ߣ�ת��ʱ��Խ����
*			 �޹�����ʱ��ADת��ʱ�� 4us;
*				2��������ʱ = 8.7us;
*				4��������ʱ = 16us
*			 	64��������ʱ = 286us
*
*	��    ��: _ucOS : ����������, 0 - 6
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AD7606_SetOS(uint8_t _ucOS)
{
	g_tAD7606.ucOS = _ucOS;
	switch (_ucOS)
	{
		case AD_OS_X2:
			OS2_0();
			OS1_0();
			OS0_1();
			break;

		case AD_OS_X4:
			OS2_0();
			OS1_1();
			OS0_0();
			break;

		case AD_OS_X8:
			OS2_0();
			OS1_1();
			OS0_1();
			break;

		case AD_OS_X16:
			OS2_1();
			OS1_0();
			OS0_0();
			break;

		case AD_OS_X32:
			OS2_1();
			OS1_0();
			OS0_1();
			break;

		case AD_OS_X64:
			OS2_1();
			OS1_1();
			OS0_0();
			break;

		case AD_OS_NO:
		default:
			g_tAD7606.ucOS = AD_OS_NO;
			OS2_0();
			OS1_0();
			OS0_0();
			break;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AD7606_SetInputRange
*	����˵��: ����AD7606ģ���ź��������̡�
*	��    ��: _ucRange : 0 ��ʾ����5V   1��ʾ����10V
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AD7606_SetInputRange(uint8_t _ucRange)
{
	if (_ucRange == 0)
	{
		g_tAD7606.ucRange = 0;
		RANGE_0();	/* ����Ϊ����5V */
	}
	else
	{
		g_tAD7606.ucRange = 1;
		RANGE_1();	/* ����Ϊ����10V */
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AD7606_Reset
*	����˵��: Ӳ����λAD7606����λ֮��ָ�����������״̬��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AD7606_Reset(void)
{
	RESET_0();	/* �˳���λ״̬ */

	RESET_1();	/* ���븴λ״̬ */
	RESET_1();	/* �������ӳ١� RESET��λ�ߵ�ƽ��������С50ns�� */
	RESET_1();
	RESET_1();

	RESET_0();	/* �˳���λ״̬ */
}

/*
*********************************************************************************************************
*	�� �� ��: AD7606_StartConvst
*	����˵��: ����1��ADCת��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AD7606_StartConvst(void)
{
	/* page 7��  CONVST �ߵ�ƽ�����Ⱥ͵͵�ƽ��������� 25ns */
	/* CONVSTƽʱΪ�� */
	CONVST_0();
	CONVST_0();
	CONVST_0();

	CONVST_1();
}

/*
*********************************************************************************************************
*	�� �� ��: AD7606_ReadNowAdc
*	����˵��: ��ȡ8·�������������洢��ȫ�ֱ��� g_tAD7606
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AD7606_ReadNowAdc(void)
{
	g_tAD7606.sNowAdc[0] = AD7606_RESULT();	/* ����1·���� */
	g_tAD7606.sNowAdc[1] = AD7606_RESULT();	/* ����2·���� */
	g_tAD7606.sNowAdc[2] = AD7606_RESULT();	/* ����3·���� */
	g_tAD7606.sNowAdc[3] = AD7606_RESULT();	/* ����4·���� */
	g_tAD7606.sNowAdc[4] = AD7606_RESULT();	/* ����5·���� */
	g_tAD7606.sNowAdc[5] = AD7606_RESULT();	/* ����6·���� */
	g_tAD7606.sNowAdc[6] = AD7606_RESULT();	/* ����7·���� */
	g_tAD7606.sNowAdc[7] = AD7606_RESULT();	/* ����8·���� */
	

	for( i=0;i<8;i++)
			{
				g_tAD7606.tcpadc[i*2+1] = g_tAD7606.sNowAdc[i] & 0x00ff;       // ??8?
				g_tAD7606.tcpadc[i*2] = g_tAD7606.sNowAdc[i] >> 8;       //??8?
			}

}








/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
