/****************************************************************************
 *	@笔者	：	Q
 *	@日期	：	2023年2月8日
 *	@所属	：	杭州友辉科技
 *	@功能	：	存放usart串口相关的函数
 ****************************************************************************/

#ifndef _Y_USART_H_
#define _Y_USART_H_
#include "main.h"

#define UART_BUF_SIZE 128 // 定义最大接收字节数 128

#define UART3_RX			GPIO_Pin_11
#define UART3_TX			GPIO_Pin_10
#define UART3_Port			GPIOB

extern u8 uart_receive_buf[UART_BUF_SIZE]; // 接收缓冲,最大UART_BUF_SIZE个字节.末字节为换行符
extern uint16_t uart1_get_ok;              // 接收完成标记

/*******串口相关函数声明*******/
void uart1_init(uint32_t BaudRate); /* 初始化串口1 */
void uart1_send_str(u8 *s);         /* Usart1发送字符串 */
void uart1_send_int(int tmp);       /*	串口1发送数字 */

void uart3_init(uint32_t BaudRate);
void uart3_send_byte(u8 dat);
void uart3_send_str(u8 *s);

#endif
