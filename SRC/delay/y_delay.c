#include "y_delay.h"

void delay_ns(u16 t)
{
   while (t--)
      ;
   return;
}

void delay_us(u16 t)
{
   u16 i=0;  
	while(t--) {
		i=10; 
		while(i--) ;    
	}
}

void delay_ms(u16 t)
{
   u32 systick_temp = millis(); /* 获取当前，用作比较值 */

   /* SysTick计数超过设定值结束循环 */
   while (t > (millis() - systick_temp))
   {
      if (systick_temp > millis()) /* 时间戳变量溢出错误 */
      {
         /* 用滴答时钟数值溢出前的最大数值减去比较值，得到滴答时钟数值溢出前记数数量，
            在用设置的ms值减去记数数量 */
         t = t - (0xffffffff - systick_temp);
         systick_temp = 0;
      }
   }
}
