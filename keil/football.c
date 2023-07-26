#include "reg52.h"			 //此文件中定义了单片机的一些特殊功能寄存器

typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;

sbit LSA=P2^0;
sbit LSB=P2^1;//常规时间显示控制引脚

sbit LSC=P2^2;
sbit LSD=P2^3;
sbit LSE=P2^4;//比分，伤停补时显示控制引脚

sbit bee=P2^5;//结束哨音引脚

sbit A_add=P3^0;
sbit B_add=P3^1;//比分增加按钮

sbit A_sub=P3^2;
sbit B_sub=P3^3;//比分减少按钮

sbit change = P3^4; // 换场地按钮

sbit T_add=P3^5;
sbit S_add=P3^6;//补时增加，分秒按钮

u8 Formal_timer[4];//常规时间数组
u8 Score_Uptime[8];//进球数，补时数组

u8 code smgduan[11]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77};//显示0~9的值

u16 ssec,sec=55,min=89,x=0,y=0,k;

u16 osec=0,omin=0,flag=0;

void keyScan();

void delay(u16 i)
{
	while(i--);	
}

void Timer0Init()//定时器0设置
{
	TMOD|=0X01;//选择为定时器0模式，工作方式1，仅用TR0打开启动。
	TH0=0Xd8;	//给定时器赋初值，定时10ms
	TL0=0Xf0;	
	ET0=1;//打开定时器0中断允许
	EA=1;//打开总中断
	TR0=1;//打开定时器			
}

void Timer1Init()//定时器1设置
{
	TMOD|=0X10;//选择为定时器1模式，工作方式1，仅用TR1打开启动。
	TH1=0Xd8;	//给定时器赋初值，定时10ms
	TL1=0Xf0;	
	ET1=1;//打开定时器0中断允许
	EA=1;//打开总中断
	TR1=1;//打开定时器			
}

void formal_time()//常规时间显示
{
	u8 i;
	for(i=0;i<4;i++)
	{
		switch(i)
		{
			case(0):
				LSA=0;LSB=0;break;
			case(1):
				LSA=1;LSB=0;break;
			case(2):
				LSA=0;LSB=1;break;
			case(3):
				LSA=1;LSB=1;break;
		}
		P0=Formal_timer[i];//发送段码
		delay(100); //间隔一段时间扫描	
		P0=0x00;//消隐
	}
}
void time_element()//常规时间数组
	{
	Formal_timer[0]=smgduan[sec%10];
	Formal_timer[1]=smgduan[sec/10];
	Formal_timer[2]=smgduan[min%10];
	Formal_timer[3]=smgduan[min/10];
	}
void score_utime()//比分，伤停补时显示
{
	u8 i;
	for(i=0;i<8;i++)
	{
		switch(i)	 //位选，选择点亮的数码管，
		{
			case(0):
				LSC=0;LSD=0;LSE=0; break;//显示第0位
			case(1):
				LSC=1;LSD=0;LSE=0; break;//显示第1位
			case(2):
				LSC=0;LSD=1;LSE=0; break;//显示第2位
			case(3):
				LSC=1;LSD=1;LSE=0; break;//显示第3位
			case(4):
				LSC=0;LSD=0;LSE=1; break;//显示第4位
			case(5):
				LSC=1;LSD=0;LSE=1; break;//显示第5位
			case(6):
				LSC=0;LSD=1;LSE=1; break;//显示第6位
			case(7):
				LSC=1;LSD=1;LSE=1; break;//显示第7位	
		}
		P1=Score_Uptime[i];//发送段码
		delay(100); //间隔一段时间扫描	
		P1=0x00;//消隐
	}
}
void score_element()//比分，伤停补时数组
	{
	Score_Uptime[0]=smgduan[x%10];
	Score_Uptime[1]=smgduan[x/10];
	Score_Uptime[2]=smgduan[y%10];
	Score_Uptime[3]=smgduan[y/10];
	Score_Uptime[4]=smgduan[osec%10];
	Score_Uptime[5]=smgduan[osec/10];
	Score_Uptime[6]=smgduan[omin%10];
	Score_Uptime[7]=smgduan[omin/10];
	}
	
void main()//主函数
{	
	
	Timer0Init();  //定时器0初始化
	P3=0xff;
	while(1)
	{
		time_element();
		formal_time();
		keyScan();
		score_element();
		score_utime();
		if(min>89)//常规时间结束，关闭定时器0
		{
			ET0=0;
			TR0=0;
			EA=0;
			break;
		}
	}
	min=sec=ssec=0;	
	Timer1Init();
	while(1)//进行伤停补时
	{
		time_element();
		formal_time();
		keyScan();
		score_element();
		score_utime();
		if(flag)//伤停补时结束标志位
		{
			break;
		}
	}
	while(1)//结束时候的哨音
	{
		EA=0;
		for(k=2000;k>0;k--)
		{
		bee=~bee;
		delay(100);
		}
		break;
	}
	min=90+omin;
	sec=osec;
	
	while(1)//最终比分显示
	{			
		time_element();
		formal_time();
		score_element();
		score_utime();
	}
}


void Timer0() interrupt 1
{
	TH0=0Xd8;	//给定时器赋初值，定时10ms
	TL0=0Xf0;
	ssec++;
	if(ssec>=100)  //1s
	{
		ssec=0;
		sec++;
		if(sec>=60)
		{
			sec=0;
			min++;
		}	
	}	
}
void Timer1() interrupt 3
{
	TH1=0Xd8;	//给定时器赋初值，定时10ms
	TL1=0Xf0;
	ssec++;
	if(ssec>=100)  //1s
	{
		ssec=0;
		sec++;
		if(sec>=60)
		{
			sec=0;
			min++;
		}	
	}
		if(sec==osec&&min==omin)
	{
		flag=1;
		TR1=0;
		EA=0;
		ET1=0;
	}
}
void keyScan() //按键扫描函数
{
	u16 temp;//中间变量 temp， 检测到有按键按下时交换 A,B 两队分数
	if(A_add==0)//当检测到 A 队加分按键按下， 对应加分
	{
				x++;
	}
	if(B_add==0)
	{
				y++;
	}
	if(A_sub==0)
	{	
			if(x>=1) x--;
	}
	if(B_sub==0)
	{
			if(y>=1)y--;
	}
	if(T_add==0)
	{
		omin++;
	}
	if(S_add==0)
	{
		osec+=30;
	}
	if(change==0) //交换场地
		{
			temp=x; //两个分数值对调
			x=y;
			y=temp;
		}
		if(osec>59)
	{
		omin=omin+osec/60;
		osec=osec%60;
	}
		while(A_add ==0 || B_add ==0 || A_sub ==0 || B_sub==0 || change==0|| T_add==0|| S_add==0)
	{
		score_element(); //等待按钮被松开
	}
}

