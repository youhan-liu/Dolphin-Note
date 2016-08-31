#include "cc3200.h"
#include "usart2.h"
#include "Screen.h"
#include "wifi.h"
#include <string.h>
#include <stdio.h>
#include "ledseq.h"

u8 socket_start = 0;
u8 Pre_Phone_IP[4] = {0};
u8 RT_Phone_IP[16] = {0};
u8 Local_IP[4]={0};	// ����IP
u8 MAC_BUF[15] = {0};
const u8* Local_Port  = "9002";		// ���ض˿ں�
const u8* PORT_Server = "9001";		// Զ�̶˿ں�
const u8* IP_Server   = "192.168.100.255";		// Զ��IP
const u8* PORT = "0";
const u8* IP   = "192.168.100.255";
const u8 *CC3200_WORKMODE_TBL[4]={"TCPS", "TCPC", "UDPS", "UDPC"};		//4�ֹ���ģʽ

//static void* timer_heartbeat;//����յ�ַָ�룬ָ���ĸ���ʱ�������ַ����ʡ�ռ�

void cc3200_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
		
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	
	usart2_init(115200);
}

//////////////�ֽڷ���//////////////////////////
void USART2_SendByte(rt_uint8_t Data)
{
	//�ȴ��������ݼĴ�����
	while (!(USART2->SR & USART_FLAG_TXE));	//����δת�Ƶ���λ�Ĵ���
	//��������
	USART2->DR = (Data & 0x00FF);
	//�ȴ��������
	while (!(USART2->SR & USART_FLAG_TC));   
}

//////////////����ATָ��////////////////////////////
void USART2_Send_ATCmd(rt_uint8_t *data)
{    
  	rt_uint16_t i,len;
	
	len = strlen((const char*)data);
		
	for (i=0; i<len; i++)
	  USART2_SendByte(data[i]);	     
}

