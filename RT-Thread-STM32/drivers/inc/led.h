/*
 * File      : led.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

#ifndef __LED_H__
#define __LED_H__
#include <stdbool.h>
#include <rtthread.h>
//#include "stm32f10x.h"

//Led polarity configuration constant
#define LED_POL_POS 0		//极性设置 0点亮
#define LED_POL_NEG 1

//Hardware configuration
#define LED_GPIO_PERIF		RCC_APB2Periph_GPIOC
#define LED_GPIO_PORT		GPIOC

#define LED_REC_GPIO_PERIF	RCC_APB2Periph_GPIOB	
#define LED_REC_GPIO_PORT	GPIOB

#define LED_GPIO_RED		GPIO_Pin_8	//LED1	PC8
#define LED_POL_RED			LED_POL_NEG

#define LED_GPIO_GREEN		GPIO_Pin_9	//LED2	PC9
#define LED_POL_GREEN		LED_POL_NEG

#define LED_GPIO_NET		GPIO_Pin_7	//LED3	PC7
#define LED_POL_NET			LED_POL_NEG

#define LED_GPIO_SNP		GPIO_Pin_6	//LED4	PC6
#define LED_POL_SNP			LED_POL_NEG

#define BEEP_GPIO			GPIO_Pin_0	//LED4	PB15
#define BEEP_POL			LED_POL_NEG

#define LED_GPIO_REC		GPIO_Pin_15	//LED4	PB15
#define LED_POL_REC			LED_POL_NEG

#define LED_NUM 6

typedef enum {LED_RED = 0, LED_GREEN, LED_NET, LED_SNP, LED_REC, BEEP} led_t;//枚举定义

void ledSet(led_t led, bool value);
bool ledTest(void);

//Legacy functions
#define ledSetRed(VALUE) 	ledSet(LED_RED,    VALUE)
#define ledSetGreen(VALUE) 	ledSet(LED_GREEN,  VALUE)
#define ledSetBlue(VALUE) 	ledSet(LED_BLUE,   VALUE)
#define ledSetYellow(VALUE)	ledSet(LED_YELLOW, VALUE)

void rt_hw_led_init(void);
void rt_hw_led_on(rt_uint32_t led);
void rt_hw_led_off(rt_uint32_t led);

#endif
