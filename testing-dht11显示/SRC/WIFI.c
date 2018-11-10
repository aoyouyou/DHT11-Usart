#include "WIFI.h"






/*************************************************************/
void WIFI(void)
{
	
	RX2_write = 0;PrintString2(WiFi_AT);
	Delay(2);
	PrintString1(RX2_Buffer);   clear();


	RX2_write = 0;PrintString2(WiFi_CWMODE);
	Delay(2);PrintString1(RX2_Buffer);  clear();


	RX2_write = 0;PrintString2(WiFi_CWJAP);
	Delay(6);PrintString1(RX2_Buffer); clear();


	RX2_write = 0;PrintString2(WiFi_CIFSR);
	Delay(3);PrintString1(RX2_Buffer);  clear();


	RX2_write = 0;PrintString2(WiFi_CIPSTART);
	Delay(6);PrintString1(RX2_Buffer); clear();


	RX2_write = 0;PrintString2(WiFi_CIPMODE);
	Delay(2);PrintString1(RX2_Buffer);  clear();


	RX2_write = 0;PrintString2(WiFi_CIPSEND);
	Delay(2);PrintString1(RX2_Buffer);
	if(strstr((char*)RX2_Buffer,"ERROR")!=NULL)
	{
		flag1=1;
	}
	clear();   
	RX2_write = 0;PrintString2(WiFi_CONNECT);
	Delay(3);PrintString1(RX2_Buffer); clear();		
}



