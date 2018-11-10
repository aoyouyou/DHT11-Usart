#include "dht11.h"
#include "string.h"
#include "oled.h"

/*************	���س�������	**************/
#define MAIN_Fosc		22118400L	//������ʱ��

#define	RX1_Length	128		/* ���ջ��峤�� */
#define	RX2_Length	128		/* ���ջ��峤�� */
#define	UART_BaudRate1	115200UL	 /* ������ */
#define	UART_BaudRate2	115200UL	 /* ������ */


/*************	���ر�������	**************/
u8	xdata	RX1_Buffer[RX1_Length];	//���ջ���
u8	xdata	RX2_Buffer[RX2_Length];	//���ջ���
u8	TX1_read,RX1_write;	//��д����(ָ��).
u8	TX2_read,RX2_write;	//��д����(ָ��).
bit	B_TX1_Busy,B_TX2_Busy;

sbit nl_lamp=P4^1;
sbit nl_fan=P4^2;


u8 flag1,j=0,i=0;
u8 Table_LCD[]="0123456789.";
u8 humy[4];
u8 temp[4];
u8 lamp_key,fan_key;
/*************	���غ�������	**************/
void	UART1_config(u8 brt);	// ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
void	UART2_config(u8 brt);	// ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
void 	PrintString1(u8 *puts);
void 	PrintString2(u8 *puts);
void 	clear(void);



void PrintByte(u8 dat)
{
        SBUF = dat;
        while(!TI);                                        //�ȴ��������
}

void port_mode()            // �˿�ģʽ
{
	P0M1=0x60; P0M0=0x00;P1M1=0x00; P1M0=0x00;P2M1=0x00; P2M0=0x00;P3M1=0x00; P3M0=0x00;
	P4M1=0x00; P4M0=0x00;P5M1=0x00; P5M0=0x00;P6M1=0x00; P6M0=0x00;P7M1=0x00; P7M0=0x00; 
}
/******��ʼ��************************/
void Init()
{
  port_mode();
	nl_lamp=1;
	UART1_config(1);	// ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
	UART2_config(2);	// ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
	
	EA = 1;						//�����ж�
	flag1=1;
	Delay(5);
	/*********�˴�����OLED��ʾ����*************/
	OLED_Init();	//��ʼ��OLED
	OLED_Clear();
}
/*****************************************/
void main()
{
	u8 val;
	Init();
	while(1)
	{
		 if(strstr((char*)RX2_Buffer,"OK")!=NULL)
		 {
					nl_fan=0;			
		 }
		 /************************************************/
		 /************DHT11����******************/
			GetDHT11_Data();
			val=(tempH+tempL+humyH+humyL)&0xff;
			if(val==CheckData)
			{
				humy[0]=Table_LCD[humyH/10];
				humy[1]=Table_LCD[humyH%10];
				PrintString1("�¶�:");
				PrintString1(temp);
				Delay1000ms();
				OLED_ShowString(20,6,temp);
				OLED_ShowCHinese(40,6,19);//��
				
				temp[0]=Table_LCD[tempH/10];
				temp[1]=Table_LCD[tempH%10];
				PrintString1("ʪ��:");
				PrintString1(humy);
				Delay1000ms();
				OLED_ShowString(75,6,humy);
				OLED_ShowString(95,6,"%RH");
			}
		}
}
		

void	SetTimer2Baudraye(u16 dat)	// ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
{
	AUXR &= ~(1<<4);	//Timer stop
	AUXR &= ~(1<<3);	//Timer2 set As Timer
	AUXR |=  (1<<2);	//Timer2 set as 1T mode
	TH2 = dat / 256;
	TL2 = dat % 256;
	IE2  &= ~(1<<2);	//��ֹ�ж�
	AUXR |=  (1<<4);	//Timer run enable
}

