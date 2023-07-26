#include "reg52.h"			 //���ļ��ж����˵�Ƭ����һЩ���⹦�ܼĴ���

typedef unsigned int u16;	  //���������ͽ�����������
typedef unsigned char u8;

sbit LSA=P2^0;
sbit LSB=P2^1;//����ʱ����ʾ��������

sbit LSC=P2^2;
sbit LSD=P2^3;
sbit LSE=P2^4;//�ȷ֣���ͣ��ʱ��ʾ��������

sbit bee=P2^5;//������������

sbit A_add=P3^0;
sbit B_add=P3^1;//�ȷ����Ӱ�ť

sbit A_sub=P3^2;
sbit B_sub=P3^3;//�ȷּ��ٰ�ť

sbit change = P3^4; // �����ذ�ť

sbit T_add=P3^5;
sbit S_add=P3^6;//��ʱ���ӣ����밴ť

u8 Formal_timer[4];//����ʱ������
u8 Score_Uptime[8];//����������ʱ����

u8 code smgduan[11]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77};//��ʾ0~9��ֵ

u16 ssec,sec=55,min=89,x=0,y=0,k;

u16 osec=0,omin=0,flag=0;

void keyScan();

void delay(u16 i)
{
	while(i--);	
}

void Timer0Init()//��ʱ��0����
{
	TMOD|=0X01;//ѡ��Ϊ��ʱ��0ģʽ��������ʽ1������TR0��������
	TH0=0Xd8;	//����ʱ������ֵ����ʱ10ms
	TL0=0Xf0;	
	ET0=1;//�򿪶�ʱ��0�ж�����
	EA=1;//�����ж�
	TR0=1;//�򿪶�ʱ��			
}

void Timer1Init()//��ʱ��1����
{
	TMOD|=0X10;//ѡ��Ϊ��ʱ��1ģʽ��������ʽ1������TR1��������
	TH1=0Xd8;	//����ʱ������ֵ����ʱ10ms
	TL1=0Xf0;	
	ET1=1;//�򿪶�ʱ��0�ж�����
	EA=1;//�����ж�
	TR1=1;//�򿪶�ʱ��			
}

void formal_time()//����ʱ����ʾ
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
		P0=Formal_timer[i];//���Ͷ���
		delay(100); //���һ��ʱ��ɨ��	
		P0=0x00;//����
	}
}
void time_element()//����ʱ������
	{
	Formal_timer[0]=smgduan[sec%10];
	Formal_timer[1]=smgduan[sec/10];
	Formal_timer[2]=smgduan[min%10];
	Formal_timer[3]=smgduan[min/10];
	}
void score_utime()//�ȷ֣���ͣ��ʱ��ʾ
{
	u8 i;
	for(i=0;i<8;i++)
	{
		switch(i)	 //λѡ��ѡ�����������ܣ�
		{
			case(0):
				LSC=0;LSD=0;LSE=0; break;//��ʾ��0λ
			case(1):
				LSC=1;LSD=0;LSE=0; break;//��ʾ��1λ
			case(2):
				LSC=0;LSD=1;LSE=0; break;//��ʾ��2λ
			case(3):
				LSC=1;LSD=1;LSE=0; break;//��ʾ��3λ
			case(4):
				LSC=0;LSD=0;LSE=1; break;//��ʾ��4λ
			case(5):
				LSC=1;LSD=0;LSE=1; break;//��ʾ��5λ
			case(6):
				LSC=0;LSD=1;LSE=1; break;//��ʾ��6λ
			case(7):
				LSC=1;LSD=1;LSE=1; break;//��ʾ��7λ	
		}
		P1=Score_Uptime[i];//���Ͷ���
		delay(100); //���һ��ʱ��ɨ��	
		P1=0x00;//����
	}
}
void score_element()//�ȷ֣���ͣ��ʱ����
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
	
void main()//������
{	
	
	Timer0Init();  //��ʱ��0��ʼ��
	P3=0xff;
	while(1)
	{
		time_element();
		formal_time();
		keyScan();
		score_element();
		score_utime();
		if(min>89)//����ʱ��������رն�ʱ��0
		{
			ET0=0;
			TR0=0;
			EA=0;
			break;
		}
	}
	min=sec=ssec=0;	
	Timer1Init();
	while(1)//������ͣ��ʱ
	{
		time_element();
		formal_time();
		keyScan();
		score_element();
		score_utime();
		if(flag)//��ͣ��ʱ������־λ
		{
			break;
		}
	}
	while(1)//����ʱ�������
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
	
	while(1)//���ձȷ���ʾ
	{			
		time_element();
		formal_time();
		score_element();
		score_utime();
	}
}


void Timer0() interrupt 1
{
	TH0=0Xd8;	//����ʱ������ֵ����ʱ10ms
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
	TH1=0Xd8;	//����ʱ������ֵ����ʱ10ms
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
void keyScan() //����ɨ�躯��
{
	u16 temp;//�м���� temp�� ��⵽�а�������ʱ���� A,B ���ӷ���
	if(A_add==0)//����⵽ A �Ӽӷְ������£� ��Ӧ�ӷ�
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
	if(change==0) //��������
		{
			temp=x; //��������ֵ�Ե�
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
		score_element(); //�ȴ���ť���ɿ�
	}
}

