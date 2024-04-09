/****************************************************************************
 *	@笔者	：	Q
 *	@日期	：	2023年2月8日
 *	@所属	：	杭州友辉科技
 *	@功能	：	存放KEY按键相关的函数
 ****************************************************************************/

#ifndef _Y_KEY_H_
#define _Y_KEY_H_
#include "main.h"

// Key scan time, based on 10ms
#define KEY_50MS 5
#define KEY_100MS 10
#define KEY_200MS 20
#define KEY_500MS 50
#define KEY_1S 100
#define KEY_2S 200
#define KEY_3S 300

#define DITHER_ELIMINATION_TIME KEY_50MS /* 消抖时间 */
#define LONG_PRESS_TIME KEY_2S            /* 长按时间 */

/* 定义KEY引脚，修改编号就可以修改KEY引脚 */
#define KEY1_PIN GPIO_Pin_8
#define KEY1_GPIO_PORT GPIOA               /* GPIO端口 */
#define KEY1_GPIO_CLK RCC_APB2Periph_GPIOA /* GPIO端口时钟 */

#define KEY2_PIN GPIO_Pin_11
#define KEY2_GPIO_PORT GPIOA               /* GPIO端口 */
#define KEY2_GPIO_CLK RCC_APB2Periph_GPIOA /* GPIO端口时钟 */

/* 控制KEY按键的宏 */
#define KEY1() GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_PIN) // 读取按键0
#define KEY2() GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_PIN) // 读取按键1

extern uint8_t key1_pressing, key2_pressing;

/*******按键相关函数声明*******/
void key_gpio_init(void); // 初始化key按键
void key_scan(void);      /* 按键处理函数，返回按键值 */
#endif
