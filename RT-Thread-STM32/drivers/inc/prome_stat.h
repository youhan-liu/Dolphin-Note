#ifndef __PROME_STAT_H
#define __PROME_STAT_H
#include "stm32f10x.h"
#include <rtthread.h>
extern u8 WIFI_Socket_Flag;    //网络状态变量 0->都未连接，1->WiFi连接、手机未连接，2->WiFi连接、手机连接
extern u8 SNP_STAT_Flag;       //拍照网络变量 0->不可拍照, 1->可以拍照 2->拍照中
extern u8 REC_STAT_Flag;       //录像网络变量 0->不可录制  1->可以录制 2->录制中
extern u8 STAT_STAT_Flag;      //硬件运行状态变量    0->正常 1->硬件错误 2->队列容量满 3->WIFI进入AP模式
extern u8 BEEP_STAT_Flag;      //队列状态变量 0->不响 1-> 响1声 2->响2声 3->长响3秒
extern u8 ERR_Status;          //硬件错误  00->正常 D1-> 上次队列中有残留数据 E1->数据队列溢出 E2->触屏工作不正常 E4->数据缓存不正常 E8->外部电压不足
static unsigned char heart_beat_lose_cnt = 0;
#endif
