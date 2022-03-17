#include "delay.h"
#include "sys.h"


extern volatile uint32_t timer_tick_count;
	   						   
/*!
* ��    �ƣ� delay_ms
* ��    �ܣ� ��ʱn����
* ��ڲ����� n-��ʱʱ��
* ���ڲ����� ��
*/
void delay_ms(u32 delay)
{
    u32 tick = timer_tick_count;
    while(1)
    {
        if(timer_tick_count-tick>delay/10)
            break;
    }
}
/*!
*��    �ƣ� systicket_init
*��    �� �����Ķ�ʱ�� 10ms
*��ڲ����� ��
*���ڲ����� ��
*/
void systicket_init(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    SysTick_Config(168000000/100);                              //һ���ӽ���100���ж�
}
	




























