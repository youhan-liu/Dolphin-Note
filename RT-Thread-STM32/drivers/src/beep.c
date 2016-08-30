#include <stdbool.h>
#include <rtthread.h>
#include <stm32f10x.h>
#include "beep.h"

#define BEEP_GPIO_PERIF		RCC_APB2Periph_GPIOB	
#define BEEP_GPIO_PORT		GPIOB
#define GPIO_BEEP			GPIO_Pin_1	

static rt_timer_t timer_beep_freq;
static rt_timer_t timer_beep;
static void run_beep(void * parameter);
static void beep_freq(void * parameter);

void rt_hw_beep_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(BEEP_GPIO_PERIF, ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin   = GPIO_BEEP;
    GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);	
	
	
	timer_beep_freq = rt_timer_create("beepfreqTimer", 			
										beep_freq,				
										RT_NULL,	/*超时函数入口参数*/
										1, 					/*定时长度*/
										RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);/*定时方式为周期性定时  软件定时*/
	timer_beep = rt_timer_create("beepTimer", 			
									run_beep,				
									RT_NULL,	/*超时函数入口参数*/
									1000, 					/*定时长度*/
									RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);/*定时方式为周期性定时  软件定时*/
//	rt_timer_start(timer_beep_freq);
}

void beep_set(bool value)
{
	if(!value)
		GPIO_SetBits(BEEP_GPIO_PORT, GPIO_BEEP);
	else
		GPIO_ResetBits(BEEP_GPIO_PORT, GPIO_BEEP); 
}

static void beep_freq(void * parameter)
{
	static unsigned char i = 0;
	i++;
	i%=2;
	beep_set(i);
}

static void run_beep(void * parameter)
{
	rt_timer_stop(timer_beep_freq);
}

void beep(void)
{
	rt_timer_start(timer_beep_freq);
	rt_timer_start(timer_beep);
}



