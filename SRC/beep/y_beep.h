/****************************************************************************
 *	@笔者	：	Q
 *	@日期	：	2023年2月8日
 *	@所属	：	杭州友辉科技
 *	@功能	：	存放蜂鸣器相关的函数
 ****************************************************************************/

#ifndef _Y_BEEP_H_
#define _Y_BEEP_H_
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "main.h"

/* 定义BEEP引脚，修改编号就可以修改BEEP引脚 */
#define BEEP_PIN GPIO_Pin_5
#define BEEP_GPIO_PORT GPIOB               /* GPIO端口 */
#define BEEP_GPIO_CLK RCC_APB2Periph_GPIOB /* GPIO端口时钟 */

/* 控制BEEP的宏 */
#define BEEP_GET_LEVEL() GPIO_ReadOutputDataBit(BEEP_GPIO_PORT, BEEP_PIN)                        // 读取BEEP蜂鸣器状态
#define BEEP_OFF() GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_PIN)                                      // BEEP蜂鸣器关闭
#define BEEP_ON() GPIO_SetBits(BEEP_GPIO_PORT, BEEP_PIN)                                         // BEEP蜂鸣器打开
#define BEEP_TOGGLE() GPIO_WriteBit(BEEP_GPIO_PORT, BEEP_PIN, (BitAction)(1 - BEEP_GET_LEVEL())) // 翻转BEEP蜂鸣器

/*******BEEP相关函数声明*******/
void beep_init(void); // 初始化BEEP蜂鸣器
void beep_on_times(uint16_t times);
#endif
