//#include "main.h" /* 包含各类驱动文件 */

//void SWJ_gpio_init(void); /* SWJ引脚配置 */

//int main(void)
//{
//    rcc_init();      /* 时钟初始化 */
//    SysTick_Init();  /* 初始化系统嘀答定时器，1ms定时一次 */
//    // SWJ_gpio_init(); /* 禁用(JTAG-DP + SW-DP) */

//    app_gpio_init(); /* 初始化gpio相关引脚 */
//    app_uart_init(); /*  初始化相关串口 */

//    app_setup_start(); /* 应用程序开始 */

//    while (1)
//    {
//        app_led_run(); /* 循环执行工作指示灯 */
//        app_key_run(); /* 循环运行监测按键状态 */
//        app_uart_run();/* 串口应用循环运行 */
//    }
//}

///* SWJ引脚配置 */
//void SWJ_gpio_init(void)
//{
//    /**********************
//    1.执行端口重映射时,复用功能时钟得使能:RCC_APB2Periph_AFIO

//    2.  &1.GPIO_Remap_SWJ_Disable: !< Full SWJ Disabled (JTAG-DP + SW-DP)
//         此时PA13|PA14|PA15|PB3|PB4都可作为普通IO用了
//       为了保存某些调试端口,GPIO_Remap_SWJ_Disable也可选择为下面两种模式：

//        &2.GPIO_Remap_SWJ_JTAGDisable: !< JTAG-DP Disabled and SW-DP Enabled
//        此时PA15|PB3|PB4可作为普通IO用了

//        &3.GPIO_Remap_SWJ_NoJTRST: !< Full SWJ Enabled (JTAG-DP + SW-DP) but without JTRST
//        此时只有PB4可作为普通IO用了
//    **********************/
//    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // 使能 PA 端口时钟
//    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);                                               // 使能禁止JTAG和SW-DP
//    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // 使能禁止JTAG
//}

#include "main.h" /* 包含各类驱动文件 */

typedef struct {
    uint8_t header_1;
    uint8_t header_2;
    
	// union {
	struct {
		uint8_t servo_id;
		uint8_t length;
		uint8_t command;
		uint8_t args[8];
	} elements;
	// uint8_t data_raw[11];
} SerialServoCmdTypeDef;

// 计算校验码
static uint8_t serial_servo_checksum(const uint8_t buf[])
{
    uint16_t temp = 0;
    for (int i = 2; i < buf[3] + 2; ++i) {
        temp += buf[i];
    }
    return (uint8_t)(~temp);
}


void SWJ_gpio_init(void); /* SWJ引脚配置 */

int main(void)
{
    rcc_init();      /* 时钟初始化 */
    SysTick_Init();  /* 初始化系统嘀答定时器，1ms定时一次 */
    // SWJ_gpio_init(); /* 禁用(JTAG-DP + SW-DP) */

    app_gpio_init(); /* 初始化gpio相关引脚 */
    app_uart_init(); /*  初始化相关串口 */

	uart1_init(115200);
	uart3_init(115200);
	
	app_setup_start(); 
	
	SerialServoCmdTypeDef tx_fram;
	tx_fram.header_1 = 0x55;
	tx_fram.header_2 = 0x55;
	
	tx_fram.elements.command = 14;
	tx_fram.elements.length = 3;
	tx_fram.elements.servo_id = 0xFE;
	tx_fram.elements.args[0] = serial_servo_checksum((uint8_t *)&tx_fram);
	// printf("send...");
	
	uart1_send_str((u8 *)&tx_fram);
	

	
	/**********************************************
	* @总线设备基本控制指令表：
	*	1.	#000PID!						//读取ID指令
	*	2.	#000PID001!						//设置ID指令
	*	3.	#000PVER!						//读取版本
	*	4.	#000PBD1!						//设置波特率 默认115200  1:9600 , 2:19200 , 3:38400 , 4:57600 , 5:115200 , 6:128000 7:256000  8:1000000
	*	5.	#000PCLE!						//恢复出厂设置包括ID
	* @总线马达ZMotor控制指令表：
	*	1.	#000P1500T1000!					//电机速度控制
	**********************************************/
//	uart3_send_str((u8 *)"#255PID000!");
//	delay_ms(500);
//	uart3_send_str((u8 *)"#000PID!");
//	delay_ms(500);
//	uart3_send_str((u8 *)"#000P2500T3000!");
//	delay_ms(5000);
//	uart3_send_str((u8 *)"#000P0500T3000!");
//	delay_ms(5000);
//	uart3_send_str((u8 *)"#000P1500T1000!");
	
	/* 应用程序开始 */

    while (1)
    {
        // app_led_run(); /* 循环执行工作指示灯 */
        // app_key_run(); /* 循环运行监测按键状态 */
        // app_uart_run();/* 串口应用循环运行 */
		uart3_send_str((u8 *)&tx_fram);
		delay_ms(1000);
    }
}

/* SWJ引脚配置 */
void SWJ_gpio_init(void)
{
    /**********************
    1.执行端口重映射时,复用功能时钟得使能:RCC_APB2Periph_AFIO

    2.  &1.GPIO_Remap_SWJ_Disable: !< Full SWJ Disabled (JTAG-DP + SW-DP)
         此时PA13|PA14|PA15|PB3|PB4都可作为普通IO用了
       为了保存某些调试端口,GPIO_Remap_SWJ_Disable也可选择为下面两种模式：

        &2.GPIO_Remap_SWJ_JTAGDisable: !< JTAG-DP Disabled and SW-DP Enabled
        此时PA15|PB3|PB4可作为普通IO用了

        &3.GPIO_Remap_SWJ_NoJTRST: !< Full SWJ Enabled (JTAG-DP + SW-DP) but without JTRST
        此时只有PB4可作为普通IO用了
    **********************/
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // 使能 PA 端口时钟
    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);                                               // 使能禁止JTAG和SW-DP
    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // 使能禁止JTAG
}

