#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gpio.h"
#include "ws2812.h"

//GRB格式
uint32 ColourArray[COLOUR_NUM] = {
	0x00FF00,
	0xA5FF00,
	0xFFFF00,
	0x000000,
	0xFF00FF,
	0x0000FF,
	0x800080
	};
static volatile uint8_t time;//volatile是防止编译器优化，给他赋值，就不会操作time++

void delay100Ns(void)
{
	time ++;//此处是用示波器一点点测试波形才确定的延迟,大概400ns,350ns
}

/******************************************************************************
 * FunctionName : SEND_WS_0
 * Description  : 写0码
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void SEND_WS_0(void)//发送0码
{
	GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, GPIO_Pin_5);//用这个接口是因为只有它是ns级的，0码是先拉高电平350ns
	GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, 0);//再给低的寄存器赋值0

	GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, 0);//把高位寄存器拉低
	GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, GPIO_Pin_5);//前面参数是寄存器的起始地址，后面是偏移地址，写入了IO端口号
										//把低位寄存器拉高，赋值1，输出低电平,800ns
	delay100Ns();//延迟是调好的，不作解释。
	delay100Ns();
	delay100Ns();
}

/******************************************************************************
 * FunctionName : SEND_WS_1
 * Description  : 写1码
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void SEND_WS_1(void)
{
	GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, GPIO_Pin_5);//拉高
	GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, 0);//700ns
	delay100Ns();//大概100多
	delay100Ns();
	GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, 0);//把高位寄存器拉低
	GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, GPIO_Pin_5);//600ns
	delay100Ns();
}

/******************************************************************************
 * FunctionName : Ws2812REST
 * Description  : 帧切换（复位）
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void Ws2812REST(void)
{
	GPIO_OUTPUT(GPIO_Pin_5,0);//直接调用接口，置0
	os_delay_us(50);
}

/******************************************************************************
 * FunctionName : Ws2812Write
 * Description  : 写入色彩数据
 * Parameters   : 色彩编码
 * Returns      : none
*******************************************************************************/
void Ws2812Write(colours colour)
{
	uint32 mask;
	uint8 i;
	taskENTER_CRITICAL();//进入临界段
	Ws2812REST();//复位
	for(i=0;i<7;i++)//循环操作7个灯泡
	{
		mask = 0x800000;//从高位开始发送24bit
		while(mask)
		{
			if(ColourArray[colour]&mask)
			SEND_WS_1();
			else
			SEND_WS_0();
			mask >>=1;//右移，因为是从高位读向低位
		}
	}
	taskEXIT_CRITICAL();
}

/******************************************************************************
 * FunctionName : Ws2812_init
 * Description  : Ws2812_init 初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void Ws2812_init(void)
{
	GPIO_AS_OUTPUT(GPIO_Pin_5);
	
}





