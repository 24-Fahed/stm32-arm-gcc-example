/****************************************************************************
 *	@笔者	：	Q
 *	@日期	：	2023年2月8日
 *	@所属	：	杭州友辉科技
 *	@功能	：	存放led相关的函数
 ****************************************************************************/

#ifndef _Y_LED_H_
#define _Y_LED_H_
#include "main.h"

/* 定义led引脚，修改编号就可以修改led引脚 */
#define LED_PIN GPIO_Pin_13

#ifdef USER_BOARD_SUPPORT
#define LED_GPIO_PORT GPIOC					// 使用非官方的开发板
#define LED_GPIO_CLK RCC_APB2Periph_GPIOC
#else
#define LED_GPIO_PORT GPIOB               /* GPIO端口 */
#define LED_GPIO_CLK RCC_APB2Periph_GPIOB /* GPIO端口时钟 */
#endif



/* 控制LED的宏 */
#define LED_GET_LEVEL() GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_PIN)                       // 读取LED信号灯状态
#define LED_ON() GPIO_ResetBits(LED_GPIO_PORT, LED_PIN)                                  // LED信号灯点亮
#define LED_OFF() GPIO_SetBits(LED_GPIO_PORT, LED_PIN)                                   // LED信号灯熄灭
#define LED_TOGGLE() GPIO_WriteBit(LED_GPIO_PORT, LED_PIN, (BitAction)(1 - LED_GET_LEVEL())) // 翻转LED信号灯

/*******LED相关函数声明*******/
void led_init(void); // 初始化LED信号灯
#endif
