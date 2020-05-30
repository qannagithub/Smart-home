#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "tcpclient.h"

#define SERVERADDR "192.168.1.6"
#define SERVERPORT 8000

/******************************************************************************
 * FunctionName : ATaskTcpClient
 * Description  : ATaskTcpClient 任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ATaskTcpClient( void *pvParameters )
{
	int iVariabSleExample = 0;
	int fd = -1;
	int NetTimeOut = 5000;
	int ret;

	struct sockaddr_in ServerAddr;
	char udpmsg[48];

	STATION_STATUS StaStatus;
	do
	{
		StaStatus = wifi_station_get_connect_status();
		vTaskDelay(100);
		
	}while(StaStatus != STATION_GOT_IP);

	fd = socket(PF_INET, SOCK_STREAM, 0);
	if(fd == -1)
	{
		printf("get socket failed \n");
		vTaskDelete(NULL);

	}

	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &NetTimeOut, sizeof(int));

	memset(&ServerAddr,0,sizeof(ServerAddr));

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr(SERVERADDR);
	ServerAddr.sin_port = htons(SERVERPORT);
	ServerAddr.sin_len = sizeof(ServerAddr);

	do
	{
		ret = connect(fd,(struct sockaddr *)&ServerAddr, ServerAddr.sin_len);
		if(ret != 1)
		{
			printf("连接失败\n");
			vTaskDelay(100);
		}
	}
	while(ret != 0);
			
		for(;;)
		{
			send(fd, "I am TcpClient!", sizeof("I am TcpClient!"),0);
			do
			{
				ret = recv(fd, udpmsg, 48,0);

				if(ret > 0)
				{
					printf("Tcpserver:%s\n",udpmsg);
				}
				else
				{
					printf("Tcpserver data is no\n");

				}
			}while(ret == -1);
	
		}
	
		vTaskDelete( NULL );

}

/******************************************************************************
 * FunctionName : TcpClient_init
 * Description  : TcpClient_init 初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void TcpClient_init( void )
{
	xTaskCreate(ATaskTcpClient, "TcpClient", 256, NULL, 4, NULL);

}










