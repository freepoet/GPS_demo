#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "led.h"
#include "lcd.h"
#include "usmart.h"	
#include "usart3.h" 	
#include "key.h" 	 
#include "string.h"	 	 
#include "gps.h"	 
#include "math.h"

/****************************************Copyright (c)****************************************************
 
**--------------File Info---------------------------------------------------------------------------------
** File name:          main.c
** Last modified Date: 2020/10/20       实现定位
** Created date:       2020/10/19
** Version:            V1.0
** Descriptions:       null
**--------------------------------------------------------------------------------------------------------*/
				  	 
u8 USART1_TX_BUF[USART3_MAX_RECV_LEN]; 					//串口1,发送缓存区
nmea_msg gpsx; 											//GPS信息
__align(4) u8 dtbuf[50];   								//打印缓存器
const u8*fixmode_tbl[4]={"Fail","Fail"," 2D "," 3D "};	//fix mode字符串 
	  
//显示GPS定位信息 
void Gps_Msg_Show(void)
{
 	float tp;		   
	 	POINT_COLOR=GREEN;
	LCD_ShowString(30,20,200,16,16,"STM32F4NE0-6M GPS TEST");	
	LCD_ShowString(30,40,200,16,16,"Version：1.0");	
	LCD_ShowString(30,60,200,16,16,"Mode   :null");
	//KEY1也就是KEY_DOWN进行功能切换，默认为位置高度速度等显示，按一下切换到运动轨迹绘制
	LCD_ShowString(30,80,200,16,16,"KEY1   :Function Switch");
	
  LCD_ShowString(30,100,200,16,16,"NMEA Data Upload:OFF");  
	POINT_COLOR=BLUE;  	 
	tp=gpsx.longitude;	   
	sprintf((char *)dtbuf,"Longitude:%.5f %1c   ",tp/=100000,gpsx.ewhemi);	//得到经度字符串
 	LCD_ShowString(30,130,200,16,16,dtbuf);	 	   
	tp=gpsx.latitude;	   
	sprintf((char *)dtbuf,"Latitude:%.5f %1c   ",tp/=100000,gpsx.nshemi);	//得到纬度字符串
 	LCD_ShowString(30,150,200,16,16,dtbuf);	 	 
	tp=gpsx.altitude;	   
 	sprintf((char *)dtbuf,"Altitude:%.1fm     ",tp/=10);	    			//得到高度字符串
 	LCD_ShowString(30,170,200,16,16,dtbuf);	 			   
	tp=gpsx.speed;	   
 	sprintf((char *)dtbuf,"Speed:%.3fkm/h     ",tp/=1000);		    		//得到速度字符串	 
 	LCD_ShowString(30,190,200,16,16,dtbuf);	 				    
	if(gpsx.fixmode<=3)														//定位状态
	{  
		sprintf((char *)dtbuf,"Fix Mode:%s",fixmode_tbl[gpsx.fixmode]);	
	  	LCD_ShowString(30,210,200,16,16,dtbuf);			   
	}	 	   
	sprintf((char *)dtbuf,"Valid satellite:%02d",gpsx.posslnum);	 		//用于定位的卫星数
 	LCD_ShowString(30,230,200,16,16,dtbuf);	    

	sprintf((char *)dtbuf,"Visible satellite:%02d",gpsx.svnum%100);	 		//可见卫星数
 	LCD_ShowString(30,250,200,16,16,dtbuf);		 
	sprintf((char *)dtbuf,"UTC Date:%04d/%02d/%02d   ",gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//显示UTC日期
	//printf("year2:%d\r\n",gpsx.utc.year);
	LCD_ShowString(30,270,200,16,16,dtbuf);		    
	sprintf((char *)dtbuf,"UTC Time:%02d:%02d:%02d   ",gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//显示UTC时间
  	LCD_ShowString(30,290,200,16,16,dtbuf);		  
}
 double  hypot( double  x,  double  y)  {
      return sqrt(x * x + y * y);
}
//经纬度坐标间距离换算为实际大地上的距离
double  distance( double  latitude1,  double  longitude1,  double latitude2,  double  longitude2) 
{
      double x, y, out;
      double PI = 3.1415926535898;
      double R = 6.371229 * 1e6;
       x = (longitude2 - longitude1) * PI * R * cos( ( (latitude1 + latitude2) / 2) * PI / 180) / 180;
       y = (latitude2 -latitude1) * PI * R / 180;
       out = hypot(x, y);
       return out;
}
//运动轨迹绘制函数
void Draw_Path(void)
{
	u32 xt,yt,delta_x,delta_y;//gpsx.longitude latitude是u32类型的 要统一
	delay_ms(50);
	LCD_Clear(WHITE);

	xt=lcddev.width/2;
	yt=lcddev.height/2;
	//GPS当前位置和LCD屏幕中心位置相统一
	
	while(1)
	{
		LCD_Fast_DrawPoint(xt,yt,RED);
		delta_x= gpsx.longitude-xt;     //
		delta_y= gpsx.latitude-yt;
		xt=xt+delta_x;
		yt=yt+delta_y;
	}
	
	

}


int main(void)
{ 
	u16 i,rxlen;
	u16 lenx;
	u8 key=0XFF;
	u8 upload=0;
	u8 flag_draw=0;//绘制轨迹标志位
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);      	//初始化延时函数
	uart_init(115200);			//初始化串口波特率为115200 
	usart3_init(38400);			//初始化串口3波特率为38400
	usmart_dev.init(84); 		//初始化USMART		
	LED_Init();					//初始化LED
	KEY_Init();					//初始化按键
 	LCD_Init();		 			//初始化LCD
	usmart_dev.init(72); 		//初始化USMART 	  
 	POINT_COLOR=GREEN;
	LCD_ShowString(30,20,200,16,16,"STM32F4：NE0-6M GPS TEST");	
	LCD_ShowString(30,40,200,16,16,"Version：1.0");	
	LCD_ShowString(30,60,200,16,16,"Mode   :null");
	LCD_ShowString(30,80,200,16,16,"KEY1   :Function Switch");   	 										   	   
   	LCD_ShowString(30,100,200,16,16,"NMEA Data Upload:OFF");   
	if(Ublox_Cfg_Rate(1000,1)!=0)	//设置定位信息更新速度为1000ms,顺便判断GPS模块是否在位. 
	{
   		LCD_ShowString(30,120,200,16,16,"NEO-6M Setting...");
		while((Ublox_Cfg_Rate(1000,1)!=0)&&key)	//持续判断,直到可以检查到NEO-6M,且数据保存成功
		{
			usart3_init(9600);				//初始化串口3波特率为9600(EEPROM没有保存数据的时候,波特率为9600.)
	  		Ublox_Cfg_Prt(38400);			//重新设置模块的波特率为38400
			usart3_init(38400);				//初始化串口3波特率为38400
			Ublox_Cfg_Tp(1000000,100000,1);	//设置PPS为1秒钟输出1次,脉冲宽度为100ms	    
			key=Ublox_Cfg_Cfg_Save();		//保存配置  
		}	  					 
	   	LCD_ShowString(30,120,200,16,16,"NEO-6M Set Done!!");
		delay_ms(500);
		LCD_Fill(30,120,30+200,120+16,WHITE);//清除显示 
	}
	while(1) 
	{		
		delay_ms(1);
		if(USART3_RX_STA&0X8000)		//接收到一次数据了
		{
			rxlen=USART3_RX_STA&0X7FFF;	//得到数据长度
			for(i=0;i<rxlen;i++)USART1_TX_BUF[i]=USART3_RX_BUF[i];	   
 			USART3_RX_STA=0;		   	//启动下一次接收
			USART1_TX_BUF[i]=0;			//自动添加结束符
			GPS_Analysis(&gpsx,(u8*)USART1_TX_BUF);//分析字符串
			//if(flag_draw)
				//Draw_Path();    //绘制轨迹
			//else
				Gps_Msg_Show();				//显示信息

			if(upload)printf("\r\n%s\r\n",USART1_TX_BUF);//发送接收到的数据到串口1
 		}
		/*
		key=KEY_Scan(0);
		if(key==KEY1_PRES)
			{
				flag_draw=!flag_draw;
				POINT_COLOR=RED;


			
			}
     
		
		if(key==KEY0_PRES)
		{
			upload=!upload;
			POINT_COLOR=RED;
			if(upload)LCD_ShowString(30,100,200,16,16,"NMEA Data Upload:ON ");
			else LCD_ShowString(30,100,200,16,16,"NMEA Data Upload:OFF");
 		}
		*/

		if((lenx%500)==0)LED0=!LED0; 	    				 
		lenx++;	
	}									    
}

