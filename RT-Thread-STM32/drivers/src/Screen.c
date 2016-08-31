#include "usart2.h"
#include "Screen.h"
#include <string.h>
#include "wifi.h"
#include "ledseq.h"
#include "spi_flash.h"
#include "prome_stat.h"
#define SW_VERSION	"1.0.0"
#define HW_VERSION	"1.0.0"

#define EnableIRP()   __enable_irq()
#define DisableIRP()  __disable_irq()

//ScreenSeq_structdef ScreenDataSeq = {0};
FlashSeq_structdef FlashScreenDataSeq = {0};
unsigned int DataFrame_in_Flash = 0;
unsigned int time_base = 0;
unsigned char up_down_flag = 0;
unsigned char pen_data_ready = 0;
unsigned char net_complete_flag = 0;
unsigned char net_reconnect_flag = 0;
unsigned char net_reset_flag = 0;
unsigned char is_idle_flag = 0;
unsigned char meeting_end_flag = 0;
unsigned char time_checking_flag = 0;
unsigned char searching_flag = 0;
unsigned char pen_data[200] = {0};//�ʼ�����

void Screen_data_analize(unsigned char *data_buf,unsigned char num)//����Э�鴦��
{
	vs16 rc_value_temp;
//	unsigned char sum = 0;
	unsigned char i = 0;
	unsigned char point_cnt = 0;//���������

//	p = &point_data[0];
//	for(i=0;i<(num-1);i++)
//		sum += *(data_buf+i);
//	if(!(sum==*(data_buf+num-1)))		
//	{	
//		return;		//�ж�sum
//	}
	
	if(!(*(data_buf) == 0x68))	
	{	
		return;
	}		//�ж�֡ͷ
/////////////////////////////////////////////////////////////////////////////////////
	switch(*(data_buf + 2))
	{
		case 0x00:
			break;
		case 0x01:
			break;
		case 0x02:
			point_cnt = (*(data_buf + 1) - 2) / 10;
			for(i = 0; i < point_cnt; i++)
			{
				pen_data[i*12 + 0] =  *(data_buf + 5 + i*10);
				pen_data[i*12 + 1] =  *(data_buf + 6 + i*10);
				pen_data[i*12 + 2] =  *(data_buf + 7 + i*10);
				pen_data[i*12 + 3] =  *(data_buf + 8 + i*10);
				pen_data[i*12 + 4] =  *(data_buf + 9 + i*10);
				pen_data[i*12 + 5] =  *(data_buf +10 + i*10);
				pen_data[i*12 + 6] =  *(data_buf +11 + i*10);
				pen_data[i*12 + 7] =  *(data_buf +12 + i*10);
				if(*(data_buf + 3 + i*10 + 0) == 0x04)//��״̬  ̧�� 0x04
				{
					pen_data[i*12 + 8] = (*(data_buf + 4 + i*10 ) & 0x7f) | 0x80;
				}
				else if(*(data_buf + 3 + i*10 + 0) == 0x07)//��״̬  ��д�� 0x07
				{
					pen_data[i*12 + 8] = (*(data_buf + 4 + i*10 ) & 0x7f);
					up_down_flag = 1;
				}
				pen_data[i*12 + 9] =  time_base & 0x0000ff ;//ʱ���
				pen_data[i*12 +10] =  (time_base & 0x00ff00) >> 8;
				pen_data[i*12 +11] =  (time_base & 0xff0000) >> 16;
			}
//			ScreenRecSeqIn(pen_data, point_cnt);//�����
			pen_data_ready = 1;
			wifi_send_pen_data(pen_data, point_cnt);
			break;
		case 0x60:
			up_down_flag = 0;
			for(i = 0; i < 100; i++)
			{
				pen_data[i] = 0;
			}
			break;
	}

}
/*��ʼ�����ݴ洢����*/
void ScreenRecSeqInit()
{
	FlashScreenDataSeq.SeqFrontAddr = 0;		//Ϊ������ӻ����ַ
	FlashScreenDataSeq.SeqRearAddr = 0;			//������������
	FlashScreenDataSeq.SeqReadAddr = 0;
}
/*����ն��з���һ֡����*/

//���������
//��ڲ�����unsigned char *_DataFrame ��д�������
//			unsigned char _DataLen	  ��д��ıʼ����ݵ�������
//����    ��state ��Error ������  ok д��ɹ�
State_typedef ScreenRecSeqIn(unsigned char *_DataFrame, unsigned char _DataLen)
{
//	DisableIRP();
	if(((FlashScreenDataSeq.SeqFrontAddr + _DataLen * 12) & 0x200000) &&
		/*(FlashScreenDataSeq.SeqFrontAddr >= FlashScreenDataSeq.SeqRearAddr) || *///ͷ��βǰ
//		((FlashScreenDataSeq.SeqFrontAddr < FlashScreenDataSeq.SeqRearAddr)&&//ͷ��δ׷��β
		(((FlashScreenDataSeq.SeqFrontAddr + _DataLen * 12) & 0x1FFFFF) > FlashScreenDataSeq.SeqRearAddr))//��һ֡�������д���ͷָ���Ƿ�ᳬ��β
	{
		//������
		ledseq_run(BEEP, seq_armed);
		BEEP_STAT_Flag = 3;//������ ������������
		ERR_Status = 0xE1;//���������м��뻺������־
//		EnableIRP();
		//TUDO::�жϻ��������
    	return Error;		
	}
	else
	{
		DisableIRP();
		SPI_FLASH_BufferWrite(_DataFrame, FlashScreenDataSeq.SeqFrontAddr, _DataLen * 12);//д��һ֡����
		EnableIRP();
		FlashScreenDataSeq.SeqFrontAddr = ((FlashScreenDataSeq.SeqFrontAddr + _DataLen * 12) & 0x1FFFFF);
		
		/*�����ڻ������е���������*/
		if(FlashScreenDataSeq.SeqFrontAddr < FlashScreenDataSeq.SeqRearAddr)//
			DataFrame_in_Flash = FlashScreenDataSeq.SeqFrontAddr + 0x200000 - FlashScreenDataSeq.SeqRearAddr;
		else
			DataFrame_in_Flash = FlashScreenDataSeq.SeqFrontAddr - FlashScreenDataSeq.SeqRearAddr;
		if((DataFrame_in_Flash > 1258290)&&(DataFrame_in_Flash < 1677720))//������80% ������������
		{
			ledseq_run(BEEP, seq_flash_80_waring);
			BEEP_STAT_Flag = 2;
		}
		else if(DataFrame_in_Flash > 1677720)//������60% ��������һ��
		{
			ledseq_run(BEEP, seq_flash_60_waring);
			BEEP_STAT_Flag = 1;
		}
	}	
//	EnableIRP();
	return Ok;
} 									 
/*���ն��г����У��������Ϊ�յĻ��򷵻�Error*/
/*
 * ���ݳ����У��ú������жϻ������е��������������������>=һ֡����ȫ����ȡ�����򷵻ض��п�
 *
 *
 */
