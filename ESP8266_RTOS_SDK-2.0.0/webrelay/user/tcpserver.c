#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "tcpserver.h"
#include "relay.h"

#define SERVERADDR "192.168.1.6"
#define SERVERPORT 8000

/******************************************************************************
 * FunctionName : ATaskTcpServer
 * Description  : ATaskTcpServer 任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ATaskTcpServer( void *pvParameters )
{
	int iVariabSleExample = 0;
	int fd = -1;
	int NetTimeOut = 20000;
	int ret;
	int cfd = -1;

	struct sockaddr_in ServerAddr;
	struct sockaddr ClientAddr;
	socklen_t ClientAddrlen = sizeof(struct sockaddr);
	char Tcpmsg[48];

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
		return;
	}

	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &NetTimeOut, sizeof(int));

	memset(&ServerAddr,0,sizeof(ServerAddr));

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = INADDR_ANY;
	ServerAddr.sin_port = htons(SERVERPORT);
	ServerAddr.sin_len = sizeof(ServerAddr);

	if(bind(fd, (struct sockaddr*)&ServerAddr, ServerAddr.sin_len) != 0)
	{
		printf("get bind failed \n");
		vTaskDelete(NULL);
		return;
	}

	if(listen(fd, 5) != 0)
	{
		printf("listen failed \n");
		vTaskDelete(NULL);
		return;
	}
		
		for(;;)
		{
			cfd = accept(fd, &ClientAddr, &ClientAddrlen);
			if(cfd != -1)
			{
				ret = recv(cfd, Tcpmsg, 48,0);

				if(ret > 0)
				{
					printf("TcpClient:%s\n",Tcpmsg);
					send(cfd, "I am TcpServer!", sizeof("I am TcpServer!"),0);
				}
				else
				{
					printf("Tcpserver data is no\n");

				}
			}
			close(cfd);
		
		}
	
		vTaskDelete( NULL );

}

/******************************************************************************
 * FunctionName : TcpServer_init
 * Description  : TcpServer_init 初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void TcpServer_init( void )
{
	xTaskCreate(ATaskTcpServer, "TcpServer", 256, NULL, 4, NULL);

}











