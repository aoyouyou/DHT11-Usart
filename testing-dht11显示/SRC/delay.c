#include "delay.h"

void Delay1ms(unsigned char t)		//@22.1184MHz
{
	unsigned char i,j,z;
	for(z=t;z>0;z--)
	{
		
		_nop_();
		_nop_();
		i = 22;
		j = 128;
		do
		{
			while (--j);
		} while (--i);
	}	
}



void Delay10us()		//@22.1184MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	_nop_();
	i = 52;
	while (--i);
}

void Delay1000ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 85;
	j = 12;
	k = 155;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void Delay(int cnt)
{
		while(cnt>0)
		{
			Delay1000ms();
			cnt--;
		}
}












