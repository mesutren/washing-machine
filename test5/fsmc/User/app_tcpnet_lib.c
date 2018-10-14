/*
*********************************************************************************************************
*
*	ģ������ : TCPnet����Э��ջ����
*	�ļ����� : app_tcpnet_lib.c
*	��    �� : V1.0
*	˵    �� : ���ԵĹ���˵��
*              1. ǿ���Ƽ������߽ӵ�·�������߽�����������ԣ���Ϊ�Ѿ�ʹ����DHCP�������Զ���ȡIP��ַ��
*              2. �ͻ��˵�������ȷ��������������鷳Щ����Ϊ�ͻ��˵�������Ҫ�û�֪�����Զ�IP�Ͷ˿ںš�
*                 ������ʵ���������IP�Ͷ˿ںŵĺ궨�壬����������ļ�app_tcpnet_lib.c��ͷ�����Ե�ʱ
*                 �����Ҫ�������IP�Ͷ˿ڣ�������Ĭ�����ã�һ��Ҫ����ʵ������������ã�����������ã�
*                 �������̶�Ӧ�Ľ̳̼��ɣ���
*                 #define IP1            192
*                 #define IP2            168
*                 #define IP3            1
*                 #define IP4            2                 
*                 #define PORT_NUM       1001
*              3. ������һ��TCP Client������ʹ���˾���������NetBIOS���û�ֻ���ڵ��Զ�ping armfly
*                 �Ϳ��Ի�ð��ӵ�IP��ַ�����ض˿ڱ�����Ϊ1024��
*              4. �û������ڵ��Զ�����������������TCP Server���Ӵ˿ͻ��ˡ�
*              5. ����K1���£�����8�ֽڵ����ݸ�TCP Server��
*              6. ����K2���£�����1024�ֽڵ����ݸ�TCP Server��
*              7. ����K3���£�����5MB�ֽڵ����ݸ�TCP Server��
*
*	�޸ļ�¼ :
*		�汾��   ����         ����        ˵��
*		V1.0    2017-04-17   Eric2013     �׷�
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/		
#include "bsp.h"		
#include <RTL.h>
#include <stdio.h>
#include <Net_Config.h>



/*
*********************************************************************************************************
*	                                  ���ڱ��ļ��ĵ���
*********************************************************************************************************
*/
#if 1
	#define printf_debug printf
#else
	#define printf_debug(...)
#endif


/*
*********************************************************************************************************
*	                          �궨�壬Զ�̷�������IP�Ͷ˿�
*********************************************************************************************************
*/
/* Ҫ���ʵ�Զ�̷�����IP�Ͷ˿����ã�Ҳ���ǵ��Զ˵����������õ�IP�Ͷ˿ں� */
#define IP1            192
#define IP2            168
#define IP3            23
#define IP4            10

#define PORT_NUM         1001

/* ����Ǳ��ض˿� */
#define LocalPort_NUM    1024

/*
*********************************************************************************************************
*	                                     ����
*********************************************************************************************************
*/
uint8_t socket_tcp;
uint8_t Rem_IP[4] = {IP1,IP2,IP3,IP4};


/*
*********************************************************************************************************
*	�� �� ��: tcp_callback
*	����˵��: TCP Socket�Ļص�����
*	��    ��: soc  TCP Socket����
*             evt  �¼�����
*             ptr  �¼�������TCP_EVT_DATA��ptrָ��Ļ�������¼�Ž��յ���TCP���ݣ������¼���¼IP��ַ
*             par  �¼�������TCP_EVT_DATA����¼���յ������ݸ����������¼���¼�˿ں�
*	�� �� ֵ: 
*********************************************************************************************************
*/
U16 tcp_callback (U8 soc, U8 evt, U8 *ptr, U16 par)
{
	char buf[50];
	uint16_t i;
	
	/* ȷ����socket_tcp�Ļص� */
	if (soc != socket_tcp) 
	{
		return (0);
	}

	switch (evt) 
	{
		/*
			Զ�̿ͻ���������Ϣ
		    1������ptr�洢Զ���豸��IP��ַ��par�д洢�˿ںš�
		    2��������ֵ1�������ӣ�������ֵ0��ֹ���ӡ�
		*/
		case TCP_EVT_CONREQ:
			sprintf(buf, "Զ�̿ͻ�����������IP: %d.%d.%d.%d", ptr[0], ptr[1], ptr[2], ptr[3]);
			printf_debug("IP:%s  port:%d\r\n", buf, par);
			return (1);
		
		/* ������ֹ */
		case TCP_EVT_ABORT:
			break;
		
		/* SocketԶ�������Ѿ����� */
		case TCP_EVT_CONNECT:
			printf_debug("Socket is connected to remote peer\r\n");
			break;
		
		/* ���ӶϿ� */
		case TCP_EVT_CLOSE:
		   	printf_debug("Connection has been closed\r\n");
			break;
		
		/* ���͵������յ�Զ���豸Ӧ�� */
		case TCP_EVT_ACK:
			break;
		
		/* ���յ�TCP����֡��ptrָ�����ݵ�ַ��par��¼���ݳ��ȣ���λ�ֽ� */
		case TCP_EVT_DATA:
			printf_debug("Data length = %d\r\n", par);
			for(i = 0; i < par; i++)
			{
				printf_debug("ptr[%d] = %d\r\n", i, ptr[i]);
			}
			break;
	}
	
	return (0);
}

