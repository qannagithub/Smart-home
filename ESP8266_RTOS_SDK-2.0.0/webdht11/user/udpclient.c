#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "udpclient.h"

#define SERVERADDR "192.168.1.6"
#define SERVERPORT 8000

/******************************************************************************
 * FunctionName : ATaskUdpclient
 * Description  : ATaskUdpclient 任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ATaskUdpclient( void *pvParameters )
{
	int iVariableExample = 0;
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

	fd = socket(PF_INET, SOCK_DGRAM, 0);
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
			
		for(;;)
		{
			sendto(fd, "I am UpClient!", sizeof("I am UpClient!"),\
					0, (struct sockaddr *)&ServerAddr, (socklen_t)ServerAddr.sin_len);
			do
			{
				ret = recvfrom(fd, udpmsg, 48,\
					0, (struct sockaddr *)&ServerAddr, (socklen_t *)(&ServerAddr.sin_len));

				if(ret > 0)
				{
					printf("udpserver:%s\n",udpmsg);
				}
				else
				{
					printf("udpserver data is no\n");

				}
			}while(ret == -1);
	
		}
	
		vTaskDelete( NULL );

}

/******************************************************************************
 * FunctionName : Udpclient_init
 * Description  : Udpclient_init 初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void Udpclient_init( void )
{
	xTaskCreate(ATaskUdpclient, "UdpClient", 256, NULL, 4, NULL);

}









