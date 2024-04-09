/****************************************************************************
 *	@笔者	：	Q
 *	@日期	：	2023年2月8日
 *	@所属	：	杭州友辉科技
 *	@功能	：	存放时钟配置相关的函数
 *	@函数列表:
 *	1.	void rcc_init(void) -- 初始化时钟
 ****************************************************************************/
#include "y_rcc.h"

/* 初始化时钟 */
void rcc_init(void)
{
    //--------------------------- CLK INIT, HSE PLL ----------------------------
    ErrorStatus HSEStartUpStatus;
    // RCC reset
    RCC_DeInit();
    // 开启外部时钟 并执行初始化
    RCC_HSEConfig(RCC_HSE_ON);
    // 等待外部时钟准备好
    HSEStartUpStatus = RCC_WaitForHSEStartUp();
    // 启动失败 在这里等待
    while (HSEStartUpStatus == ERROR)
        ;
    // 设置内部总线时钟
    RCC_HCLKConfig(RCC_SYSCLK_Div1); // SYSCLK
    RCC_PCLK1Config(RCC_HCLK_Div2);  // APB1  MAX = 36M
    RCC_PCLK2Config(RCC_HCLK_Div1);  // APB2  MAX = 72M
    // 外部时钟为8M 这里倍频到72M
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    RCC_PLLCmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        ;
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while (RCC_GetSYSCLKSource() != 0x08)
        ;
    return;
}
