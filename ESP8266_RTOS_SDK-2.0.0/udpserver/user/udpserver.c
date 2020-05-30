#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "udpserver.h"

#define SERVERADDR "192.168.1.6"
#define SERVERPORT 8000

/******************************************************************************
 * FunctionName : ATaskUdpServer
 * Description  : ATaskUdpServer 任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ATaskUdpServer( void *pvParameters )
{
	int iVariableExample = 0;
	int fd = -1;
	int NetTimeOut = 5000;
	int ret;

	struct sockaddr_in ServerAddr;
	struct sockaddr from;	
	socklen_t fromlen = sizeof(struct sockaddr);
	char udpmsg[48];

	STATION_STATUS StaStatus;
	do
	{
		StaStatus = wifi_station_get_connect_status();
		vTaskDelay(100);
		
	}while(StaStatus != STATION_GOT_IP);

	fd = socket(PF_INET, SOCK_DGRAM, 0);
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

	if(	bind(fd, (struct sockaddr*)&ServerAddr,ServerAddr.sin_len ) != 0)
	{

		printf("bind socket failed!\n");
		vTaskDelete(NULL);
		return;
	}
			
		for(;;)
		{
			do
			{
				ret = recvfrom(fd, udpmsg, 48,\
					0, &from, (&fromlen));

				if(ret > 0)
				{
					printf("udpclient:%s\n",udpmsg);
				}
				else
				{
					printf("udpclient data is no\n");

				}
			}while(ret == -1);
	
			sendto(fd, "I am UdpServer!", sizeof("I am UpClient!"),\
					0, &from, fromlen);
			
		}
	
		vTaskDelete( NULL );

}

/******************************************************************************
 * FunctionName : UdpServer_init
 * Description  : UdpServer_init 初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void UdpServer_init( void )
{
	xTaskCreate(ATaskUdpServer, "UdpServer", 256, NULL, 4, NULL);

}


