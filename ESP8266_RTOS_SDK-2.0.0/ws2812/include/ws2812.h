#ifndef __WS2812_H__
#define __WS2012_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ws2812_PIN_NUM 5

#define COLOUR_NUM 7
typedef enum{
	red = 0,
	orange,
	yellow,
	green,
	blue,
	purple
}colours;

void Ws2812Write(colours colour);
void Ws2812_init(void);


#ifdef __cplusplus
}
#endif

#endif



