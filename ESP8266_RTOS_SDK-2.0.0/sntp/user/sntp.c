

#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "sntp.h"

//#define SntpServerName "ntp1.aliyun.com"
//#define SERVERPORT 80



/******************************************************************************
 * FunctionName : ATaskSntp
 * Description  : ATaskSntp 任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ATaskSntp( void *pvParameters )
{
	STATION_STATUS StaStatus;
	uint32 time;
	do
	{
		StaStatus = wifi_station_get_connect_status();
		vTaskDelay(100);
		
	}while(StaStatus != STATION_GOT_IP);

	printf("task is SNTP\n");
	printf("STATION_GOT_IP!\n");
	sntp_setservername(0,"0.cn.pool.ntp.org");
	sntp_setservername(1,"1.cn.pool.ntp.org");
	sntp_setservername(2,"2.cn.pool.ntp.org");
	sntp_init();
	for(;;)
	{
		time = sntp_get_current_timestamp();
		if(time)
		{
			printf("time:%d\r\n",time);
			printf("current data:%s\n",sntp_get_real_time(time));
		}
		vTaskDelay(500);
	}
	vTaskDelete(NULL);
	}

/******************************************************************************
 * FunctionName : SntpServer_init
 * Description  : SntpServer_init 初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void Sntp_init( void )
{
	xTaskCreate(ATaskSntp, "Sntp", 512, NULL, 4, NULL);

}













