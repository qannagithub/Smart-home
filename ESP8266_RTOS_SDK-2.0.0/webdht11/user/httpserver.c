
#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "httpserver.h"
#include "dht11.h"

#define SERVERADDR "192.168.1.6"
#define SERVERPORT 80


const char *DefaultPage=
"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
"<html xmlns=\"http://www.w3.org/1999/xhtml\">"
"<head>"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />"
"<title>智能环境采集</title>"
"<script defer=\"defer\">"
"	function ledSwitch(){"
"		var xmlhttp;"
"		if(window.XMLHttpRequest){"
"			xmlhttp = new XMLHttpRequest();"
"		} else {"
"			xmlhttp = new ActiveXObject(\"Microsoft.XMLHTTP\");"
"		}"
"		xmlhttp.onreadystatechange = function () {"
"			if(xmlhttp.readyState == 4 && xmlhttp.status == 200)"
"				document.getElementById(\"txtState\").innerHTML = xmlhttp.responseText;"
"		    }"
"	       },"
"	    xmlhttp.open(\"GET\",\"Data\",true);"
"	   	xmlhttp.send();"
"}"
"</script>"
"</head>"
"<body style=\"background-color: #2F4F4F\">"
"<font size=\"12\" color=\"yellow\">"
"<b>"
"<div class=\"text\" style=\" text-align:center;\"><big>WIFI-智能家居</big>"
"</b>"
"</font>"
"<font size=\"12\" color=\"yellow\">"
"<b>"
"<div class=\"text\" style=\" text-align:center;\"><big>智能环境采集</big></div>"
"</b>"
"</font>"
"<br></br>"
"<font size=\"20\" color=\"white\">"
"<div style=\" center\" id=\"txtState\">环境数据未知</div>"
"</font>"
"<br></br>"
"<div style=\" text-align: center;\">"
"<input type=\"button\" value=\"获取数据\" style=\"width:80px;height:40px;\" onclick=\"ledSwitch()\"> "
"</div>"
"</body>"
"</html>";

int file_ok(int cfd,long flen)
{
	char *send_buf = zalloc(sizeof(char)*100);
	sprintf(send_buf,"HTTP/1.1 200 OK\r\n");
	send(cfd,send_buf,strlen(send_buf),0);
	sprintf(send_buf,"Connection: keep-alive\r\n");
	send(cfd,send_buf,strlen(send_buf),0);
	sprintf(send_buf,"Content-Length: %ld\r\n",flen);
	send(cfd,send_buf,strlen(send_buf),0);
	sprintf(send_buf,"Content-Type: text/html\r\n");
	send(cfd,send_buf,strlen(send_buf),0);
	sprintf(send_buf,"\r\n");
	send(cfd,send_buf,strlen(send_buf),0);
	free(send_buf);
	return 0;
}



/******************************************************************************
 * FunctionName : ATaskHttpServer
 * Description  : ATaskHttpServer 任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ATaskHttpServer( void *pvParameters )
{
	int iVariabSleExample = 0;
	int fd = -1;
	int NetTimeOut = 20000;
	int ret;
	int cfd = -1;

	struct sockaddr_in ServerAddr;
	struct sockaddr ClientAddr;
	socklen_t ClientAddrlen = sizeof(struct sockaddr);
	char *Httpmsg;
	char *Sendmsg;
	char data[30];

	extern uint8 ucharT_data_H,ucharRH_data_H;

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
	Httpmsg = (char*)zalloc(sizeof(char)*1000);
		
		for(;;)
		{
			cfd = accept(fd, &ClientAddr, &ClientAddrlen);
			if(cfd != -1)
			{
				printf("HttpClient accept\n");				
				ret = recv(cfd, Httpmsg, 1000,0);			
				if(ret > 0)
				{
					printf("HttpClient recv\n");
					printf("%s\n",Httpmsg);
					if(strstr(Httpmsg,"GET / HTTP/1.1") != NULL)
					{
						file_ok(cfd,strlen(DefaultPage));
						send(cfd, DefaultPage, strlen(DefaultPage),0);
					}
					else if(strstr(Httpmsg,"GET /Data") != NULL)
					{
						sprintf(data,"温度=%d\n湿度=%d",ucharT_data_H,ucharRH_data_H);
						send(cfd,data,strlen(data),0);
					}
				}
		else
		{
			printf("HttpClient data is no!\n");
		}
		
		}
	close(cfd);
}
		vTaskDelete( NULL );

}

/******************************************************************************
 * FunctionName : HttpServer_init
 * Description  : HttpServer_init 初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void HttpServer_init( void )
{
	xTaskCreate(ATaskHttpServer, "HttpServer", 256, NULL, 4, NULL);

}











