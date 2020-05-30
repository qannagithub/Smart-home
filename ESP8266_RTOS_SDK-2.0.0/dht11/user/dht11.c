#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gpio.h"
#include "dht11.h"

#define DHT11_PIN_NUM 5//定义GPIO5
#define DHT11_PIN_BIT GPIO_Pin_5//在定义中就是指GPIO5

#define DHT11_SET 1//拉高
#define DHT11_RESET 0//拉底

//温湿度定义
uint8 ucharT_data_L=0,ucharRH_data_L=0,ucharcheckdata=0;
uint8 ucharT_data_H=0;
uint8 ucharRH_data_H=0;


static void DHT11_TEST(void)//测量接口
{
	//8bit湿度整数数据，8bit湿度小数数据，8bi温度整数数据，8bit温度小数数据
	uint8 ucharT_data_H_temp,ucharT_data_L_temp,ucharRH_data_H_humidity,ucharRH_data_L_humidity,ucharcheckdata_temp;
	uint8 ucharFLAG = 0,uchartemp=0;
	uint8 ucharcomdata;
	uint8 i;

	taskENTER_CRITICAL();//进入临界段，防止被打断

	{
	//拉底18ms
	GPIO_OUTPUT_SET(DHT11_PIN_NUM,DHT11_RESET);//配置为输出模式，参数为GPIO5，拉底，为0
	os_delay_us(18*1000);//延迟18ms，换算：1ms=1000us,1s=1000ms
	GPIO_OUTPUT_SET(DHT11_PIN_NUM,DHT11_SET);//延迟后开始拉高，为1
	//拉高40us
	GPIO_AS_INPUT(DHT11_PIN_BIT);//释放GPIO5
	os_delay_us(4*10);//延迟等待40us
	}
	//等待拉底80us应答
	if(!GPIO_INPUT_GET(DHT11_PIN_NUM))//如果读取的是0，就是真，拉低就是应答。此处取反
	{
		//等待拉高应答
		ucharFLAG=2;//标记         //ucharFLAG++是为了防止操作超时，因为char类型的
		while((!GPIO_INPUT_GET(DHT11_PIN_NUM))&&ucharFLAG++);//如果读取为1，取反后为0则退出循环，又或者标记函数ucharFLAG不为1时，也退出循环，可以进行下一步。否则一直等待什么时候为1拉高电平
		ucharFLAG=2;
		//再次等待拉底传输数据
		while(GPIO_INPUT_GET(DHT11_PIN_NUM)&&ucharFLAG++);//当拉低为0时，才能跳出这个循环。进行下一步采集数据（传输数据）
		for(i=0;i<8;i++)//循环8次是因为要读出8bit
		{
			//等待拉高
			ucharFLAG=2;
			while((!GPIO_INPUT_GET(DHT11_PIN_NUM))&&ucharFLAG++);//当为0跳出循环，里面为0，取反为1，是真，跳出循环。进行下一步。
			//延迟30us
			os_delay_us(28);//当延迟的是30us时，是0信号
			//赋值当前bit为0
			uchartemp=0;
			//如果继续为高电平，则赋值bit为1
			if(GPIO_INPUT_GET(DHT11_PIN_NUM))uchartemp=1;//1为70us，比0的30us大，所以当信号过了30us还是1时，就是1.
			//判断是否超时
			ucharFLAG=2;//因为ucharFLAG是uint8类型，
			while(GPIO_INPUT_GET(DHT11_PIN_NUM)&&ucharFLAG++);//此时还是0的话，就是超时了。
			//超时退出
			if(ucharFLAG==1)break;//上面++循环了255次之后，再加1就退出循环
			//左移一位，进行赋值
			ucharcomdata<<=1;//10
			ucharcomdata |=uchartemp;//10和当前为相与
		}
	ucharRH_data_H_humidity = ucharcomdata;//读取湿度整数数据的高八位，上面for循环8次，就获取了8位。
	for(i=0;i<8;i++)
	{
		ucharFLAG=2;
		while((!GPIO_INPUT_GET(DHT11_PIN_NUM))&&ucharFLAG++);
		os_delay_us(28);
		uchartemp=0;
		if(GPIO_INPUT_GET(DHT11_PIN_NUM))uchartemp=1;
		ucharFLAG=2;
		while(GPIO_INPUT_GET(DHT11_PIN_NUM)&&ucharFLAG++);
		if(ucharFLAG==1)break;
		ucharcomdata<<=1;
		ucharcomdata|=uchartemp;
	}
	ucharRH_data_L_humidity = ucharcomdata;//获取低8位，8bit湿度小数数据
	for(i=0;i<8;i++)//一共for循环4次，因为一次完整的数据传输为40Bit，高位先出。
					//1）8bit湿度整数数据+8bit湿度小数数据
					//2）8bit温度整数数据+8bit温度小数数据
					//每次都是读取8位
	{
		ucharFLAG=2;
		while((!GPIO_INPUT_GET(DHT11_PIN_NUM))&&ucharFLAG++);
		os_delay_us(28);
		uchartemp=0;
		if(GPIO_INPUT_GET(DHT11_PIN_NUM))uchartemp=1;
		ucharFLAG=2;
		while(GPIO_INPUT_GET(DHT11_PIN_NUM)&&ucharFLAG++);
		if(ucharFLAG==1)break;
		ucharcomdata<<=1;
		ucharcomdata|=uchartemp;
	}
	ucharT_data_H_temp = ucharcomdata;//获取高八位的8bit温度整数数据
	for(i=0;i<8;i++)
	{
		ucharFLAG=2;
		while((!GPIO_INPUT_GET(DHT11_PIN_NUM))&&ucharFLAG++);
		os_delay_us(28);
		uchartemp=0;
		if(GPIO_INPUT_GET(DHT11_PIN_NUM))uchartemp=1;
		ucharFLAG=2;
		while(GPIO_INPUT_GET(DHT11_PIN_NUM)&&ucharFLAG++);
		if(ucharFLAG==1)break;
		ucharcomdata<<=1;
		ucharcomdata|=uchartemp;
	}
	ucharT_data_L_temp = ucharcomdata;//获取低8bit温度小数数据
	for(i=0;i<8;i++)//这次for循环是因为8bit校验和
	{
		ucharFLAG=2;
		while((!GPIO_INPUT_GET(DHT11_PIN_NUM))&&ucharFLAG++);
		os_delay_us(28);
		uchartemp=0;
		if(GPIO_INPUT_GET(DHT11_PIN_NUM))uchartemp=1;
		ucharFLAG=2;
		while(GPIO_INPUT_GET(DHT11_PIN_NUM)&&ucharFLAG++);
		if(ucharFLAG==1)break;
		ucharcomdata<<=1;
		ucharcomdata|=uchartemp;
	}
	ucharcheckdata_temp = ucharcomdata;//校验和=8bit湿度整数数据+8bit湿度小数数据+8bi温度整数数据+8bit温度小数数据
	uchartemp=(ucharT_data_H_temp+ucharT_data_L_temp+ucharRH_data_H_humidity+ucharRH_data_L_humidity);

	if(uchartemp==ucharcheckdata_temp)//如果校验和相等，就赋值
	{
		ucharT_data_H = ucharT_data_H_temp;
		ucharT_data_L = ucharT_data_L_temp;
		ucharRH_data_H = ucharRH_data_H_humidity;
		ucharRH_data_L = ucharRH_data_L_humidity;
		ucharcheckdata = ucharcheckdata_temp;
	}
	}
	else//校验和不相等时，就赋值为0
		{
			ucharT_data_H = 0;
			ucharT_data_L = 0;
			ucharRH_data_H = 0;
			ucharRH_data_L = 0;
			}
	taskEXIT_CRITICAL();//退出临界段
}

/******************************************************************************
 * FunctionName : ATaskDht11
 * Description  :ATaskDht11 任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ATastDht11(void *pvParameters)
{
	for(;;)
	{
		vTaskDelay(500);
		DHT11_TEST();
		printf("ucharT_data_H = %d\n",ucharT_data_H);//因为不涉及小数，小数是为了扩展用的，所以此处只打印高8位
		printf("ucharRT_data_H = %d\n",ucharRH_data_H);
	}
	vTaskDelete(NULL);
}

/******************************************************************************
 * FunctionName : Dht11_init
 * Description  :Dht11_init 初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void Dht11_init(void)
{
	xTaskCreate(ATastDht11,"ATastDht11",256,NULL,4,NULL);
}

