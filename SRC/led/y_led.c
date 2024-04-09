/****************************************************************************
 *	@笔者	：	Q
 *	@日期	：	2023年2月8日
 *	@所属	：	杭州友辉科技
 *	@功能	：	存放led相关的函数
 *	@函数列表:
 *	1.	void led_init(void) -- 初始化LED信号灯
 ****************************************************************************/

#include "y_led.h"

/* 初始化LED信号灯 */
void led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE); /* 使能端口时钟 */

    GPIO_InitStructure.GPIO_Pin = LED_PIN;            /* 配置 pin */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  /* 推挽输出 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; /* IO 翻转 50MHz */
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
}
