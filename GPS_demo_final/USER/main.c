#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "led.h"
#include "tftlcd.h"
#include "usmart.h"	
#include "usart3.h" 	
#include "key.h" 	 
#include "string.h"	 	 
#include "gps.h"	 
#include "math.h"
#include "timer.h"
/****************************************Copyright (c)****************************************************
 
**--------------File Info---------------------------------------------------------------------------------
** File name:          main.c
** Last modified Date: 2020/11/07
** Created date:       2020/10/19    
** Version:            V1.0
** Descriptions:       Null
**--------------------------------------------------------------------------------------------------------*/
/*				  	 
u8 USART1_TX_BUF[USART3_MAX_RECV_LEN]; 					//串口1,发送缓存区
nmea_msg gpsx; 											//GPS信息
__align(4) u8 dtbuf[50];   								//打印缓存器
const u8*fixmode_tbl[4]={"Fail","Fail"," 2D "," 3D "};	//fix mode字符串 
*/
u8 USART1_TX_BUF[USART3_MAX_RECV_LEN]; 					//串口1,发送缓存区
nmea_msg gpsx; 											//GPS信息
u8 key=0XFF;
u8 flag_draw=0;//绘制轨迹标志位
u8 flag_demo=0;//演示标志位
int main(void)
{ 
	u16 i,rxlen;
	u16 lenx;
	//u8 upload=0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);      	//初始化延时函数
	uart_init(115200);			//初始化串口波特率为115200 
	usart3_init(38400);			//初始化串口3波特率为38400
	usmart_dev.init(84); 		//初始化USMART		
	LED_Init();					//初始化LED
	KEY_Init();					//初始化按键
 	TFTLCD_Init();			 			//初始化LCD
	usmart_dev.init(72); 		//初始化USMART 	 
	TIM3_Init(100-1,8400-1);  //定时100ms	   通用定时器中断TIMER3
	Interface_Display();
	if(Ublox_Cfg_Rate(1000,1)!=0)	//设置定位信息更新速度为1000ms,顺便判断GPS模块是否在位. 
	{
   	//LCD_ShowString(40,140,200,24,24,"NEO-6M Setting...");
		while((Ublox_Cfg_Rate(1000,1)!=0)&&key)	//持续判断,直到可以检查到NEO-6M,且数据保存成功
		{
			usart3_init(9600);				//初始化串口3波特率为9600(EEPROM没有保存数据的时候,波特率为9600.)
	  	Ublox_Cfg_Prt(38400);			//重新设置模块的波特率为38400
			usart3_init(38400);				//初始化串口3波特率为38400
			Ublox_Cfg_Tp(1000000,100000,1);	//设置PPS为1秒钟输出1次,脉冲宽度为100ms	    
			key=Ublox_Cfg_Cfg_Save();		//保存配置  
		}	  					 
	   //LCD_ShowString(40,140,200,24,24,"NEO-6M Set Done!!");
		delay_ms(500);
	   //LCD_Fill(30,120,30+200,120+16,WHITE);//清除显示 
	}
	while(1) 
	{	
		static u16 t=0;
    //timer.c 里的TIM3_IRQHandler()作为定时器中断服务子函数
		delay_ms(1);
		if(USART3_RX_STA&0X8000)		//接收到一次数据了
		{
			rxlen=USART3_RX_STA&0X7FFF;	//得到数据长度
			for(i=0;i<rxlen;i++)USART1_TX_BUF[i]=USART3_RX_BUF[i];	   
 			USART3_RX_STA=0;		   	//启动下一次接收
			USART1_TX_BUF[i]=0;			//自动添加结束符
			GPS_Analysis(&gpsx,(u8*)USART1_TX_BUF);//分析字符串
			/*
			if(flag_draw)
			{
				if(!t) LCD_Clear(WHITE);//判断是否首次进入if，首次则清屏，否则不清
				Draw_Path();    //绘制轨迹
				t++;
				if(t>1000) t=1000;//防止t越界
			}
			else
			{
				LCD_Clear(WHITE);
				//show_chinese(30,140,�",RED,WHITE);  //YELLOW
				Gps_Msg_Show();				//显示信息
			}
			*/
			if(!flag_demo) Gps_Msg_Show();				//显示信息
			//if(upload)printf("\r\n%s\r\n",USART1_TX_BUF);//发送接收到的数据到串口1
 		}
		//接受数据成功则D1闪烁
		if((lenx%100)==0)LED0=!LED0; 	    				 
		lenx++;	
	}
}
	
/*
		if(key==KEY0_PRES)
		{
			upload=!upload;
			FRONT_COLOR=RED;
			if(upload)LCD_ShowString(30,100,200,16,16,"NMEA Data Upload:ON ");
			else LCD_ShowString(30,100,200,16,16,"NMEA Data Upload:OFF");
 		}
	*/	
								    


