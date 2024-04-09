/****************************************************************************
 *	@笔者	：	Q
 *	@日期	：	2023年2月8日
 *	@所属	：	杭州友辉科技
 *	@功能	：	存放KEY按键相关的函数
 *	@函数列表:
 *	1.	void key_init(void)  -- 初始化key按键
 *	2.	u8 key_scan(u8 mode) -- 按键处理函数，返回按键值
 ****************************************************************************/

#include "y_key.h"

uint8_t key1_pressing = 0, key2_pressing = 0; /* 按键被确认按下后值为: 0-未按下 1-短按  2-长按 (短按记得把值需要手动清零，防止误判) */

static uint8_t key1_flag = 0, key2_flag = 0;              /* 标记按下状态，用来判断是长按还是短按 */
static uint16_t key1_press_time = 0, key2_press_time = 0; /* 记录按键按下的时间，按键消抖需要 */

/* 初始化key按键 */
void key_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK | KEY1_GPIO_CLK, ENABLE); // 使能端口时钟

    GPIO_InitStructure.GPIO_Pin = KEY1_PIN;       /* 配置 pin */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 设置成上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = KEY2_PIN; /* 配置 pin */
    GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);
}

/*********************函数说明*********************
函数作用：按键扫描函数
注意事项：每10ms被中断调用一次，判断是否有按键按下
         消抖时间：100ms
**********************************************/
void key_scan(void)
{
    if (KEY1() == 0)
    {
        key1_press_time++;
        if (key1_flag == 0) /* 防止重复处理,逻辑错误 */
        {
            // 50ms消抖后，确认需要处理
            if (key1_press_time > DITHER_ELIMINATION_TIME)
            {
                key1_flag = 1;
            }
        }
        if (key1_flag == 1)
        {
            // 如果连续按下，为长按
            if (key1_press_time > LONG_PRESS_TIME)
            {
                key1_flag = 2;
                key1_pressing = 2;
            }
        }
    }
    else
    {
        if (key1_flag == 1) /* 如果 key1_flag==1 ，说明是短按,keyx_pressi需要手动清零 */
        {
            key1_pressing = 1;
        }
        else
        {
            key1_pressing = 0;
        }
        // 如果没有被按下，定时器、pressing标志都清零。
        key1_flag = 0;
        key1_press_time = 0;
    }

    if (KEY2() == 0)
    {
        key2_press_time++;
        if (key2_flag == 0) /* 防止重复处理,逻辑错误 */
        {
            // 50ms消抖后，确认需要处理
            if (key2_press_time > DITHER_ELIMINATION_TIME)
            {
                key2_flag = 1;
            }
        }
        if (key2_flag == 1)
        {
            // 如果连续按下，为长按
            if (key2_press_time > LONG_PRESS_TIME)
            {
                key2_flag = 2;
                key2_pressing = 2;
            }
        }
    }
    else
    {
        if (key2_flag == 1) /* 如果 key1_flag==1 ，说明是短按 keyx_pressi需要手动清零*/
        {
            key2_pressing = 1;
        }
        else
        {
            key2_pressing = 0;
        }

        // 如果没有被按下，定时器、pressing标志都清零。
        key2_flag = 0;
        key2_press_time = 0;
    }
}
