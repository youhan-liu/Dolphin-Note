#include "key.h"
#include "usart2.h"
#include "cc3200.h"

u8 WIFI_Socket_Flag = 0;    //网络状态变量 0->都未连接，1->WiFi连接、手机未连接，2->WiFi连接、手机连接
u8 SNP_STAT_Flag 	= 0;       //拍照网络变量 0->不可拍照, 1->可以拍照 2->拍照中
u8 REC_STAT_Flag 	= 0;       //录像网络变量 0->不可录制  1->可以录制 2->录制中
u8 STAT_STAT_Flag 	= 0;      //硬件运行状态变量    0->正常 1->硬件错误 2->队列容量满 3->WIFI进入AP模式
u8 BEEP_STAT_Flag 	= 0;      //队列状态变量 0->不响 1-> 响1声 2->响2声 3->长响3秒
u8 ERR_Status 		= 0;          //硬件错误  00->正常 D1-> 上次队列中有残留数据 E1->数据队列溢出 E2->触屏工作不正常 E4->数据缓存不正常 E8->外部电压不足

#define T_NET	1
#define T_SNP 2
#define T_REC 4

#define N_key	0		// 无键
#define S_key	1		// 单键
#define L_key 	2		// 长按

#define key_state_0	0		// 按键未按下
#define key_state_1	1		// 按键消抖与确认
#define key_state_2	2		// 按键未释放
#define key_state_3	3		// 等待按键释放


void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
//	TIM7_Int_Init(1000-1,7200-1);		//10ms中断
}
/*******************************************************************************
* 函数名  : key_scan
* 描述    : 按键扫描
* 返回值  : 0：无按键; 
* 说明    : 
*******************************************************************************/
u8 KeyRead(void)
{
	u8 Trg=0;
	u8 Cont=0;
	u8 status;
	u8 ReadData;

		ReadData = ((GPIO_ReadInputData(GPIOB) & 0x7000) >> 12) ^ 0x07;
		Trg = ReadData & (ReadData ^ Cont);
		Cont = ReadData;
	
		rt_thread_delay(300);
		status = Trg + ReadData;
	
	return status;
}
/*******************************************************************************
* 函数名  : key_scan
* 描述    : 按键扫描
* 返回值  : 0：无按键; 
*						0x12: T_REC 短按; 0x13: T_REC 长按; 
*						0x22: T_SNP 短按; 0x23: T_SNP 长按; 
*						0x32: T_NET 短按; 0x33: T_NET 长按; 
* 说明    : wait_time：连接等待时间; over_time: 配置连接超时时间，单位 1S
*******************************************************************************/
u8 key_scan(void)
{
	u8 key_value;
	u8 key_flag;
	u8 key_press;
	u8 key_return;
	u8 i;
	key_flag = 0;

	key_value = KeyRead();
	if(key_value !=0 )
	{
		key_press = ((key_value>>2) ^ 0x03)<<4;
		for(i=0;i<10;i++)
		{			
			rt_thread_delay(1);
			key_return = KeyRead();
			if(key_return == 0)
			{
				key_flag = key_press | 0x02;
				break;	
			}
			if(i == 7)
			{
				key_flag = key_press | 0x03;
				break;
			}			
		}
		return key_flag;
	}
	else
		return key_flag;
}

void key_state_function(unsigned char _key)
{
	static unsigned char key_state = 0;
	static unsigned char key_num = 0;
	switch(key_state)
	{
		case 0:
			
			break;
		case 1:
			break;
		case 2:
			break;
		
	}
}