/*
*********************************************************************************************************
*	�� �� ��: TCP_StatusCheck
*	����˵��: ���TCP������״̬����Ҫ�������߲�ε��ж�
*	��    ��: ��
*	�� �� ֵ: __TRUE  ����
*             __FALSE �Ͽ�
*********************************************************************************************************
*/
uint8_t TCP_StatusCheck(void) 
{
	switch (tcp_get_state(socket_tcp)) 
	{
		case TCP_STATE_FREE:
		case TCP_STATE_CLOSED:
			{
				/* ����Ĭ�ϲ��������ԣ�����δ�����Զ˷���������ǰ��һֱ�� */
				#if 1
				  uint8_t res;
				  res = tcp_connect (socket_tcp, Rem_IP, PORT_NUM, LocalPort_NUM);
				  printf_debug("tcp connect res = %d\r\n", res);
                #else
				  tcp_connect (socket_tcp, Rem_IP, PORT_NUM, LocalPort_NUM);
				#endif
			}
			break;
		
		case TCP_STATE_LISTEN:
			break;
		
		case TCP_STATE_CONNECT:
			return (__TRUE);
			
		default:  
			break;
	}
	
	return (__FALSE);
}

/*
*********************************************************************************************************
*	�� �� ��: tcpnet_poll
*	����˵��: ʹ��TCPnet����Ҫһֱ���õĺ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void tcpnet_poll(void)
{
	if(bsp_CheckTimer(0))
	{
		/* �˺�����������Է����ж������� */
		timer_tick ();
	}
	
	main_TcpNet ();
}

/*
*********************************************************************************************************
*	�� �� ��: TCPnetTest
*	����˵��: TCPnetӦ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void TCPnetTest(void)
{  
	int32_t iCount;
	uint8_t *sendbuf;
	uint8_t tcp_status;
	uint16_t maxlen;
	uint8_t res;
	uint8_t ucKeyCode;
	
	
	/* ��ʼ������Э��ջ */
	init_TcpNet ();

	/* 
	   ����TCP Socket�����ӣ��ͻ������ӷ�������10����������ͨ�Ž��Ͽ����ӡ�
	   ������������ʹ����TCP_TYPE_KEEP_ALIVE����һֱ�������ӣ�����10���ʱ�����ơ�
	*/
    socket_tcp = tcp_get_socket (TCP_TYPE_CLIENT | TCP_TYPE_KEEP_ALIVE, 0, 10, tcp_callback);
	if(socket_tcp != 0)
	{
		res = tcp_connect (socket_tcp, Rem_IP, PORT_NUM, LocalPort_NUM);
		printf("TCP Socket�����ɹ�res = %d\r\n", res);
	}

	/* ����һ��������100ms����ʱ�� */
    bsp_StartAutoTimer(0, 100);
}

	void tcpnet(void)
	{
//		int32_t iCount = 8;

		uint16_t length;
		uint8_t *sendbuf;
		/* TCP?? */
		tcpnet_poll();
		
		/* ????????? */


						tcpnet_poll();
						if (tcp_check_send (socket_tcp) == __TRUE) 
						{
							length = 16;
							
							sendbuf = tcp_get_buf(length);
							sendbuf[0] = g_tAD7606.tcpadc[0];
							sendbuf[1] = g_tAD7606.tcpadc[1];
							sendbuf[2] = g_tAD7606.tcpadc[2];
							sendbuf[3] = g_tAD7606.tcpadc[3];
							sendbuf[4] = g_tAD7606.tcpadc[4];
							sendbuf[5] = g_tAD7606.tcpadc[5];
							sendbuf[6] = g_tAD7606.tcpadc[6];
							sendbuf[7] = g_tAD7606.tcpadc[7];
							sendbuf[8] = g_tAD7606.tcpadc[8];
							sendbuf[9] = g_tAD7606.tcpadc[9];
							sendbuf[10] = g_tAD7606.tcpadc[10];
							sendbuf[11] = g_tAD7606.tcpadc[11];
							sendbuf[12] = g_tAD7606.tcpadc[12];
							sendbuf[13] = g_tAD7606.tcpadc[13];
							sendbuf[14] = g_tAD7606.tcpadc[14];
							sendbuf[15] = g_tAD7606.tcpadc[15];
							
							/* ????????????? */
							tcp_send (socket_tcp, sendbuf , length);
					}
						

			}
/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
