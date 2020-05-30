#include "esp_common.h"
#include "gpio.h"

/******************************************************************************
 * FunctionName : Relay_Control
 * Description  : Relay_Control
 * Parameters   : uint32 mask
 * Returns      : none
*******************************************************************************/
void Relay_Control( uint32 mask)
{
	GPIO_OUTPUT(GPIO_Pin_5, mask);//调用了GPIO_OUTPUT驱动，用GPIO_Pin_5是GPIO5引脚，mask是放大参数0和1，1是打开，拉高。0是关闭。

}