State_typedef ScreenRecSeqOut(unsigned char *_DataFrame, unsigned char _DataLen)
{
//	DisableIRP();
	if(FlashScreenDataSeq.SeqReadAddr == FlashScreenDataSeq.SeqFrontAddr)  //�����
	{
//		EnableIRP();
		return Error;
	}
	else
	{		

		SPI_FLASH_BufferRead(_DataFrame, FlashScreenDataSeq.SeqReadAddr, _DataLen);
		if((FlashScreenDataSeq.SeqReadAddr / 4096) != ((FlashScreenDataSeq.SeqReadAddr + _DataLen) / 4096))
			SPI_FLASH_SectorErase(FlashScreenDataSeq.SeqReadAddr);
		EnableIRP();
		FlashScreenDataSeq.SeqReadAddr = ((FlashScreenDataSeq.SeqReadAddr + _DataLen) & 0x1FFFFF);
//		EnableIRP();
		return Ok;
	} 

}
///*��ʼ�����ݴ洢����*/
//void ScreenRecSeqInit()
//{
////	ScreenDataSeq.Datalen[] = 0;
//	ScreenDataSeq.addr = 0;		//Ϊ������ӻ����ַ
//	ScreenDataSeq.SeqFront = 0;			//������������
//	ScreenDataSeq.SeqRear = 0;
//}
///*����ն��з���һ֡����*/
//State_typedef ScreenRecSeqIn(unsigned char *_DataFrame, unsigned char _DataLen)
//{
//	DisableIRP();
//	if(((ScreenDataSeq.SeqFront + 1) % Seqbufferlen) == ScreenDataSeq.SeqRear)  //������ TUDO::�Ƿ�������������������һ���Ƕ��������ڶ����Ǵ洢�ռ���
//    {
//		EnableIRP();
//		//TUDO::�жϻ��������
//    	return Error;
//  	}
//	/*TUDO::�ô�����flash�洢*/
//	
//	ScreenDataSeq.addr += _DataLen * 12;
//	ScreenDataSeq.DataHead[ScreenDataSeq.SeqFront].DataLen = _DataLen * 12;
//	ScreenDataSeq.DataHead[ScreenDataSeq.SeqFront].DataAddr = ScreenDataSeq.addr;
//	
////	SPI_FLASH_PageWrite(_DataFrame, ScreenDataSeq.DataHead[ScreenDataSeq.SeqFront].DataAddr, (_DataLen + 1) * 12);
//	SPI_FLASH_BufferWrite(_DataFrame, ScreenDataSeq.DataHead[ScreenDataSeq.SeqFront].DataAddr, _DataLen * 12);
//	
//  	ScreenDataSeq.SeqFront = (ScreenDataSeq.SeqFront + 1) % Seqbufferlen;  //ָ��ѭ��0~��Seqbufferlen - 1��
//	if(ScreenDataSeq.SeqFront == 0)
//	{
//		ScreenDataSeq.addr = 0;
//	}
//	EnableIRP();
//  	return Ok; 
//} 									 
///*���ն��г����У��������Ϊ�յĻ��򷵻�Error*/
//State_typedef ScreenRecSeqOut(unsigned char *_DataFrame, unsigned char *_DataLen)
//{
//	DisableIRP();
//  	if(ScreenDataSeq.SeqFront == ScreenDataSeq.SeqRear)  //�����
//  	{
//		EnableIRP();
//    	return Error;
//  	}
//	else
//  	{
//		//TUDO::�ô�����flash��ȡ
//		SPI_FLASH_BufferRead(_DataFrame, ScreenDataSeq.DataHead[ScreenDataSeq.SeqRear].DataAddr , ScreenDataSeq.DataHead[ScreenDataSeq.SeqRear].DataLen);
//		*_DataLen = ScreenDataSeq.DataHead[ScreenDataSeq.SeqRear].DataLen;
//		if((ScreenDataSeq.DataHead[ScreenDataSeq.SeqRear].DataAddr / 4096) > 0)
//		{
//			SPI_FLASH_SectorErase(4096 * (ScreenDataSeq.DataHead[ScreenDataSeq.SeqRear].DataAddr / 4096 - 1));
//		}
//    	ScreenDataSeq.SeqRear = (ScreenDataSeq.SeqRear + 1) % Seqbufferlen; 
//		//�Ƿ���ܵ��ж�Ӱ�죿
//		EnableIRP();
//    	return Ok;
//		
//  	} 
//}
