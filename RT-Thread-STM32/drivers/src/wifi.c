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
void wifi_ack_send(unsigned char code, unsigned short para) //����Ӧ����Ϣָ��0x77,0x76,0xB1,0xB2 (code),para Ϊ0x77�Ĳ���
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
			//��ͨ״̬Ӧ��
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
			wifi_data_len=11;	//�����ַ�������	
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
			wifi_data_len=17;	//�����ַ�������
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
			wifi_data_len=17;	//�����ַ�������
			break;
		case 0xA1:
			//��ͨ״̬Ӧ��
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
			wifi_data_len=11;	//�����ַ�������	
			//�̼��汾��
			break;
		case 0x70:
			//��ͨ״̬Ӧ��
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
			wifi_data_len=11;	//�����ַ�������	
			//�̼��汾��
			break;
		case 0xB1:
		
			//�̼��汾��
			break;
		
		case 0xB2:
		
			//������Ϣ
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
			wifi_data_len = 13;	//�����ַ�������			
			//������Ϣ
			break;
	}
//	is_wifisent_flag=2;//����ϢΪӦ����Ϣ,����Ҫ�ֻ�Ӧ��
//	Uart2_Put_Buf(wifi_data_to_send , wifi_send_len);
}
void wifi_data_deal(unsigned char *data_buf, unsigned char num)
{
	
	vs16 rc_value_temp;
	unsigned char sum = 0;
	unsigned char i = 0;
	unsigned char Bar_code_err_flag = 0;
	unsigned char Phone_ID_err_flag = 0;
	
//	unsigned char point_cnt = 0;//���������

	if(!(*(data_buf)==0xFA && *(data_buf + 1)==0x0A))	
	{	
		return;
	}		//�ж�֡ͷ
/////////////////////////////////////////////////////////////////////////////////////
	switch(*(data_buf + 2))
	{				//��һ�����ӵ�ʱ����Ҫ�ж϶�ά���Ƿ���洢�Ķ�ά����ͬ�������ͬ�����ӣ�
					//�����Ķ�����������Ҫ�ж��ֻ�ID�Ƿ����һ������ʱ�洢����ͬ�������ͬ�Ļ��������ж��ֻ�IP�Ƿ�ı䣬
		case 0x75:	//����ı�����Ҫ��������UDPC��IP����
			for(i = 0; i < 10; i++)//�κ�������յ�����������Ҫ�ж϶�ά���Ƿ���ȷ
			{
				if(*(data_buf + 27 + i) != Bar_code[i])
				{
					Bar_code_err_flag = 1;
					break;
				}
			}
			if(Bar_code_err_flag == 1)//��ά���������Ӧ��������
				net_complete_flag = 0;
			else//��ά����ȷ�����ȣ�����ǵ�һ�����ӣ��洢���յ����ֻ�IP�������������á�
			{	//������ǵ�һ�����ӣ�����Ҫ�ȶ��ֻ�ID�Ƿ���ȷ
				//�ֻ�IP
				if(is_First_connect)
				{
					is_First_connect = 0;
					//if()
					Phone_IP[0]	= *(data_buf + 7);
					Phone_IP[1]	= *(data_buf + 8);
					Phone_IP[2]	= *(data_buf + 9);
					Phone_IP[3]	= *(data_buf + 10);
					//�ֻ�ID
					for(i = 0; i < 16; i ++)
					{
						Phone_ID[i] = *(data_buf + 11 + i);
						//Phone_ID[1] = *(data_buf + 26);
					}
					//�װ�ID
					Board_ID[0] = *(data_buf + 35);
					Board_ID[1] = *(data_buf + 36);
					wifi_ack_send(0x76, 0);
				}
				else
				{
					for(i = 0; i < 16; i++)//�ж��ֻ�ID�Ƿ���ȷ
					{
						if(*(data_buf + 11 + i) != Phone_ID[i])
						{
							Phone_ID_err_flag = 1;
							break;
						}
					}	
					if(Phone_ID_err_flag == 1)//�ֻ�ID��������Ӧ
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
		case 0x78://������
			for(i = 0; i < 10; i++)//�κ�������յ�����������Ҫ�ж϶�ά���Ƿ���ȷ
			{
				if(*(data_buf + 27 + i) != Bar_code[i])
				{
					Bar_code_err_flag = 1;
					break;
				}
			}
			if(Bar_code_err_flag == 1)//��ά���������Ӧ��������
				searching_flag = 0;
			else
			{
//				//if()
//				Phone_IP[0]	= *(data_buf + 7);
//				Phone_IP[1]	= *(data_buf + 8);
//				Phone_IP[2]	= *(data_buf + 9);
//				Phone_IP[3]	= *(data_buf + 10);
				//�ֻ�ID
				for(i = 0; i < 16; i ++)
				{
					Phone_ID[i] = *(data_buf + 11 + i);
					//Phone_ID[1] = *(data_buf + 26);
				}
				//�װ�ID
//				Board_ID[0] = *(data_buf + 35);
//				Board_ID[1] = *(data_buf + 36);
				wifi_ack_send(0x78, 0);
				searching_flag = 1;
			}
//			wifi_ack_send(0x77, 0);
//			is_idle_flag = 1;
			break;
		case 0xA9://������
			wifi_ack_send(0x77, 0);
			is_idle_flag = 1;
			break;
		case 0xA1:{//�ӵ��ֻ����͵Ļ������ָ��
			meeting_end_flag = 1;
			is_idle_flag = 0;
			//wifi_ack_send(0x77,0);
			//*����ָ��
			//(1)����״̬�Ͽ�,ͬʱ����wifiģ��
			//(2)��������,WIFI��������,�ֻ�IP��ID����,�װ�IP�ض�
			//(3)ERR_Status��������,����״̬׼��
			break;
			}
		case 0xA2:{//Ҫ���ϴ�ָ���ط�
//			Uart2_Put_Buf(wifi_data_to_send , wifi_sent_len);
			break;
			}
		case 0xA5:{//ĿǰЧ����ͬ��A9
//		  if(ERR_Status==0)
//				wifi_ack_send(0x77,0);
//			else
//				wifi_ack_send(0x77,0x7800|ERR_Status);		
			break;		
			}
		case 0xA6:{//�װ崥����Ϣ
			wifi_ack_send(0xB2,0);	
			break;
			}
		case 0xa7:{//Ӳ���汾����Ϣ
			wifi_ack_send(0xB1,0);
			break;
			}
		case 0xA8:{//��ѯ��ǰ¼��״̬
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
		case 0xAC:{	//������ʹ�ձ����ڶ�����,������û�з���
					//*���Ӷ���βָ���״̬���Ʊ���
					//FLA_DPMV_Enable=0;
			wifi_ack_send(0x77,0);
			break;
			}
		case 0xAB:{	//�Ӹ���������
			wifi_ack_send(0x77,0);
					//*���ӵ��ö��������Ӻ���
					//*����д���,wifi_ack_send(0x77,0x78FE)
			break;
			}
		case 0xAD:{	//�����е����ݷ���ָ��
			wifi_ack_send(0x77,0);
//			FLA_DPMV_Enable=1;
					//*���� ���еĵ�ǰָ��ָ��βָ������λ��
			break;
			}
		case 0xAA:{	//�̼�����ģʽ
			wifi_ack_send(0x77,0);
					//*���ӹ̼���ʱ��Ҫ��ָ��
			break;
			}
//		case 0x77:{//�յ��ֻ�ָ������Ӧ���ź�
////		is_wifisent_flag=1;//��һ��ָ������
////*����:���FLA_DPMV_Enable==1;����βָ��=��ǰָ��
//			break;	
//			}
		case 0x73:{	//�յ���ʱ����
					//*���Ӽ�ʱ������ָ��,������ʱ��ָ��
			time_base = 0;
			//wifi_ack_send(0x74,0);
			time_checking_flag = 1;
			break;
			}
		case 0x79:{//�յ�ָʾ��Ϣ����
			//*���ӵ���ָʾ���ƺ��LED_Bee_Con�,����Ϊ*(data_buf+7),�ú����Ķ���μ�����Э��:0x79�Ĳ���		
			wifi_ack_send(0x77,0);
			break;		
			}
		case 0x7A:{//�յ����������ʺź������ָ��
			wifi_ack_send(0x77,0);
//*����ָ��:
//(1)����һ���ַ���:AT+STA=[ssid],[sectype],[key]		
//(2)WIFIģ�����ATģʽ		
//(3)���ַ�������,�ȴ�����������Ϣ
//(4)����wifiģ��
			break;
			}
		case 0x5A:{//�յ����ðװ��ά��ָ��
			wifi_ack_send(0x77,0);
//		for(i = 0; i < 10; i++)
//		{
//			Bar_code[i]=*(data_buf+7+i);
//		}
//*����ָ��:
//���ð�Bar_code��д��FLASHָ��
//����д���,����	wifi_ack_send(0x77,0x78FE);	
			break;
			}	
		case 0x5B:{//���ô�����־�� unsigned char TC_INF[4]
			wifi_ack_send(0x77,0);
//		for(i=0;i<4;i++){
//			TC_INF[i]=*(data_buf+7+i);
//		}
//*����ָ��:
//���ð�TC_INF��д��FLASHָ��
//����д���,����	wifi_ack_send(0x77,0x78FE);	
			break;
			}
		case 0x5c:{//���������ֵ
			wifi_ack_send(0x77,0);
//		Ex=*(data_buf+7);
//		Ey=*(data_buf+8);
//*����ָ��:
//���ð�EX EY ��д��FLASHָ��
//����д���,����	wifi_ack_send(0x77,0x78FE);	
			break;
			}
		case 0x5D:{//���õ������ϴ����ֻ���ʱ����
			wifi_ack_send(0x77,0);
//		Dev_T=*(data_buf+8);
//		Dev_T<<=8;
//		Dev_T+=*(data_buf+7);
//*����ָ��:
//���ð�Dev_T��д��FLASHָ��
//����д���,����	wifi_ack_send(0x77,0x78FE);	
//����������ʱָ��		
			break;
			}			
	}
}
