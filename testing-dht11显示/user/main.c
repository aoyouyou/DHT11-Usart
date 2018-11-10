#include "dht11.h"
#include "string.h"
#include "oled.h"

/*************	本地常量声明	**************/
#define MAIN_Fosc		22118400L	//定义主时钟

#define	RX1_Length	128		/* 接收缓冲长度 */
#define	RX2_Length	128		/* 接收缓冲长度 */
#define	UART_BaudRate1	115200UL	 /* 波特率 */
#define	UART_BaudRate2	115200UL	 /* 波特率 */


/*************	本地变量声明	**************/
u8	xdata	RX1_Buffer[RX1_Length];	//接收缓冲
u8	xdata	RX2_Buffer[RX2_Length];	//接收缓冲
u8	TX1_read,RX1_write;	//读写索引(指针).
u8	TX2_read,RX2_write;	//读写索引(指针).
bit	B_TX1_Busy,B_TX2_Busy;

sbit nl_lamp=P4^1;
sbit nl_fan=P4^2;


u8 flag1,j=0,i=0;
u8 Table_LCD[]="0123456789.";
u8 humy[4];
u8 temp[4];
u8 lamp_key,fan_key;
/*************	本地函数声明	**************/
void	UART1_config(u8 brt);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
void	UART2_config(u8 brt);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
void 	PrintString1(u8 *puts);
void 	PrintString2(u8 *puts);
void 	clear(void);



void PrintByte(u8 dat)
{
        SBUF = dat;
        while(!TI);                                        //等待发送完成
}

void port_mode()            // 端口模式
{
	P0M1=0x60; P0M0=0x00;P1M1=0x00; P1M0=0x00;P2M1=0x00; P2M0=0x00;P3M1=0x00; P3M0=0x00;
	P4M1=0x00; P4M0=0x00;P5M1=0x00; P5M0=0x00;P6M1=0x00; P6M0=0x00;P7M1=0x00; P7M0=0x00; 
}
/******初始化************************/
void Init()
{
  port_mode();
	nl_lamp=1;
	UART1_config(1);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
	UART2_config(2);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
	
	EA = 1;						//开总中断
	flag1=1;
	Delay(5);
	/*********此处加入OLED显示程序*************/
	OLED_Init();	//初始化OLED
	OLED_Clear();
	//PrintString1("开始\r");
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
		 /************DHT11部分******************/
			GetDHT11_Data();
			val=(tempH+tempL+humyH+humyL)&0xff;
			if(val==CheckData)
			{
				humy[0]=Table_LCD[humyH/10];
				humy[1]=Table_LCD[humyH%10];
				PrintString1("温度:");
				PrintString1(temp);
				Delay1000ms();
				OLED_ShowString(20,6,temp);
				OLED_ShowCHinese(40,6,19);//℃
				
				temp[0]=Table_LCD[tempH/10];
				temp[1]=Table_LCD[tempH%10];
				PrintString1("湿度:");
				PrintString1(humy);
				Delay1000ms();
				OLED_ShowString(75,6,humy);
				OLED_ShowString(95,6,"%RH");
			}
		}
}
		

void	SetTimer2Baudraye(u16 dat)	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
{
	AUXR &= ~(1<<4);	//Timer stop
	AUXR &= ~(1<<3);	//Timer2 set As Timer
	AUXR |=  (1<<2);	//Timer2 set as 1T mode
	TH2 = dat / 256;
	TL2 = dat % 256;
	IE2  &= ~(1<<2);	//禁止中断
	AUXR |=  (1<<4);	//Timer run enable
}

void	UART1_config(u8 brt)	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
{
	u8	i;
	/*********** 波特率使用定时器2 *****************/
	if(brt == 2)
	{
		AUXR |= 0x01;		//S1 BRT Use Timer2;
		SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / UART_BaudRate1);
	}

	/*********** 波特率使用定时器1 *****************/
	else
	{
		TR1 = 0;
		AUXR &= ~0x01;		//S1 BRT Use Timer1;
		AUXR |=  (1<<6);	//Timer1 set as 1T mode
		TMOD &= ~(1<<6);	//Timer1 set As Timer
		TMOD &= ~0x30;		//Timer1_16bitAutoReload;
		TH1 = (65536UL - (MAIN_Fosc / 4) / UART_BaudRate1) / 256;
		TL1 = (65536UL - (MAIN_Fosc / 4) / UART_BaudRate1) % 256;
		ET1 = 0;	//禁止中断
		INT_CLKO &= ~0x02;	//不输出时钟
		TR1  = 1;
	}
	/*************************************************/

	SCON = (SCON & 0x3f) | (1<<6);	// 8位数据, 1位起始位, 1位停止位, 无校验
//	PS  = 1;	//高优先级中断
	ES  = 1;	//允许中断
	REN = 1;	//允许接收
	P_SW1 = P_SW1 & 0x3f;	//切换到 P3.0 P3.1
//	P_SW1 = (P_SW1 & 0x3f) | (1<<6);	//切换到P3.6 P3.7
//	P_SW1 = (P_SW1 & 0x3f) | (2<<6);	//切换到P1.6 P1.7 (必须使用内部时钟)

	for(i=0; i<RX1_Length; i++)		RX1_Buffer[i] = 0;
	B_TX1_Busy  = 0;
	TX1_read    = 0;
	RX1_write   = 0;
}
void	UART2_config(u8 brt)	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
{
	u8	i;
	/*********** 波特率固定使用定时器2 *****************/
	if(brt == 2)	SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / UART_BaudRate2);

	S2CON &= ~(1<<7);	// 8位数据, 1位起始位, 1位停止位, 无校验
	IE2   |= 1;			//允许中断
	S2CON |= (1<<4);	//允许接收
	P_SW2 &= ~1;		//切换到 P1.0 P1.1
//	P_SW2 |= 1;			//切换到 P4.6 P4.7

	for(i=0; i<RX2_Length; i++)		RX2_Buffer[i] = 0;
	B_TX2_Busy  = 0;
	TX2_read    = 0;
	RX2_write   = 0;
}

void PrintString1(u8 *puts)
{
    for (; *puts != 0;	puts++)
	{
		B_TX1_Busy = 1;		//标志发送忙
		SBUF = *puts;		//发一个字节
		while(B_TX1_Busy);	//等待发送完成
	}
}

void PrintString2(u8 *puts)
{
    for (; *puts != 0;	puts++)
	{
		B_TX2_Busy = 1;		//标志发送忙
		S2BUF = *puts;		//发一个字节
		while(B_TX2_Busy);	//等待发送完成
	}
}
/********************* UART1中断函数************************/
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

/********************* UART2中断函数************************/
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
		RX2_Buffer[i] = 0;//清除接收缓存
}
