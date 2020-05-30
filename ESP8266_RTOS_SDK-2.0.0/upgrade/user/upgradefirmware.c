
#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "upgrade.h"


/******************************************************************************
 * FunctionName : user_esp_platform_upgrade_cb
 * Description  : Processing the downloaded data from the server
 * Parameters   : pespconn -- the espconn used to connetion with the host
 * Returns      : none
*******************************************************************************/
LOCAL void  
DeviceUpgradeRsp(void *arg)
{
	struct upgrade_server_info *server = arg;

	if (server->upgrade_flag == true) 
	{
		printf("upgarde_successfully\n");
//		system_upgrade_reboot( );
	} 
	else 
	{
		printf("upgrade_failed\n");
	}

	if(server != NULL)
	{
		free(server->url);
		server->url = NULL;
		free(server);
		server = NULL;
	}
}

/**
	* @brief  no .	  
	* @note   no.
	* @param  no.
	* @retval no.
	*/
bool ExcuteUpgrade( char * FirmWareDownLoadPath )
{
	char * Url = FirmWareDownLoadPath;
	
	struct	hostent hostinfo,*phost;
	char buf[101];
	char hostname[100];
	char UserBinStr[10];
	int ret;
	char *ipaddr = NULL;
	char *ptr , *path;

	if( Url == NULL )
	{
		printf("error:have no url!\r\n");
		return ;
	}

	if( strncmp( Url , "http://" , 7) == 0 )
	{ 
		Url += 7;
	}
	
	ptr = strchr( Url , '/' );
	if( ptr == NULL )
	{
		printf("error:url parse!\r\n");
		return ;
	}
	path = ptr;
	
	memcpy( hostname , Url , ptr - Url );
	hostname[ ptr - Url ] = 0x00;

	err_t err;
	uint8_t CycleNum = 0;

	if( inet_addr( hostname ) == INADDR_NONE )
	{
		do
		{
			if( CycleNum > 0 )
			{
				printf("get host by name Count:%d!\r\n" , CycleNum );
				vTaskDelay( 100 / portTICK_RATE_MS );
			}
			err = gethostbyname_r( hostname , &hostinfo , buf , 100 , &phost , &ret );
		}
		while( ( err != ERR_OK ) && ( ++CycleNum < 5 ) );
	
		if( err == ERR_OK )
		{

			int i;

			for( i = 0; hostinfo.h_addr_list[i]; i++ )
			{
				ipaddr = inet_ntoa( *(struct in_addr*)hostinfo.h_addr_list[i] );
				if( ipaddr != NULL )
				{
	//				printf("host addr is:%s\n",  ipaddr );
					break;
				}
			}

			if( ipaddr == NULL )
			{
				printf("error:get ip fail!\r\n");
				return ;
			}
		}
		else
		{
			printf( "error:gethostbyname\r\n" );
			return ;
		}
	}
	else
	{
		ipaddr = hostname;
		printf("host name is ip!\r\n");
	}
	
	struct upgrade_server_info *UpgradeServer = NULL;

	UpgradeServer = (struct upgrade_server_info *)zalloc( sizeof( struct upgrade_server_info ) );

	bzero(&UpgradeServer->sockaddrin, sizeof(struct sockaddr_in) );
	
	UpgradeServer->sockaddrin.sin_family = AF_INET;
	UpgradeServer->sockaddrin.sin_addr.s_addr = inet_addr( ipaddr );
	UpgradeServer->sockaddrin.sin_port = htons( 80 );
	UpgradeServer->sockaddrin.sin_len = sizeof( struct sockaddr );

	UpgradeServer->check_cb = DeviceUpgradeRsp;

	if (UpgradeServer->url == NULL) 
	{
		UpgradeServer->url = (uint8 *)zalloc(256);
	}

	if ( system_upgrade_userbin_check( ) == UPGRADE_FW_BIN1 ) 
	{
		strcpy( UserBinStr , "user2.bin" );
	}
	else
	{
		strcpy( UserBinStr , "user1.bin" );
	}
	UserBinStr[9] = '\0';

	sprintf(	UpgradeServer->url , "GET %s/%s HTTP/1.0\r\nHost: %s\r\nConnection: keep-alive\r\nCache-Control: no-cache\r\nAccept: */*\r\n\r\n" , path , UserBinStr , hostname );
	printf("%s" , UpgradeServer->url );
   	
	if( system_upgrade_start( UpgradeServer ) == true )
	{
		return true;
	}

	return false;

}
