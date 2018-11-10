#ifndef __8266_H_
#define __8266_H_
typedef unsigned char BYTE;
typedef unsigned int WORD;

//unsigned char config[]={"AT+MCONFIG=\"869300031657095\",\"5qx9dzUzHMvls4e9kMAO0010066c00\",\"355d19eef2104fb5bb28f8caea0b8b3a\"\x00D\x00A"};
//-------------����Ϊ���ܶ���------------------------------------------------------------
BYTE code  WiFi_AT[]={"AT\x00D\x00A"};	                				//��ʽ�����
BYTE code  WiFi_CWMODE[]={"AT+CWMODE=1\x00D\x00A"};	          	//����ģ�鹤��ģʽ��
																																//1��STAģʽ 2��APģʽ 3��STA��APģʽ
BYTE code  WiFi_CWJAP[]={"AT+CWJAP_DEF=\"aoaoao\",\"1412241450\"\x00D\x00A"};	//����·����
BYTE code  WiFi_CIFSR[]={"AT+CIFSR\x00D\x00A"};	//
BYTE code  WiFi_CIPSTART[]={"AT+CIPSTART=\"TCP\",\"120.77.58.34\",8700\x00D\x00A"};	//
BYTE code  WiFi_CIPMODE[]={"AT+CIPMODE=1\x00D\x00A"};	
BYTE code  WiFi_CIPSEND[]={"AT+CIPSEND\x00D\x00A"};               				 

BYTE code  WiFi_CONNECT[]={"{\"t\": 1,\"device\": \"STC15IOT\",\"key\":\"0338ba0fadc340c88b051f231388a05f\",\"ver\":\"v1.1\"}"};         
BYTE   WiFi_UPDATA[]={"{\"t\": 3,\"datatype\":1,\"datas\":{\"temp\":\"23\",\"humy\":\"46\"},\"msgid\": 123}"}; 


//void WIFI(void);

#endif
