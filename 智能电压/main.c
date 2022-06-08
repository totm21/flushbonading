#include<STC15F2K60S2.H>
#include"ds1302.h"
#include"seg.h"
#include"key.h"
#include"iic.h"

unsigned char Show_Led_Duan[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char Show_Led_true[8]={"        "};
unsigned char Time_main[3]={23,59,55};
unsigned char Data_24c02[7]={0x00,20,10,0,0,0,0};
int ADC_number=0;
bit flag_Time=0;
char show_flag_time=0;
int Static_flag=0;      //初始为状态0 时间显示状态为1 电压状态为2 时间修改为3 电压上下限修改为4 上次溢出为5
char flag_set_time=0;
bit flag_set_time_up=0;
char flag_ti_up_H_L=0;
void DelayUs2x(unsigned char t)
{   
	while(--t);
}
void DelayMs(int t)
{    
	while(t--)
	{
		DelayUs2x(245);
		DelayUs2x(245);
	}
}

void read_24c02()
{
    TR0=0;
    EEPROM_Read(Data_24c02,0x00,1);
    if(Data_24c02[0]!=0x00){
        Data_24c02[0]=0x00;
        EEPROM_Write(Data_24c02,0x00,7);
    }
    else
    {
        EEPROM_Read(Data_24c02,0x00,7);
    }
    TR0=1;
}

void write_24c02()
{
    TR0=0;
    EEPROM_Write(Data_24c02,0x00,7);
    TR0=1;
}


void Set_INIT0()
{
    EA=ET0=1;
    TH0=(65535-1000)/256;
    TL0=(65535-1000)%256;
    TR0=1;
}
void Play_INIT0() interrupt 1
{
    static int t=0;
    TH0=(65535-1000)/256;
    TL0=(65535-1000)%256;
    Seg_Disp(Show_Led_Duan,show_flag_time);
    show_flag_time++;
    if(show_flag_time==8)
    {
        show_flag_time=0;
    }
    t++;
    if(t==1000)
    {
        flag_set_time_up=~flag_set_time_up;
        flag_Time=1;
        t=0;
    }
}







void Refresh_Led()
{
    float now_number=(ADC_number*50.0/255.0)*100.0;
    bit flag_now=0;
    if(Static_flag==1)
    {
        Show_Led_true[0]=Time_main[0]/10+'0';
        Show_Led_true[1]=Time_main[0]%10+'0';
        Show_Led_true[2]='-';
        Show_Led_true[3]=Time_main[1]/10+'0';
        Show_Led_true[4]=Time_main[1]%10+'0';
        Show_Led_true[5]='-';
        Show_Led_true[6]=Time_main[2]/10+'0';
        Show_Led_true[7]=Time_main[2]%10+'0';
    }
    else if(Static_flag==2)
    {
        Show_Led_true[0]='-';
        Show_Led_true[1]='1';
        Show_Led_true[2]='-';
        Show_Led_true[3]=' ';
        if((int)now_number/1000%10==0&&!flag_now)
        {
            Show_Led_true[4]=' ';
        }
        else
        {
            Show_Led_true[4]=now_number/1000+'0';
            flag_now=1;
        }

        if((int)now_number/100%10==0&&!flag_now)
        {
            Show_Led_true[5]=' ';
        }
        else
        {
            Show_Led_true[5]=(int)now_number/100%10+'0';
            flag_now=1;
        }

        if((int)now_number%100/10==0&&!flag_now)
        {
            Show_Led_true[6]=' ';
        }
        else
        {
            Show_Led_true[6]=(int)now_number%100/10+'0';
            flag_now=1;
        }
        Show_Led_true[7]=(int)now_number%10+'0';
    }
    else if(Static_flag==3)
    {
        Show_Led_true[0]=Time_main[0]/10+'0';
        Show_Led_true[1]=Time_main[0]%10+'0';
        if(flag_set_time==0&&!flag_set_time_up)
        {
            Show_Led_true[0]=' ';
            Show_Led_true[1]=' ';
        }
        Show_Led_true[2]='-';
        Show_Led_true[3]=Time_main[1]/10+'0';
        Show_Led_true[4]=Time_main[1]%10+'0';
        if(flag_set_time==1&&!flag_set_time_up)
        {
            Show_Led_true[3]=' ';
            Show_Led_true[4]=' ';
        }
        Show_Led_true[5]='-';
        Show_Led_true[6]=Time_main[2]/10+'0';
        Show_Led_true[7]=Time_main[2]%10+'0';
        if(flag_set_time==2&&!flag_set_time_up)
        {
            Show_Led_true[6]=' ';
            Show_Led_true[7]=' ';
        }
    }
    else if(Static_flag==4)
    {
        Show_Led_true[0]=Data_24c02[1]/10+'0';
        Show_Led_true[1]=Data_24c02[1]%10+'0';
        Show_Led_true[2]='0';
        Show_Led_true[3]='0';
        if(flag_set_time==0&&!flag_set_time_up)
        {
            Show_Led_true[0]=' ';
            Show_Led_true[1]=' ';
            Show_Led_true[2]=' ';
            Show_Led_true[3]=' ';
        }
        Show_Led_true[4]=Data_24c02[2]/10+'0';
        Show_Led_true[5]=Data_24c02[2]%10+'0';
        Show_Led_true[6]='0';
        Show_Led_true[7]='0';
        if(flag_set_time==1&&!flag_set_time_up)
        {
            Show_Led_true[4]=' ';
            Show_Led_true[5]=' ';
            Show_Led_true[6]=' ';
            Show_Led_true[7]=' ';
        }
        
    }
    else if(Static_flag==5)
    {
        Show_Led_true[0]=Data_24c02[3]/10+'0';
        Show_Led_true[1]=Data_24c02[3]%10+'0';
        Show_Led_true[2]='-';
        Show_Led_true[3]=Data_24c02[4]/10+'0';
        Show_Led_true[4]=Data_24c02[4]%10+'0';
        Show_Led_true[5]='-';
        Show_Led_true[6]=Data_24c02[5]/10+'0';
        Show_Led_true[7]=Data_24c02[5]%10+'0';
        if(flag_set_time==1)
        {
            Show_Led_true[0]=' ';
            Show_Led_true[1]=' ';
            Show_Led_true[2]=' ';
            Show_Led_true[3]=' ';
            Show_Led_true[4]=' ';
            Show_Led_true[5]=' ';
            Show_Led_true[6]=Data_24c02[6]/10+'0';
            Show_Led_true[7]=Data_24c02[6]%10+'0';
        }
    }
    Seg_Tran(Show_Led_true,Show_Led_Duan);
}


bit key_y_n()
{
	P3=0x0f;
	P44=0;
	P42=0;
	if((P3&0x0f)==0x0f)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}






int main()
{
    unsigned char static_point=0;
    Set_RTC(Time_main);
    read_24c02();
    Set_INIT0();
    while(1)
    {
        if(key_y_n())
        {
            DelayMs(5);
            if(key_y_n())
            {

                static_point=Key_Read();
                if(static_point==7)
                {
                    Static_flag=1;
                }
                else if(static_point==6)
                {
                    Static_flag=2;
                }
                else if(static_point==4)
                {

                    if(Static_flag==1)
                    {
                        Static_flag=3;
                        flag_set_time=0;
                    }
                    else if(Static_flag==2)
                    {
                        Static_flag=4;
                    }
                    else if(Static_flag==3)
                    {
                        flag_set_time++;
                        flag_set_time%=3;
                    }
                    else if(Static_flag==4)
                    {
                        flag_set_time++;
                        flag_set_time%=2;
                    }
                    else if(Static_flag==5)
                    {
                        flag_set_time++;
                        flag_set_time%=2;
                    }
                }
                else if(static_point==11)   //加
                {
                    if(Static_flag==3)
                    {
                        Time_main[flag_set_time]++;
                        Set_RTC(Time_main);
                    }
                    else if(Static_flag==4)
                    {
                        if(flag_set_time==0)
                        {
                            Data_24c02[1]+=5;
                            if(Data_24c02[1]>50)
                            {
                                Data_24c02[1]-=5;
                            }
                        }
                        else if(flag_set_time==1)
                        {
                            Data_24c02[2]+=5;
                            if(Data_24c02[2]>=Data_24c02[1])
                            {
                                Data_24c02[2]-=5; 
                            }
                        }
                        write_24c02();
                    }
                }
                else if(static_point==10)   //减
                {
                    if(Static_flag==3)
                    {
                        Time_main[flag_set_time]--;
                        Set_RTC(Time_main);
                    }
                    else if(Static_flag==4)
                    {
                        if(flag_set_time==0)
                        {
                            Data_24c02[1]-=5;
                            if(Data_24c02[1]<=Data_24c02[2])
                            {
                                Data_24c02[1]+=5;
                            }
                        }
                        else if(flag_set_time==1)
                        {
                            Data_24c02[2]-=5;
                            if(Data_24c02[2]<0)
                            {
                                Data_24c02[2]+=5;
                            }
                        }
                        write_24c02();
                    }
                }
                else if(static_point==9)
                {
                    Static_flag=5;
                }
                while((P3)!=0x0f)
			    {
                    P3=0x0f;
	                P44=0;
	                P42=0;
			    }
                Refresh_Led();
            }
        }
        if(flag_Time==1)
        {
            ADC_number=PCF8591_Adc();
            flag_Time=0;
            Read_RTC(Time_main);
            TR0=0;
            if(Time_main[0]==23&&Time_main[1]==59&&Time_main[2]==55)
            {
                if(flag_ti_up_H_L==0)
                {
                    flag_ti_up_H_L++;
                }
            }
            if(Time_main[0]==23&&Time_main[1]==59&&Time_main[2]==56)
            {
                if(flag_ti_up_H_L==1)
                {
                    flag_ti_up_H_L++;
                }
            }

            if((ADC_number*50.0/255.0)>(float)Data_24c02[1])
            {
                if(Time_main[0]!=23&&Time_main[1]!=59&&(Time_main[2]!=55||Time_main[2]!=56)&&flag_ti_up_H_L!=2){
                    Data_24c02[3]=Time_main[0];
                    Data_24c02[4]=Time_main[1];
                    Data_24c02[5]=Time_main[2];
                    Data_24c02[6]=1;
                    write_24c02();
                }
            }
            if((ADC_number*50.0/255.0)<(float)Data_24c02[2])
            {
                if(Time_main[0]!=23&&Time_main[1]!=59&&(Time_main[2]!=55||Time_main[2]!=56)){
                    Data_24c02[3]=Time_main[0];
                    Data_24c02[4]=Time_main[1];
                    Data_24c02[5]=Time_main[2];
                    Data_24c02[6]=0;
                    write_24c02();
                }
            }
            TR0=1;
            Refresh_Led();
        }
    }
}