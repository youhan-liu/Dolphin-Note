#ifndef __SCREEN_H
#define __SCREEN_H

#include "stm32f10x.h"
#include <rtthread.h>

#define Seqbufferlen 20//�������
extern unsigned int DataFrame_in_Flash ;
//unsigned char BEEP_STAT_Flag;
//unsigned char ERR_Status;
typedef enum
{
	Ok		=(unsigned char)1, 
	Error	=! Ok    
}State_typedef;
///*���нṹ*/
//typedef struct
//{
//	unsigned char	DataLen;//
//	unsigned int	DataAddr;//
//}SeqDataAddr_structdef;

///*���нṹ*/
//typedef struct
//{
//	unsigned int			addr;//����FLASH�е����ݵ�ַָ��
////	unsigned char			Seqlen;//�������
//	SeqDataAddr_structdef	DataHead[Seqbufferlen];//���ݳ���
//	unsigned int 			SeqFront;//���еĶ�����������
//	unsigned int			SeqRear;//���еĵײ���������
//}ScreenSeq_structdef;
/*���нṹ*/
typedef struct
{
//	unsigned int			addr;//����FLASH�е����ݵ�ַָ��
//	unsigned char			Seqlen;//�������
//	SeqDataAddr_structdef	DataHead[Seqbufferlen];//���ݳ���
	unsigned int 			SeqFrontAddr;//���еĶ�����������
	unsigned int			SeqRearAddr;//���еĵײ���������
	unsigned int			SeqReadAddr;
}FlashSeq_structdef;

extern unsigned int time_base;
extern unsigned char up_down_flag;
extern unsigned char pen_data_ready;
extern unsigned char net_complete_flag;
extern unsigned char net_reconnect_flag;
extern unsigned char net_reset_flag;
extern unsigned char is_idle_flag;
extern unsigned char meeting_end_flag;
extern unsigned char time_checking_flag;
extern unsigned char searching_flag;
void Screen_data_analize(unsigned char *data_buf,unsigned char num);
void ScreenRecSeqInit(void);
State_typedef ScreenRecSeqIn(unsigned char *_DataFrame, unsigned char _DataLen);
State_typedef ScreenRecSeqOut(unsigned char *_DataFrame, unsigned char _DataLen);


#endif
