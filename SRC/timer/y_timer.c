/****************************************************************************
 *	@笔者	：	Q
 *	@日期	：	2023年2月8日
 *	@所属	：	杭州友辉科技
 *	@功能	：	存放定时器相关的函数
 *	@函数列表:
 *	1.	void systick_init(void) -- 初始化systick
 *	2.	void SysTick_Handler(void) interrupt 19 -- systick中断函数
 *	3.	u32 millis(void) -- 滴答时钟查询
 *	4.	void TIM2_init(void) -- 初始化TIM2
 *	5.	void TIM2_IRQHandler(void) -- 输出舵机控制波形
 ****************************************************************************/
#include "y_timer.h"

static u32 systick_ms = 0; /* 记录时间 */

/* 初始化systick */
void SysTick_Init(void) // 1毫秒@72MHz
{
	SysTick_Config(SystemCoreClock / 1000);
}

/* SysTick中断 */
void SysTick_Handler(void)
{
	static uint8_t key_scan_times = 0;
	systick_ms++;

	/* 10ms检查一次按键状态 */
	key_scan_times++;
	if (key_scan_times >= 10)
	{
		key_scan_times = 0;
		key_scan();
	}
}

/* 获取滴答时钟数值 */
u32 millis(void)
{
	return systick_ms;
}