void	UART1_config(u8 brt)	// ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
{
	u8	i;
	/*********** ������ʹ�ö�ʱ��2 *****************/
	if(brt == 2)
	{
		AUXR |= 0x01;		//S1 BRT Use Timer2;
		SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / UART_BaudRate1);
	}

	/*********** ������ʹ�ö�ʱ��1 *****************/
	else
	{
		TR1 = 0;
		AUXR &= ~0x01;		//S1 BRT Use Timer1;
		AUXR |=  (1<<6);	//Timer1 set as 1T mode
		TMOD &= ~(1<<6);	//Timer1 set As Timer
		TMOD &= ~0x30;		//Timer1_16bitAutoReload;
		TH1 = (65536UL - (MAIN_Fosc / 4) / UART_BaudRate1) / 256;
		TL1 = (65536UL - (MAIN_Fosc / 4) / UART_BaudRate1) % 256;
		ET1 = 0;	//��ֹ�ж�
		INT_CLKO &= ~0x02;	//�����ʱ��
		TR1  = 1;
	}
	/*************************************************/

	SCON = (SCON & 0x3f) | (1<<6);	// 8λ����, 1λ��ʼλ, 1λֹͣλ, ��У��
//	PS  = 1;	//�����ȼ��ж�
	ES  = 1;	//�����ж�
	REN = 1;	//�������
	P_SW1 = P_SW1 & 0x3f;	//�л��� P3.0 P3.1
//	P_SW1 = (P_SW1 & 0x3f) | (1<<6);	//�л���P3.6 P3.7
//	P_SW1 = (P_SW1 & 0x3f) | (2<<6);	//�л���P1.6 P1.7 (����ʹ���ڲ�ʱ��)

	for(i=0; i<RX1_Length; i++)		RX1_Buffer[i] = 0;
	B_TX1_Busy  = 0;
	TX1_read    = 0;
	RX1_write   = 0;
}
void	UART2_config(u8 brt)	// ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
{
	u8	i;
	/*********** �����ʹ̶�ʹ�ö�ʱ��2 *****************/
	if(brt == 2)	SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / UART_BaudRate2);

	S2CON &= ~(1<<7);	// 8λ����, 1λ��ʼλ, 1λֹͣλ, ��У��
	IE2   |= 1;			//�����ж�
	S2CON |= (1<<4);	//�������
	P_SW2 &= ~1;		//�л��� P1.0 P1.1
//	P_SW2 |= 1;			//�л��� P4.6 P4.7

	for(i=0; i<RX2_Length; i++)		RX2_Buffer[i] = 0;
	B_TX2_Busy  = 0;
	TX2_read    = 0;
	RX2_write   = 0;
}

void PrintString1(u8 *puts)
{
    for (; *puts != 0;	puts++)
	{
		B_TX1_Busy = 1;		//��־����æ
		SBUF = *puts;		//��һ���ֽ�
		while(B_TX1_Busy);	//�ȴ��������
	}
}

void PrintString2(u8 *puts)
{
    for (; *puts != 0;	puts++)
	{
		B_TX2_Busy = 1;		//��־����æ
		S2BUF = *puts;		//��һ���ֽ�
		while(B_TX2_Busy);	//�ȴ��������
	}
}
/********************* UART1�жϺ���************************/
void UART1_int (void) interrupt UART1_VECTOR
{
	if(RI)
	{
		RI = 0;
		RX1_Buffer[RX1_write] = SBUF;
		if(++RX1_write >= RX1_Length)	RX1_write = 0;
	}

	if(TI)
	{
		TI = 0;
		B_TX1_Busy = 0;
	}
}

/********************* UART2�жϺ���************************/
void UART2_int (void) interrupt UART2_VECTOR
{
	if(RI2)
	{
		CLR_RI2();
		RX2_Buffer[RX2_write] = S2BUF;
		if(++RX2_write >= RX2_Length)	RX2_write = 0;
	}

	if(TI2)
	{
		CLR_TI2();
		B_TX2_Busy = 0;
	}

}
void clear(void)
{  
		int i;
		for(i=0; i<RX2_Length; i++)		
		RX2_Buffer[i] = 0;//������ջ���
}
