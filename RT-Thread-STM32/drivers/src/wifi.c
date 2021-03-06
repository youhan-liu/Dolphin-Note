//#include "sys.h"
//#include "delay.h"
#include "usart.h"
#include "usart2.h"
#include "wifi.h"
#include "Screen.h"
#include <string.h>
#include <stdlib.h>
#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
	
unsigned char wifi_data_to_send[255] = {0};
unsigned char Phone_ID[16] = {0};
unsigned char Board_ID[2] = {0};
unsigned char Phone_IP[4] = {0};
unsigned char Board_IP[4] = {0};
unsigned char Bar_code[10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
unsigned char wifi_data_len = 0;
unsigned char is_First_connect = 1;
//void insert(void *a,void *ins,int index,size_t n,size_t width)
//{
//	char *tmp = malloc(width * (n - index - 1));
//	memcpy(tmp, (char *)a + (index * width), width * (n - index - 1));
//	memcpy((char *)a + index * width, ins, width);
//	memcpy((char *)a + width * (index + 1), tmp, width * (n - index - 1));
//	free(tmp);
//}
void wifi_init()
{
	
}
void wifi_send_pen_data(unsigned char * data_buffer, unsigned char pen_cnt)
{
	unsigned char ii = 0;
	unsigned int num = 0;
	unsigned char sum = 0;
//	unsigned char *p = (wifi_data_to_send + 7);
//	p = wifi_data_to_send + 7;
	wifi_data_len = pen_cnt;
	num = (pen_cnt) * 12;
	wifi_data_to_send[0] = 0xFA;
	wifi_data_to_send[1] = 0x0A;
	wifi_data_to_send[2] = 0xDD;
	wifi_data_to_send[3] = Phone_ID[14];
	wifi_data_to_send[4] = Phone_ID[15];
	wifi_data_to_send[5] = BYTE0(num);
	wifi_data_to_send[6] = BYTE1(num);
//	*wifi_data_to_send += 7;
	
	for(ii = 0; ii < pen_cnt; ii ++)
	{
		memcpy(&(wifi_data_to_send [7 + ii*12]), data_buffer + ii*12, 12);
	}
	for(ii = 0; ii < (num + 7); ii ++)
	{
		sum += wifi_data_to_send[ii];
	}
	wifi_data_to_send[7 + num] = sum;
	wifi_data_to_send[8 + num] = 0xED;
//	rt_sem_take(&Screen_seqSem, RT_WAITING_FOREVER);
	rt_sem_release(&Screen_seqSem);	
	
}
void task_wifi()
{
//	if(screen_data_ready)
//	{
//		screen_data_ready = 0;
////		wifi_send_data(unsigned char * data_buffer, unsigned char pen_cnt);
//	}
}
//------------------------------------
void wifi_ack_send(unsigned char code, unsigned short para) //发送应答信息指令0x77,0x76,0xB1,0xB2 (code),para 为0x77的参数
{
	unsigned char i,sum;
//	unsigned char wifi_send_len = 0;
	wifi_data_to_send[0] = 0xFA;
	wifi_data_to_send[1] = 0x0A;
	wifi_data_to_send[2] = code;
	wifi_data_to_send[3] = Phone_ID[14];
	wifi_data_to_send[4] = Phone_ID[15];
	
	switch(code)
	{
		case(0x77):
			//普通状态应答
			wifi_data_to_send[5] = 0x02;
			wifi_data_to_send[6] = 0x00;
			wifi_data_to_send[7]=(para>>8);
			wifi_data_to_send[8]=(para&0xFF);
			sum = 0;
			for(i = 0; i < 8; i ++)
			{
				sum += wifi_data_to_send[i];
			}	
			wifi_data_to_send[9] = sum;
			wifi_data_to_send[10] = 0xED;	
			wifi_data_len=11;	//发送字符串长度	
			break;
		
		case(0x76):
			wifi_data_to_send[5] = 0x08;
			wifi_data_to_send[6] = 0x00;
	
			wifi_data_to_send[7] = Board_IP[0];
			wifi_data_to_send[8] = Board_IP[1];
			wifi_data_to_send[9] = Board_IP[2];
			wifi_data_to_send[10] = Board_IP[3];

			wifi_data_to_send[11] = Board_ID[0];
			wifi_data_to_send[12] = Board_ID[1];
			wifi_data_to_send[13] = Phone_ID[14];
			wifi_data_to_send[14] = Phone_ID[15];
			sum = 0;
			for(i = 0; i < 15; i ++)
			{
				sum += wifi_data_to_send[i];
			}	
			wifi_data_to_send[15] = sum;
			wifi_data_to_send[16] = 0xED;		
			wifi_data_len=17;	//发送字符串长度
			break;
		case(0x78):
			wifi_data_to_send[5] = 0x08;
			wifi_data_to_send[6] = 0x00;
	
			wifi_data_to_send[7] = Board_IP[0];
			wifi_data_to_send[8] = Board_IP[1];
			wifi_data_to_send[9] = Board_IP[2];
			wifi_data_to_send[10] = Board_IP[3];

			wifi_data_to_send[11] = Board_ID[0];
			wifi_data_to_send[12] = Board_ID[1];
			wifi_data_to_send[13] = Phone_ID[14];
			wifi_data_to_send[14] = Phone_ID[15];
			sum = 0;
			for(i = 0; i < 15; i ++)
			{
				sum += wifi_data_to_send[i];
			}	
			wifi_data_to_send[15] = sum;
			wifi_data_to_send[16] = 0xED;		
			wifi_data_len=17;	//发送字符串长度
			break;
		case 0xA1:
			//普通状态应答
			wifi_data_to_send[5] = 0x02;
			wifi_data_to_send[6] = 0x00;
			wifi_data_to_send[7]=(para>>8);
			wifi_data_to_send[8]=(para&0xFF);
			sum = 0;
			for(i = 0; i < 8; i ++)
			{
				sum += wifi_data_to_send[i];
			}	
			wifi_data_to_send[9] = sum;
			wifi_data_to_send[10] = 0xED;	
			wifi_data_len=11;	//发送字符串长度	
			//固件版本号
			break;
		case 0x70:
			//普通状态应答
			wifi_data_to_send[5] = 0x02;
			wifi_data_to_send[6] = 0x00;
			wifi_data_to_send[7]=(para>>8);
			wifi_data_to_send[8]=(para&0xFF);
			sum = 0;
			for(i = 0; i < 8; i ++)
			{
				sum += wifi_data_to_send[i];
			}	
			wifi_data_to_send[9] = sum;
			wifi_data_to_send[10] = 0xED;	
			wifi_data_len=11;	//发送字符串长度	
			//固件版本号
			break;
		case 0xB1:
		
			//固件版本号
			break;
		
		case 0xB2:
		
			//触屏信息
			break;
		case 0x74:
			wifi_data_to_send[5] = 0x04;
			wifi_data_to_send[6] = 0x00;
	
			wifi_data_to_send[7] = 0;
			wifi_data_to_send[8] = 0;
			wifi_data_to_send[9] = 0;
			wifi_data_to_send[10] = 0;

			sum = 0;
			for(i = 0; i < 11; i ++)
			{
				sum += wifi_data_to_send[i];
			}	
			wifi_data_to_send[11] = sum;
			wifi_data_to_send[12] = 0xED;		
			wifi_data_len = 13;	//发送字符串长度			
			//触屏信息
			break;
	}
//	is_wifisent_flag=2;//此信息为应答信息,不需要手机应答
//	Uart2_Put_Buf(wifi_data_to_send , wifi_send_len);
}
void wifi_data_deal(unsigned char *data_buf, unsigned char num)
{
	
	vs16 rc_value_temp;
	unsigned char sum = 0;
	unsigned char i = 0;
	unsigned char Bar_code_err_flag = 0;
	unsigned char Phone_ID_err_flag = 0;
	
//	unsigned char point_cnt = 0;//触摸点计数

	if(!(*(data_buf)==0xFA && *(data_buf + 1)==0x0A))	
	{	
		return;
	}		//判断帧头
/////////////////////////////////////////////////////////////////////////////////////
	switch(*(data_buf + 2))
	{				//第一次连接的时候，需要判断二维码是否与存储的二维码相同，如果相同则连接；
					//后续的断线重连，需要判断手机ID是否与第一次连接时存储的相同，如果相同的话，继续判断手机IP是否改变，
		case 0x75:	//如果改变则需要重新配置UDPC的IP参数
			for(i = 0; i < 10; i++)//任何情况下收到连接请求都需要判断二维码是否正确
			{
				if(*(data_buf + 27 + i) != Bar_code[i])
				{
					Bar_code_err_flag = 1;
					break;
				}
			}
			if(Bar_code_err_flag == 1)//二维码错误则不响应连接请求
				net_complete_flag = 0;
			else//二维码正确，首先，如果是第一次连接，存储接收到的手机IP，进行网络设置。
			{	//如果不是第一次连接，则需要比对手机ID是否正确
				//手机IP
				if(is_First_connect)
				{
					is_First_connect = 0;
					//if()
					Phone_IP[0]	= *(data_buf + 7);
					Phone_IP[1]	= *(data_buf + 8);
					Phone_IP[2]	= *(data_buf + 9);
					Phone_IP[3]	= *(data_buf + 10);
					//手机ID
					for(i = 0; i < 16; i ++)
					{
						Phone_ID[i] = *(data_buf + 11 + i);
						//Phone_ID[1] = *(data_buf + 26);
					}
					//白板ID
					Board_ID[0] = *(data_buf + 35);
					Board_ID[1] = *(data_buf + 36);
					wifi_ack_send(0x76, 0);
				}
				else
				{
					for(i = 0; i < 16; i++)//判断手机ID是否正确
					{
						if(*(data_buf + 11 + i) != Phone_ID[i])
						{
							Phone_ID_err_flag = 1;
							break;
						}
					}	
					if(Phone_ID_err_flag == 1)//手机ID错误则不响应
					{	
					}
					else
					{
						Phone_IP[0]	= *(data_buf + 7);
						Phone_IP[1]	= *(data_buf + 8);
						Phone_IP[2]	= *(data_buf + 9);
						Phone_IP[3]	= *(data_buf + 10);
						net_reconnect_flag = 1;
						wifi_ack_send(0x76, 0);
					}
				}
			}
			break;
		case 0x78://心跳包
			for(i = 0; i < 10; i++)//任何情况下收到连接请求都需要判断二维码是否正确
			{
				if(*(data_buf + 27 + i) != Bar_code[i])
				{
					Bar_code_err_flag = 1;
					break;
				}
			}
			if(Bar_code_err_flag == 1)//二维码错误则不响应连接请求
				searching_flag = 0;
			else
			{
//				//if()
//				Phone_IP[0]	= *(data_buf + 7);
//				Phone_IP[1]	= *(data_buf + 8);
//				Phone_IP[2]	= *(data_buf + 9);
//				Phone_IP[3]	= *(data_buf + 10);
				//手机ID
				for(i = 0; i < 16; i ++)
				{
					Phone_ID[i] = *(data_buf + 11 + i);
					//Phone_ID[1] = *(data_buf + 26);
				}
				//白板ID
//				Board_ID[0] = *(data_buf + 35);
//				Board_ID[1] = *(data_buf + 36);
				wifi_ack_send(0x78, 0);
				searching_flag = 1;
			}
//			wifi_ack_send(0x77, 0);
//			is_idle_flag = 1;
			break;
		case 0xA9://心跳包
			wifi_ack_send(0x77, 0);
			is_idle_flag = 1;
			break;
		case 0xA1:{//接到手机发送的会议结束指令
			meeting_end_flag = 1;
			is_idle_flag = 0;
			//wifi_ack_send(0x77,0);
			//*增加指令
			//(1)连接状态断开,同时重启wifi模块
			//(2)队列清零,WIFI缓冲清零,手机IP和ID清零,白板IP重读
			//(3)ERR_Status变量清零,会议状态准备
			break;
			}
		case 0xA2:{//要求上次指令重发
//			Uart2_Put_Buf(wifi_data_to_send , wifi_sent_len);
			break;
			}
		case 0xA5:{//目前效果等同于A9
//		  if(ERR_Status==0)
//				wifi_ack_send(0x77,0);
//			else
//				wifi_ack_send(0x77,0x7800|ERR_Status);		
			break;		
			}
		case 0xA6:{//白板触屏信息
			wifi_ack_send(0xB2,0);	
			break;
			}
		case 0xa7:{//硬件版本号信息
			wifi_ack_send(0xB1,0);
			break;
			}
		case 0xA8:{//查询当前录制状态
//		switch(REC_STAT_Flag){
//			case 0:
//				temp_sh=0x7910;
//				break;
//			case 1:
//				temp_sh=0x7911;
//				break;
//			case 2:
//				temp_sh=0x7913;
//			break;
//		}
		
//		wifi_ack_send(0x77,temp_sh);
			break;
			}
		case 0xAC:{	//把数据使终保存在队列中,不管有没有发送
					//*增加队列尾指针的状态控制变量
					//FLA_DPMV_Enable=0;
			wifi_ack_send(0x77,0);
			break;
			}
		case 0xAB:{	//队更清零命令
			wifi_ack_send(0x77,0);
					//*增加调用队列清零子函数
					//*如果有错误,wifi_ack_send(0x77,0x78FE)
			break;
			}
		case 0xAD:{	//队列中的数据发送指令
			wifi_ack_send(0x77,0);
//			FLA_DPMV_Enable=1;
					//*增加 队列的当前指针指向尾指针所在位置
			break;
			}
		case 0xAA:{	//固件升级模式
			wifi_ack_send(0x77,0);
					//*增加固件升时需要的指令
			break;
			}
//		case 0x77:{//收到手机指令正常应答信号
////		is_wifisent_flag=1;//上一条指令发送完成
////*增加:如果FLA_DPMV_Enable==1;队列尾指针=当前指针
//			break;	
//			}
		case 0x73:{	//收到对时请求
					//*增加计时器清零指令,开启计时器指令
			time_base = 0;
			//wifi_ack_send(0x74,0);
			time_checking_flag = 1;
			break;
			}
		case 0x79:{//收到指示信息控制
			//*增加调用指示控制函蔐ED_Bee_Con�,参数为*(data_buf+7),该函数的定义参见传输协议:0x79的参数		
			wifi_ack_send(0x77,0);
			break;		
			}
		case 0x7A:{//收到设置网络帐号和密码的指令
			wifi_ack_send(0x77,0);
//*增加指令:
//(1)构建一个字符串:AT+STA=[ssid],[sectype],[key]		
//(2)WIFI模块进入AT模式		
//(3)把字符串送入,等待接收正常信息
//(4)重启wifi模块
			break;
			}
		case 0x5A:{//收到设置白板二维码指令
			wifi_ack_send(0x77,0);
//		for(i = 0; i < 10; i++)
//		{
//			Bar_code[i]=*(data_buf+7+i);
//		}
//*增加指令:
//调用把Bar_code反写入FLASH指令
//如果有错误,调用	wifi_ack_send(0x77,0x78FE);	
			break;
			}	
		case 0x5B:{//设置触屏标志号 unsigned char TC_INF[4]
			wifi_ack_send(0x77,0);
//		for(i=0;i<4;i++){
//			TC_INF[i]=*(data_buf+7+i);
//		}
//*增加指令:
//调用把TC_INF反写入FLASH指令
//如果有错误,调用	wifi_ack_send(0x77,0x78FE);	
			break;
			}
		case 0x5c:{//设置误差阈值
			wifi_ack_send(0x77,0);
//		Ex=*(data_buf+7);
//		Ey=*(data_buf+8);
//*增加指令:
//调用把EX EY 反写入FLASH指令
//如果有错误,调用	wifi_ack_send(0x77,0x78FE);	
			break;
			}
		case 0x5D:{//设置点数据上传给手机的时间间隔
			wifi_ack_send(0x77,0);
//		Dev_T=*(data_buf+8);
//		Dev_T<<=8;
//		Dev_T+=*(data_buf+7);
//*增加指令:
//调用把Dev_T反写入FLASH指令
//如果有错误,调用	wifi_ack_send(0x77,0x78FE);	
//重新设置延时指令		
			break;
			}			
	}
}
