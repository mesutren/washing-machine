/*
*********************************************************************************************************
*
*	模块名称 : TCPnet网络协议栈测试
*	文件名称 : app_tcpnet_lib.c
*	版    本 : V1.0
*	说    明 : 测试的功能说明
*              1. 强烈推荐将网线接到路由器或者交换机上面测试，因为已经使能了DHCP，可以自动获取IP地址。
*              2. 客户端的例子相比服务器的例子稍麻烦些，因为客户端的例子需要用户知道电脑端IP和端口号。
*                 并根据实际情况设置IP和端口号的宏定义，这个配置在文件app_tcpnet_lib.c开头，测试的时
*                 候板子要连接这个IP和端口（下面是默认配置，一定要根据实际情况重新配置，如果不会配置，
*                 看本例程对应的教程即可）：
*                 #define IP1            192
*                 #define IP2            168
*                 #define IP3            1
*                 #define IP4            2                 
*                 #define PORT_NUM       1001
*              3. 创建了一个TCP Client，而且使能了局域网域名NetBIOS，用户只需在电脑端ping armfly
*                 就可以获得板子的IP地址，本地端口被设置为1024。
*              4. 用户可以在电脑端用网络调试软件创建TCP Server连接此客户端。
*              5. 按键K1按下，发送8字节的数据给TCP Server。
*              6. 按键K2按下，发送1024字节的数据给TCP Server。
*              7. 按键K3按下，发送5MB字节的数据给TCP Server。
*
*	修改记录 :
*		版本号   日期         作者        说明
*		V1.0    2017-04-17   Eric2013     首发
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/		
#include "bsp.h"		
#include <RTL.h>
#include <stdio.h>
#include <Net_Config.h>



/*
*********************************************************************************************************
*	                                  用于本文件的调试
*********************************************************************************************************
*/
#if 1
	#define printf_debug printf
#else
	#define printf_debug(...)
#endif


/*
*********************************************************************************************************
*	                          宏定义，远程服务器的IP和端口
*********************************************************************************************************
*/
/* 要访问的远程服务器IP和端口配置，也就是电脑端调试助手设置的IP和端口号 */
#define IP1            192
#define IP2            168
#define IP3            23
#define IP4            10

#define PORT_NUM         1001

/* 这个是本地端口 */
#define LocalPort_NUM    1024

/*
*********************************************************************************************************
*	                                     变量
*********************************************************************************************************
*/
uint8_t socket_tcp;
uint8_t Rem_IP[4] = {IP1,IP2,IP3,IP4};


/*
*********************************************************************************************************
*	函 数 名: tcp_callback
*	功能说明: TCP Socket的回调函数
*	形    参: soc  TCP Socket类型
*             evt  事件类型
*             ptr  事件类型是TCP_EVT_DATA，ptr指向的缓冲区记录着接收到的TCP数据，其余事件记录IP地址
*             par  事件类型是TCP_EVT_DATA，记录接收到的数据个数，其余事件记录端口号
*	返 回 值: 
*********************************************************************************************************
*/
U16 tcp_callback (U8 soc, U8 evt, U8 *ptr, U16 par)
{
	char buf[50];
	uint16_t i;
	
	/* 确保是socket_tcp的回调 */
	if (soc != socket_tcp) 
	{
		return (0);
	}

	switch (evt) 
	{
		/*
			远程客户端连接消息
		    1、数组ptr存储远程设备的IP地址，par中存储端口号。
		    2、返回数值1允许连接，返回数值0禁止连接。
		*/
		case TCP_EVT_CONREQ:
			sprintf(buf, "远程客户端请求连接IP: %d.%d.%d.%d", ptr[0], ptr[1], ptr[2], ptr[3]);
			printf_debug("IP:%s  port:%d\r\n", buf, par);
			return (1);
		
		/* 连接终止 */
		case TCP_EVT_ABORT:
			break;
		
		/* Socket远程连接已经建立 */
		case TCP_EVT_CONNECT:
			printf_debug("Socket is connected to remote peer\r\n");
			break;
		
		/* 连接断开 */
		case TCP_EVT_CLOSE:
		   	printf_debug("Connection has been closed\r\n");
			break;
		
		/* 发送的数据收到远程设备应答 */
		case TCP_EVT_ACK:
			break;
		
		/* 接收到TCP数据帧，ptr指向数据地址，par记录数据长度，单位字节 */
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
*	函 数 名: TCP_StatusCheck
*	功能说明: 检测TCP的连接状态，主要用于网线插拔的判断
*	形    参: 无
*	返 回 值: __TRUE  连接
*             __FALSE 断开
*********************************************************************************************************
*/
uint8_t TCP_StatusCheck(void) 
{
	switch (tcp_get_state(socket_tcp)) 
	{
		case TCP_STATE_FREE:
		case TCP_STATE_CLOSED:
			{
				/* 这里默认不开启调试，否则未跟电脑端服务器连接前会一直发 */
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
*	函 数 名: tcpnet_poll
*	功能说明: 使用TCPnet必须要一直调用的函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void tcpnet_poll(void)
{
	if(bsp_CheckTimer(0))
	{
		/* 此函数坚决不可以放在中断里面跑 */
		timer_tick ();
	}
	
	main_TcpNet ();
}

/*
*********************************************************************************************************
*	函 数 名: TCPnetTest
*	功能说明: TCPnet应用
*	形    参: 无
*	返 回 值: 无
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
	
	
	/* 初始化网络协议栈 */
	init_TcpNet ();

	/* 
	   创建TCP Socket并连接，客户端连接服务器后，10秒内无数据通信将断开连接。
	   但是由于这里使能了TCP_TYPE_KEEP_ALIVE，会一直保持连接，不受10秒的时间限制。
	*/
    socket_tcp = tcp_get_socket (TCP_TYPE_CLIENT | TCP_TYPE_KEEP_ALIVE, 0, 10, tcp_callback);
	if(socket_tcp != 0)
	{
		res = tcp_connect (socket_tcp, Rem_IP, PORT_NUM, LocalPort_NUM);
		printf("TCP Socket创建成功res = %d\r\n", res);
	}

	/* 创建一个周期是100ms的软定时器 */
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
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
