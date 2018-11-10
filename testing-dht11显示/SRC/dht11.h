

#ifndef __DHT11_H__
#define __DHT11_H__

#include "STC15W4K.h"
#include "delay.h"

sbit DHT11_Data=P5^4;

extern void DHT11_Start();
extern unsigned char ReceiveDHT11_Data();
extern void GetDHT11_Data();

extern unsigned char tempH,tempL,humyH,humyL,CheckData;

#endif