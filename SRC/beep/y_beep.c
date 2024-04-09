/****************************************************************************
 *	@笔者	：	Q
 *	@日期	：	2023年2月8日
 *	@所属	：	杭州友辉科技
 *	@功能	：	存放蜂鸣器相关的函数
 *	@函数列表:
 *	1.	void beep_init(void) -- 初始化蜂鸣器BEEP
 ****************************************************************************/

#include "y_beep.h"

/* 初始化蜂鸣器BEEP */
void beep_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(BEEP_GPIO_CLK, ENABLE); /* 使能端口时钟 */

    GPIO_InitStructure.GPIO_Pin = BEEP_PIN;           /* 配置 pin */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  /* 推挽输出 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; /* IO 翻转 50MHz */
    GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @函数描述: 蜂鸣器鸣叫时间，单位ms
 * @param {uint16_t} times
 * @return {*}
 */
void beep_on_times(uint16_t times)
{
    BEEP_ON();
    delay_ms(times);
    BEEP_OFF();
}

