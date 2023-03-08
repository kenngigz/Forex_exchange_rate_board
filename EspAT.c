#include "display_slot.h"
#include <intrins.h>
#include <string.h>


#define NELEMS(a)  (sizeof(a) / sizeof((a)[0]))



const char code * code messages[] = {
"0,CONNECT\r\n",    //1
"1,CONNECT\r\n",    //2
"2,CONNECT\r\n",    //3
"3,CONNECT\r\n",    //4
"0,CLOSED\r\n",     //5
"1,CLOSED\r\n",     //6
"2,CLOSED\r\n",     //7
"3,CLOSED\r\n",     //8
"ERROR\r\n",        //9
"OK\r\n",           //10
"ERROR\r\n",        //11
"WIFI GOT IP\r\n",  //12
"+IPD,",            //13
"SEND OK\r\n",        //14
"SEND FAIL\r\n",      //15
"+CIFSR:APIP,",       //16
"+CIFSR:APMAC,",      //17
"+CIFSR:STAIP,",      //18
"+CIFSR:STAMAC,",     //19
"+CIPMUX:",           //20
"WIFI DISCONNECT\r\n", //21	
"STATUS:\r\n",				 //22
"+CWMODE:\r\n"				 //23
};

extern bit pass_accept;
unsigned char pdata epsBuff[64];
unsigned char pdata sndBuff[48];
//unsigned char pdata staip[16];
//unsigned char pdata staip[16];
unsigned char connected,answer;
unsigned char dat,len,cannel;
bit gotip;
static unsigned char indexT;
void  uart() interrupt 4 using 1
{
	static unsigned char indexR;
	unsigned char i,j;
	bit success;
	if (_testbit_(RI0))
	{
		if (dat)
		{
			epsBuff[indexR++]=SBUF0;
			dat--;
			if (dat==0)
				answer=13;
			return;
		}
		if (indexR>=NELEMS(epsBuff)) 
			indexR=NELEMS(epsBuff)-1;
		i=(epsBuff[indexR++]=SBUF0);
		if (i=='\n') //a line received
		{
			if (indexR==2)
			{
				indexR=0;
				return;
			}
			epsBuff[indexR]=0xff;
			indexR=0;
			success=0;
			for (i=0;(i<NELEMS(messages))&&(!success);i++)
			 {
				 j=0;
				 while (messages[i][j]==epsBuff[j])
					 j++;
				 if (messages[i][j]==0)
           success=1;					 					 
			 }
			if (success)
				 switch (i)
			    {
						case 1:
						case 2:
						case 3:
						case 4:
							//connect group
							j=1;
						  j<<=(i-1);
						  connected|=j;
						  led=0;
				//		  change++;
						  pass_accept=0;
						 break;
						case 5:
						case 6:
						case 7:
						case 8:
							//closed group
							j=1;
						  j<<=(i-5);
						  connected&=~j;
							if (connected==0)
						   led=1;
						break;
						case 9: //error
						  answer=9;
						break;
						case 10: //ok
							answer=10;
						break;
						
						case 12: //got ip
							 answer=12;
						   gotip=1;
						break;
						case 16: //apip
							 answer =16;
						 break;
						case 17: //apmac
							answer=17;
						 break;
						case 18: //staip
							answer=18;
						 break;
						case 19: //stamac
							answer=19;
						 break;
						case 21:
							answer=21;
						  gotip=0;
						break;
						default:
							answer=i;
					  break;
					}
			 
		}
		else
		{
			if ((i=='>')&&(indexR==1)) //wait send data to send
			{
				answer=255;
				return;
			}
			
			if (i==':') //espsciali for +IPD
			{
				if (epsBuff[0]!='+')
					return;
				if (epsBuff[1]!='I')
					return;
				if (epsBuff[2]!='P')
					return;
				if (epsBuff[3]!='D')
					return;
				dat=epsBuff[7]&0xf;
				if (indexR!=9)    //not more of 99 bytes
				{
					dat*=10;
					dat+=epsBuff[8]&0xf;
				}
				len=dat;
				indexR=0;
				cannel=epsBuff[5];
			}
		}
	}
	
	if (_testbit_(TI0))
	{

			if ((j=sndBuff[indexT])!=0)
				SBUF0=sndBuff[indexT++];
			else
				indexT=0;

	}
	
}

void initEps(const char code *tbl[])
{
	unsigned char i;

	for (i=0;;i++)
	 {		 
		 answer=0;
		 strcpy(sndBuff,tbl[i]);
		 if (*sndBuff==0)
			 break;
		 TI0=1;
		 while(answer!=10)  //ok
		 {
			 WATCHDOG;	
       if (answer==12) //got ip
        break;				 
		 }			 
	 }		
}

bit SendToESP (const char *comm,unsigned char answ,unsigned char t)
{
	unsigned int timer;
	while(indexT!=0)
		WATCHDOG;	
	strcpy(sndBuff,comm);
	answer=0;
	TI0=1;	
	if (answ==0) // no aswer need
		return 1;
	timer= ((unsigned int) t) * 100;; //set to 2.5 sec
	while(answer!=answ)  //ok
		 {
			 WATCHDOG;	
       if (TF2H) //use timer 2 at 500khz		
			 {
				 TF2H=0;
				 if (timer--==0) 
					 return 0;
			 }				 
		 }
	return 1;
}