/**************************************************
* CC3200���������,�����յ���Ӧ��
* str:	�ڴ���Ӧ����
* ����ֵ: 0,û�еõ��ڴ���Ӧ����
*     		����,�ڴ�Ӧ������λ��(str��λ��)
**************************************************/
u8* cc3200_check_cmd(u8 *str)
{
	
	char *strx=0;
	if(USART2_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}

/**************************************************
*�������ܣ���CC3200��������
*cmd: ���͵������ַ���
*ack: �ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
*waittime: �ȴ�ʱ��(��λ:10ms)
*����ֵ: 0,���ͳɹ�(�õ����ڴ���Ӧ����)
*        1,����ʧ��
**************************************************/
u8 cc3200_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART2_RX_STA=0;
	USART2_Send_ATCmd(cmd);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			rt_thread_delay(10);
			if(USART2_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(cc3200_check_cmd(ack))
				{
					break;//�õ���Ч���� 
				}
					USART2_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 

/*******************************************************************************
* ������  : cc3200_Hardware_Reset
* ����    : Ӳ����λcc3200
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��λ��ȴ� 3S ���Ӧ���ź�
*******************************************************************************/
void cc3200_Hardware_Reset(void)
{
	GPIO_ResetBits(CC3200_RST_PORT, CC3200_RST_Pin);//��λ��������
	rt_thread_delay(500);
	GPIO_SetBits(CC3200_RST_PORT, CC3200_RST_Pin);//��λ��������
	rt_thread_delay(5000);
}

/*******************************************************************************
* ������  : wifi_reset
* ����    : ���� cc3200 ����
* ����ֵ  : ��
* ˵��    : ������㹻����ʱʱ�䣬��Ȼ�����޷���Ӧ
*******************************************************************************/
void wifi_reset(void)
{
	socket_start = 0;
	cc3200_Hardware_Reset();
	cc3200_send_cmd("+++", "Switch AT Command Mode!", 100);
	rt_thread_delay(3000);
	sprintf((char*)USART2_TX_BUF, "%s", "AT+RESET");
	cc3200_send_cmd(USART2_TX_BUF, "Reset", 150);		// "Reset All Config!"
	rt_thread_delay(5000);
	cc3200_Hardware_Reset();		// ������Ч����������
	rt_thread_delay(5000);			// �㹻����ʱ���ȴ���Ӧ
}

/*******************************************************************************
* ������  : cc3200_start
* ����    : cc3200 ����
* ����ֵ  : ��
* ˵��    : ��ȡ IP, ���� socket ����Ҫ������ wifi ��ǰ���£������޷�ͨ��
*******************************************************************************/
void cc3200_start(void)
{
	while(cc3200_smartcfg(Link_wait_time,Cfg_over_time))	// �ȴ��ͳ�ʱʱ���ɺ궨�����
	{
		rt_thread_delay(1);		// ���� WiFi ʱ, �����ȴ�
	}
	cc3200_get_staip(Board_IP);		// ���� WiFi ��, ��ȡ�װ� IP
}

/*******************************************************************************
* ������  : cc3200_smartcfg
* ����    : һ������cc3200
* ����ֵ  : 0���ɹ���1��ʧ��
* ˵��    : wait_time�����ӵȴ�ʱ��; over_time: �������ӳ�ʱʱ�䣬��λ 1S
*******************************************************************************/
u8 cc3200_smartcfg(u16 wait_time, u16 over_time)
{
	u8 time=0;
	socket_start = 0;
	cc3200_Hardware_Reset();
	cc3200_send_cmd("+++", "Switch AT Command Mode!", 50);
	if(cc3200_send_cmd("AT+ROLE=?", "WiFi Role = STA.", 50) == 0)// �����ʱ��ʹ���ݽ�����
	{	
		while(wait_time--)
		{
			rt_thread_delay(1000);
			if(GPIO_ReadInputDataBit(CC3200_Link_PORT, CC3200_Link_Pin) == 0)
				break;
			
			if(wait_time == 0)
			{
				sprintf((char*)USART2_TX_BUF, "%s", "AT+STA=ff");					// ÿ�� smartconfig ʱ������б�
				cc3200_send_cmd(USART2_TX_BUF, "Del All STA Profile", 100);
				cc3200_send_cmd("AT+SL=smartcfg", "Enter Smartconfig Mode...", 100);
			}
		}
	}
	else
	{
		rt_thread_delay(5000);	// �����ʱ, ����������ò��ɹ�
		cc3200_send_cmd("AT+ROLE=STA", "Change WiFi Role = STA.", 100);		// ����Ҫ��֤���óɹ�
		rt_thread_delay(1000);
		cc3200_send_cmd("AT+RST", "Device Restart...", 300);
		rt_thread_delay(3000);
		cc3200_send_cmd("+++", "Switch AT Command Mode!", 50);
		rt_thread_delay(2000);
		sprintf((char*)USART2_TX_BUF, "%s", "AT+STA=ff");					// ÿ�� smartconfig ʱ������б�
		cc3200_send_cmd(USART2_TX_BUF, "Del All STA Profile", 100);
		cc3200_send_cmd("AT+SL=smartcfg", "Enter Smartconfig Mode...", 100);
	}
//	ledseq_run(LED_REC, seq_alive);
	
	if(GPIO_ReadInputDataBit(CC3200_Smart_PORT, CC3200_Smart_Pin) == 0)
	{
		while(GPIO_ReadInputDataBit(CC3200_Link_PORT, CC3200_Link_Pin))			// �ȴ�����
		{
			rt_thread_delay(1000);
			time++;
			if(time >= over_time)
				return 1;
		}
	}
//	ledseq_stop(LED_REC, seq_alive);
//	ledseq_run(LED_REC, seq_power_on);
	return 0;
}  

/*******************************************************************************
* ������  : ASCII_to_binary
* ����    : ASCII ת���� ������
* ����ֵ  : int��ת�����
* ˵��    : 
*******************************************************************************/
u8 ASCII_to_binary(u8* data, u8 len)
{
	u8 i, data_t=0;
	for(i=0;i<len;i++)
	{
		data_t *= 10;
		data_t += data[i] - '0';
	}
	return data_t;
}

/*******************************************************************************
* ������  : cc3200_get_staip
* ����    : ��ȡIP ��ַ
* ����ֵ  : ��
* ˵��    : ipbuf: IP ������
*******************************************************************************/
void cc3200_get_staip(u8* Local_IP)
{
	u8 *p, *p1, *p2, *p3, *p4;
	u8 Local_IP0[3] = {0};
	u8 Local_IP1[3] = {0};
	u8 Local_IP2[3] = {0};
	u8 Local_IP3[3] = {0};
	if(cc3200_send_cmd("AT+IP=?", "DHCP", 50) == 0)
	{
		p  = cc3200_check_cmd(",");
		p1 = (u8*)strstr((const char*)(p+1), ".");
		*p1= 0;
		sprintf((char*)Local_IP0, "%s", p+1);
		Local_IP[0] = ASCII_to_binary(Local_IP0, p1-p-1);

		p2 = (u8*)strstr((const char*)(p1+1), ".");
		*p2= 0;
		sprintf((char*)Local_IP1, "%s", p1+1);
		Local_IP[1] = ASCII_to_binary(Local_IP1, p2-p1-1);

		p3 = (u8*)strstr((const char*)(p2+1), ".");
		*p3= 0;
		sprintf((char*)Local_IP2, "%s", p2+1);
		Local_IP[2] = ASCII_to_binary(Local_IP2, p3-p2-1);

		p4 = (u8*)strstr((const char*)(p3+1), ",");
		*p4= 0;
		sprintf((char*)Local_IP3, "%s", p3+1);
		Local_IP[3] = ASCII_to_binary(Local_IP3, p4-p3-1);
	}
}

/*******************************************************************************
* ������  : cc3200_soclet1
* ����    : ǰһ�ν���socket, ��ȡ������ IP
* ����ֵ  : ��
* ˵��    : ��ʼģʽ��UDPC,9002,0.0.0.0,9001
*******************************************************************************/
void cc3200_set_udpPort(void)
{
	sprintf((char*)USART2_TX_BUF, "AT+SOCK=%s,%s,%s,%s", CC3200_WORKMODE_TBL[3], Local_Port, IP, PORT_Server);
	cc3200_send_cmd(USART2_TX_BUF, "Set Socket CFG:", 100);
	socket_start = 1;
	cc3200_send_cmd("AT+RST", "Device Restart...", 300);
	rt_thread_delay(3000);
	while(GPIO_ReadInputDataBit(CC3200_Socket_PORT, CC3200_Socket_Pin) == 1)
	{
		rt_thread_delay(1);
	}
	ledseq_stop(LED_NET, seq_alive);
	ledseq_run(LED_NET, seq_alive);
	
}

/*******************************************************************************
* ������  : cc3200_soclet2
* ����    : ��һ�ν���socket, ����ͨ��
* ����ֵ  : ��
* ˵��    : ��������Ч���ȴ��������磬��Լ 8s ����
*******************************************************************************/
void cc3200_set_udpIP(void)
{
	u8 i;
	for(i=0;i<4;i++)
		Pre_Phone_IP[i] = Phone_IP[i];

	sprintf((char*)RT_Phone_IP, "%d.%d.%d.%d", Phone_IP[0],Phone_IP[1],Phone_IP[2],Phone_IP[3]);

	socket_start = 0;
	cc3200_send_cmd("+++", "Switch AT Command Mode!", 50);
	sprintf((char*)USART2_TX_BUF, "AT+SOCK=%s,%s,%s,%s", CC3200_WORKMODE_TBL[3], Local_Port, RT_Phone_IP, PORT_Server);
	cc3200_send_cmd(USART2_TX_BUF, "Set Socket CFG:", 100);	
	socket_start = 1;
	cc3200_send_cmd("AT+RST", "Device Restart...", 300);
	rt_thread_delay(3000);
	while(GPIO_ReadInputDataBit(CC3200_Link_PORT, CC3200_Link_Pin) == 1)
	{
		rt_thread_delay(1);
	}

//	ledseq_stop(LED_NET, seq_alive);
	ledseq_stop(LED_NET, seq_power_on);
	ledseq_run(LED_NET, seq_power_on);
	net_complete_flag = 1;
}

/*******************************************************************************
* ������  : cc3200_socket
* ����    : ���� socket
* ����ֵ  : ��
* ˵��    : ��������Ч���ȴ��������磬��Լ 8s ����
*******************************************************************************/
void cc3200_socket(void)
{
//	u8 wait_time = 0;
	u8 i;
	sprintf((char*)USART2_TX_BUF, "AT+SOCK=%s,%s,%s,%s", CC3200_WORKMODE_TBL[3], Local_Port, IP, PORT_Server);
	cc3200_send_cmd(USART2_TX_BUF, "Set Socket CFG:", 100);
	socket_start = 1;
	cc3200_send_cmd("AT+RST", "Device Restart...", 300);
	rt_thread_delay(3000);
	while(GPIO_ReadInputDataBit(CC3200_Socket_PORT, CC3200_Socket_Pin) == 1)
	{
		rt_thread_delay(1);
	}
	ledseq_stop(LED_NET, seq_alive);
	ledseq_run(LED_NET, seq_alive);
	
	while(Phone_IP[0] == 0)	//����ط��������ȴ���һ�����ӵĹ����н��յ�IP���ݣ�ͬʱҲ����˵�һ�����ӵ�ʱ��
							//����װ�ID���ԣ���ô��Э�������ʱ�򣬲����Phone_IP���鸳ֵ��
							//Ҳ���ǲ�����Ӧ����İװ�ID��������
	{
		rt_thread_delay(1);
	}

	for(i=0;i<4;i++)
		Pre_Phone_IP[i] = Phone_IP[i];

	sprintf((char*)RT_Phone_IP, "%d.%d.%d.%d", Phone_IP[0],Phone_IP[1],Phone_IP[2],Phone_IP[3]);

	socket_start = 0;
	cc3200_send_cmd("+++", "Switch AT Command Mode!", 50);
	sprintf((char*)USART2_TX_BUF, "AT+SOCK=%s,%s,%s,%s", CC3200_WORKMODE_TBL[3], Local_Port, RT_Phone_IP, PORT_Server);
	cc3200_send_cmd(USART2_TX_BUF, "Set Socket CFG:", 100);	
	socket_start = 1;
	cc3200_send_cmd("AT+RST", "Device Restart...", 300);
	rt_thread_delay(3000);
	while(GPIO_ReadInputDataBit(CC3200_Link_PORT, CC3200_Link_Pin) == 1)
	{
		rt_thread_delay(1);
	}

//	ledseq_stop(LED_NET, seq_alive);
	ledseq_stop(LED_NET, seq_power_on);
	ledseq_run(LED_NET, seq_power_on);
	net_complete_flag = 1;
}
void cc3200_reconnect(void)
{
	unsigned char i = 0, j = 0;
	unsigned char is_same_IP = 1;
	while(1)
	{
		if(net_reconnect_flag)
		{
			net_reconnect_flag = 0;
			for(i = 0; i < 4; i++)//�ж��ֻ�ID�Ƿ���ȷ
			{
				if(Pre_Phone_IP[i] != Phone_IP[i])
				{
					for(j = 0; j < 4; j++)
						Pre_Phone_IP[j] = Phone_IP[j];
					is_same_IP = 0;
					break;
				}
			}
			if(is_same_IP)
			{
				net_complete_flag = 1;
				break;
			}
			else
			{
				sprintf((char*)RT_Phone_IP, "%d.%d.%d.%d", Phone_IP[0],Phone_IP[1],Phone_IP[2],Phone_IP[3]);
				socket_start = 0;
				cc3200_send_cmd("+++", "Switch AT Command Mode!", 50);
				sprintf((char*)USART2_TX_BUF, "AT+SOCK=%s,%s,%s,%s", CC3200_WORKMODE_TBL[3], Local_Port, RT_Phone_IP, PORT_Server);
				cc3200_send_cmd(USART2_TX_BUF, "Set Socket CFG:", 100);	
				socket_start = 1;
				cc3200_send_cmd("AT+RST", "Device Restart...", 300);
				rt_thread_delay(3000);
				while(GPIO_ReadInputDataBit(CC3200_Link_PORT, CC3200_Link_Pin) == 1)
				{
					rt_thread_delay(1);
				}
				net_complete_flag = 1;
				break;
			}
		}
		rt_thread_delay(1);
	}
}

/*******************************************************************************
* ������  : socket_stop
* ����    : �Ͽ��ֻ���װ�����
* ����ֵ  : ��
* ˵��    : ����Ϊ����ģʽ TCPS,9002,0.0.0.0,0
*******************************************************************************/
void socket_stop(void)
{
	socket_start = 0;
	cc3200_send_cmd("+++", "Switch AT Command Mode!", 50);
	rt_thread_delay(100);
	sprintf((char*)USART2_TX_BUF, "AT+SOCK=%s,%s,%s,%s", CC3200_WORKMODE_TBL[0], Local_Port, IP, PORT);		// TCPS,9002,0.0.0.0,0
	cc3200_send_cmd(USART2_TX_BUF, "Set Socket CFG:", 100);	
	socket_start = 1;
	rt_thread_delay(100);	
	cc3200_send_cmd("AT+RST", "Device Restart...", 300);
	rt_thread_delay(3000);
}

