#include "display_slot.h"
#include <intrins.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>


#define ESPINT

bit dir;
void mbiSend();
extern unsigned char xdata screen[];
extern union my_data idata my_data;
#define BALLS 0
#define START_TIME  17
#define FULL_TIME   21
void ptr(char);

void SetPoint(unsigned char b,bit sr);
void gotoxy(unsigned char x,unsigned char y);
void initEps(const char code *tbl[]);
bit SendToESP (const char *comm,unsigned char answ,unsigned char t); //wait 3 seconds for answer
extern unsigned char connected,answer,len,cannel;
extern unsigned char pdata epsBuff[];
extern unsigned char pdata sndBuff[];
 long t;
 
const char code  * code command0[]={ //no internet
	"AT\r\n",
	"ATE0\r\n",	
	"AT+CIPMUX=1\r\n",
	"AT\r\n",
	"AT+CIPSERVER=1,333\r\n",	
	"AT+CIPSTO=0\r\n",
	""
};

const char code  * code command1[]={ //internet exist
	"AT\r\n",
	"ATE0\r\n",
	"AT+CIPMUX=1\r\n",
	"AT\r\n",
	"AT+CIPSERVER=1,333\r\n",	
	"AT+CIPSTO=0\r\n",	
	""
};
/*
  	"AT+CWJAP=\"Mitko-home\",\"mozart13\"\r\n",
  	"AT+CWSAP=\"KenForex\",\"1234test\",5,3\r\n",
*/
const char code  * code command2[]={ //no internet
	"AT\r\n",
	"ATE0\r\n",	
	"AT+CIPMUX=1\r\n",
	"AT\r\n",
	"AT+CIPSERVER=1,333\r\n",	
	"AT+CIPSTO=0\r\n",
	""
};

const char code  * code command3[]={
	"ATE0\r\n",
  "AT+CWMODE=2\r\n",
	//"AT+CWJAP=\"Forex\",\"counti2018\"\r\n",
  "AT+CWSAP=\"Start\",\"\",1,0\r\n",
	//"AT+CWJAP=\"Mitko-home\",\"mozart13\"\r\n",
  //"AT+CWSAP=\"Forex\",\"1234test\",5,3\r\n",
	""
};

typedef struct
{
    unsigned char second; // 0-59
    unsigned char minute; // 0-59
    unsigned char hour;   // 0-23
    unsigned char day;    // 1-31
    unsigned char month;  // 1-12
    unsigned char year;   // 0-99 (representing 2000-2099)
} date_time_t;

const unsigned code days[4][12] =
{
    {   0,  31,  60,  91, 121, 152, 182, 213, 244, 274, 305, 335},
    { 366, 397, 425, 456, 486, 517, 547, 578, 609, 639, 670, 700},
    { 731, 762, 790, 821, 851, 882, 912, 943, 974,1004,1035,1065},
    {1096,1127,1155,1186,1216,1247,1277,1308,1339,1369,1400,1430},
};



date_time_t tim;
bit pass_accept;
char pdata combuff[48]; 
char pdata helpbuff[48];
struct currency cc;

void date_time(date_time_t* date_time,unsigned long epoch)
{
	  unsigned int years,year,month;
	  epoch-=946684800L;
    date_time->second = epoch%60; epoch /= 60;
    date_time->minute = epoch%60; epoch /= 60;
    date_time->hour   = epoch%24; epoch /= 24;
    years = epoch/(365*4+1); epoch %= 365*4+1;
		years*=4;
    for (year=3; year>0; year--)
    {
        if (epoch >= days[year][0])
            break;
    }
    for (month=11; month>0; month--)
    {
        if (epoch >= days[year][month])
            break;
    }

    //date_time->year  = years+year+70;
		date_time->year  = years+year;
    date_time->month = month+1;
    date_time->day   = epoch-days[year][month]+1;
}

void  write_time()
	{
		gotoxy(0,1);
		ptr(tim.hour/10+0x20);
		ptr(tim.hour%10+0x20);
		gotoxy(4,1);
		ptr(tim.minute/10+0x20);
		ptr(tim.minute%10+0x20);
	}
	
 void write_date()
	{
		gotoxy(0,0);
		ptr(tim.day/10+0x20);
		ptr(tim.day%10+0xa0);
		ptr(tim.month/10+0x20);
		ptr(tim.month%10+0xa0);
		ptr((tim.year%100)/10+0x20);
		ptr(tim.year%10+0x20);
	}
	
void time_wait(unsigned char tim)
{
 t=rtc_readTime();
 t+=tim;
 while (t!=rtc_readTime())
	 WATCHDOG;
}	

void prnval(unsigned char v)
 {
	 unsigned char p,i,j;
	 
	 i=v*sizeof(struct currency);
	 WriteI2C(EEADDR,&i,1);
	 ReadI2C(EEADDR,(char *) &cc,sizeof(struct currency));
	 p=cc.point;
	 if (v>=NUMOFCUR)
		 v=0x10;
	 else
		 v=0;
	 for (i=0;i<3;i++)
	 {
		 j=cc.vals[i];
		 if ((p&1)==0)
			 ptr((j>>4)&0xf+v);
		 else
			 ptr(((j>>4)&0x0f)|0x80+v);
		 p>>=1;
		 if ((p&1)==0)
			 ptr(j&0xf+v);
		 else
			 ptr((j&0x0f)|0x80+v);	
     p>>=1;		 
	 }
 }
 
void asciival(unsigned char v,char *k)
 {
	 unsigned char p,i,j;
	 while (*k!=0)
		 k++;
	 v*=sizeof(struct currency);
	 WriteI2C(EEADDR,&v,1);
	 ReadI2C(EEADDR,(char *) &cc,sizeof(struct currency));
	 p=cc.point;
	 for (i=0;i<3;i++)
	 {
		 j=cc.vals[i];
		 *k=((j>>4)&0xf)+'0';
		 if (*k>'9')
			 *k=(*k==':')?'-':' ';
		 k++;
		 if ((p&1)!=0)
			 *k++='.';
		 p>>=1;
		 *k=(j&0xf)+'0';
		 if (*k>'9')
			 *k=(*k==':')?'-':' ';
		 k++;
		 if ((p&1)!=0)
			 *k++='.';
     p>>=1;		 
	 }	 
   *k=0;
 }

void itoa(unsigned char value,char *desitination)
{
while(*desitination)
	desitination++;
	desitination[0] = '0';
	desitination[1] = '0';
	desitination[3] = '\0';//you at least 4 char array, last char is NULL
	while (value >= 100)
	{
    desitination[0]++;
    value -= 100;
	}
	while (value >= 10)
	{
     desitination[1]++;
     value -= 10;
	}
	value+= '0';
	desitination[2] =value;
}

bit printto(unsigned char *str,unsigned char can)
{
    char xdata hlpbuff[32]; 
	
		strcpy(hlpbuff,"AT+CIPSEND=");
		strncat(hlpbuff,(char *) &can,1);
  	strcat(hlpbuff,",");								
	  itoa(strlen(str),hlpbuff);
		strcat(hlpbuff,"\r\n");
	  if (SendToESP(hlpbuff,255,8))   //wait 8 seconds for connectinp
	   {	
	       if (SendToESP(	str,14,8))   //wait 5 seconds for directly wait for time
					 return 1;
		 }
		return 0;
}


unsigned char com_stat;
//communication status
// 0 - wait HI! or password

void main() 
{

  int	i; 
  unsigned char j,k;	
	Init_Device();
	SPI0CKR   = SIOSPD; //set the speed of sio
	WATCHDOG;	
  rtc_init();
	WATCHDOG;
	sel0=0;
  sel1=0;
	for (i=0;i<(NUM_OF_MBI*NUM_OF_COLUMNS*16);i++)
	 screen[i]=0x0;


  ESPI0=1;
	SPIF=1;
//-----------------------------------------------
// I2C
 WATCHDOG;
 init_i2c();
/*
 my_data.rtc_ram[0]=22;
 my_data.rtc_ram[1]=0x41;
 my_data.rtc_ram[2]=0x92;
 my_data.rtc_ram[3]=0x14;
 my_data.rtc_ram[4]=0x25;
 WriteI2C(EEADDR,my_data.rtc_ram,5);
 my_data.rtc_ram[0]=22;
 WriteI2C(EEADDR,my_data.rtc_ram,1);
 WATCHDOG;
 ReadI2C(EEADDR,my_data.rtc_ram,2);
 while (ESPI0)
	 WATCHDOG;
 */
//------------------------------------------------	
	TR2=1; //start timer 2 to 800Hz
	//if (TR2||(!rtc_readram()))
	if (!rtc_readram())
	{
		 //crc is not good
		 WATCHDOG;
     my_data.dat.bright[0]=0x60;
     my_data.dat.bright[1]=0x40;
     my_data.dat.bright[2]=0x40;
     my_data.dat.bright[3]=0x10;
     strcpy(my_data.dat.pass,"boss");
		 my_data.dat.syntime=0;
		 my_data.dat.mode=3;
		 WATCHDOG;
	//	 memset (my_data.dat.currency,0,NUMOFCUR*2*sizeof(struct currency));

    rtc_writeram();		
		rtc_writeTime(0);
		WATCHDOG;
		rtc_writeram();
	}

time_wait(3); //whait two second for start
//my_data.dat.mode=1;
switch (my_data.dat.mode)
{	
	case 0:
		  gotoxy(0,2);
	    ptr(0);
		  initEps(command0);
		break;
	
  case 1:  //sta mode 
    if (SendToESP(	"AT+RST\r\n",12,15))   //wait for got IP
     {
       initEps(command1);
	    if (SendToESP(	"AT+CIFSR\r\n",18,10)) 
	     {
			   gotoxy(0,2+NUMOFCUR);
			   k=2+NUMOFCUR;
			  for (j=14;j<32;j++)
			   {
				    if (epsBuff[j]=='"')
					   break;
				    if (epsBuff[j+1]=='.')
				     {
						   ptr((epsBuff[j]&0xf)|0x80);
						   k++;
						   j++;
						   gotoxy(0,k);
				     }
				  else
					 ptr((epsBuff[j]&0xf));
			    }
			
	      }
		
		//find time server and get the time
		time_wait(5);
		if (SendToESP(	"AT+CIPSTART=4,\"UDP\",\"time.nist.gov\",123\r\n",10,5))   //wait OK udp connectin
		{
			//OK recieved now send only one enter
			 if (SendToESP(	"AT+CIPSEND=4,1\r\n",255,5))   //wait 5 seconds for connectinp
			  {
					if (SendToESP(	"\r",13,5))   //wait 5 seconds for directly wait for time
					{
						//time recieved
						t=*((long *) &epsBuff[40]);
			  		t-=0x83aa7e80L - 10800L; //1970-1900 + 3 hours					
						rtc_writeTime(t);
						my_data.dat.syntime=t;
					}
			  }
			SendToESP(	"AT+CIPCLOSE=4\r\n",10,5); // wait close the connecton
		}
  }	
 break;
 
 
 case 2: // ap mode
	 initEps(command2);
	 if (SendToESP(	"AT+CIFSR\r\n",16,10)) 
	  {
			gotoxy(0,2);
			k=2;
			for (j=13;j<32;j++)
			 {
				 if (epsBuff[j]=='"')
					 break;
				 if (epsBuff[j+1]=='.')
				  {
						 ptr((epsBuff[j]&0xf)|0x80);
						 k++;
						 j++;
						 gotoxy(0,k);
				  }
				 else
					 ptr((epsBuff[j]&0xf));
			 }
			
	  }  
  break;
		
 //IP is shwon wait 5 sec to clear ip
		
	case 3: //mode initial
		SendToESP(	"AT+RESTORE\r\n",12,15);
		initEps(command3);
	  //now clear flash
	  for (k=1;k<9;k++)
	   helpbuff[k]=0xff;
	  WATCHDOG;
	  k=0;
	  do
	   {
			 helpbuff[0]=k; //select address to clear 8 bytes
			 WriteI2C(EEADDR,helpbuff,9);
			 for (j=5;j;j--) //wait ee to be ready
			  {
			   TF2H=0;
			   while (!TF2H)
			    WATCHDOG; //500HZ refresh rate 
		    }
				k+=8;
		 } while (k);
	  my_data.dat.mode=0; //set mode to 0 wait init
	  WATCHDOG;
		rtc_writeram();
    while(1);	  //watchdog restart	 
		break;
}

 for (i=0;i<800;i++)
	{
		  while (!TF2H)
				 WATCHDOG; //500HZ refresh rate
		  TF2H=0;
	}
	
 for (j=0;j<NUMOFCUR*2;j++) // print rates
	{
		 gotoxy(0,j+TIMER);
		 prnval(j);
		 WATCHDOG;
	}
	
 t=rtc_readTime();
 date_time(&tim,t);
 write_time();
 write_date();		
	
 while (1)
 {	   
		  while (!TF2H)
				 WATCHDOG; //500HZ refresh rate
		  TF2H=0;

			if (rtc_readTime()-t)
			{
				t=rtc_readTime();
				tim.second++;
				if (tim.second==60)
				{
					 date_time(&tim,t);
           write_time();
           write_date();
				}
			  if (t&0x1) //1 sec count
			  {
			   SetPoint(3,1); //the points for the clock
			   }
			 else
			  {
			    SetPoint(3,0);
			   }
		   }
       if (answer!=0)
			 {
				 switch (answer)
			   {	

            case 13:	
							answer=0;
						  if (com_stat==0) //wait handshake
							{
								if (strncmp(epsBuff,"Hi!\r\n",5)==0)
								{
								 //recieved high for computer comunication
									strcpy(helpbuff,VERSION);
									strcpy(combuff," \r\n");
									combuff[0]=(my_data.dat.mode&0xf)+'0';
									strcat(helpbuff,combuff);
									strcpy (combuff,"AT+CIPSEND=");
									strncat(combuff,(char *) &cannel,1);
									strcat (combuff,",");								
									itoa(strlen(helpbuff),combuff);
									strcat(combuff,"\r\n");
                  SendToESP(combuff,0,0);									
									break;
								}
								//here i must check for phone communication
								SendToESP("AT+CIPCLOSE=5\r\n",10,5);	
								break;
							}
						 if (com_stat==1) //handshake happend
						 {
							 if (strncmp(epsBuff,"STA",3)==0) //set to station
							 {
								 strcpy(combuff,"AT+CWJAP=");
								 strcat(combuff,&epsBuff[3]); //ready after reset
								 SendToESP(	"AT+RST\r\n",12,15);  //reset the esp
								 if (SendToESP(	"AT+CWMODE=1\r\n",10,15))
								 {
									 //station send succesfuly
									  if (SendToESP(	combuff,12,15))
										{
											//Got ip all is ok go to mode 1
										 		 my_data.dat.mode=1;
												 rtc_writeram();	
                         //now reset
												 while(1);
										}
										else
										{
											//something wrog got to mode 3
										 		 my_data.dat.mode=3;
												 rtc_writeram();	
                         //now reset
												 while(1);
										}											
								 }
								 break;
							 }
							 if (strncmp(epsBuff,"APN",3)==0) //set to station
							 {
								 strcpy(combuff,"AT+CWSAP=");
								 strcat(combuff,&epsBuff[3]); //ready after reset
								 SendToESP(	"AT+RST\r\n",12,15);  //reset the esp
								 if (SendToESP(	"AT+CWMODE=2\r\n",10,15))
								 {
									 //station send succesfuly
									  if (SendToESP(	combuff,12,15))
										{
											//Got ip all is ok go to mode 1
										 		 my_data.dat.mode=2;
												 rtc_writeram();	
                         //now reset
												 while(1);
										}
										else
										{
											//something wrog got to mode 3
										 		 my_data.dat.mode=3;
												 rtc_writeram();	
                         //now reset
												 while(1);
										}											
								 }
								 break;
							 }
							 // SET(B,S)ccVVVVVVVV
							 if (strncmp(epsBuff,"SET",3)==0)
							 {
								 j=epsBuff[4]&0x0f;
								 j*=10;
								 j+=epsBuff[5]&0x0f;
								 //currency number on j
								 if (j>=NUMOFCUR)
									 break;
								 if (epsBuff[3]=='S')
									 j+=NUMOFCUR;
								 else
									 if (epsBuff[3]!='B')
										 break;
								 helpbuff[0]=j*4; //address of currency
								 for (k=6;k<14;k++)
								  {
										epsBuff[k]-='0';
										if (epsBuff[k]>9)
											epsBuff[k]-=7;
									}
									//===
								  k=epsBuff[6];
									k<<=4;
									k+=epsBuff[7];
									helpbuff[1]=k;
									//===
								  k=epsBuff[8];
									k<<=4;
									k+=epsBuff[9];
									helpbuff[2]=k;
									//===
								  k=epsBuff[10];
									k<<=4;
									k+=epsBuff[11];
									helpbuff[3]=k;
									//===
								  k=epsBuff[12];
									k<<=4;
									k+=epsBuff[13];
									helpbuff[4]=k;	
									WriteI2C(EEADDR,helpbuff,5);
									//wait 5 ms to be writen in the EEPROM
									TF2H=0;
									while (!TF2H)
										WATCHDOG; //500HZ refresh rate
									TF2H=0;
									while (!TF2H)
										WATCHDOG; //500HZ refresh rate
									gotoxy(0,j+TIMER);
									prnval(j);
									strcpy (combuff,"AT+CIPSEND=");
									strncat(combuff,(char *) &cannel,1);
									strcat (combuff,",8\r\n");	
									strcpy (helpbuff,"SET OK\r\n");
									com_stat=0; //dummy
									SendToESP(combuff,0,0);		
								 break;
							 }
							 if (strncmp(epsBuff,"NAME",4)==0)
							 {
								 j=epsBuff[4]&0x0f;
								 j*=10;
								 j+=epsBuff[5]&0x0f;
								 if (j>=NUMOFCUR)
									 break;
								 j*=4;
								 j+=NAMES;
								 memset(helpbuff,0xff,5);
								 for (k=1;k<5;k++)
								  if (isalnum(epsBuff[5+k]))
										helpbuff[k]=epsBuff[5+k];
									else
										break; //break the for loop
								 helpbuff[0]=j;
								 WriteI2C(EEADDR,helpbuff,5);
									//wait 5 ms to be writen in the EEPROM
								 TF2H=0;
								 while (!TF2H)
										WATCHDOG; //500HZ refresh rate
								 TF2H=0;
								 while (!TF2H)
										WATCHDOG; //500HZ refresh rate
								 strcpy (combuff,"AT+CIPSEND=");
							   strncat(combuff,(char *) &cannel,1);
								 strcat (combuff,",9\r\n");	
								 strcpy (helpbuff,"NAME OK\r\n");
								 com_stat=0; //dummy
								 SendToESP(combuff,0,0);									 
								 break;
							 }
							 if (strncmp(epsBuff,"GET",3)==0)
							 {
								 strcpy(helpbuff,"C");
								 itoa(com_stat-1,&helpbuff[1]);
								 strcat(helpbuff,",'");
								 k=(com_stat-1)*4;
								 k+=NAMES;
								 WriteI2C(EEADDR,&k,1);
								 ReadI2C(EEADDR,(char *) combuff,4);
								 for (k=0;k<4;k++)
								  if (!isalnum(combuff[k]))
										break; //iner loop
								 combuff[k]=0;
								 strcat(helpbuff,combuff);
								 strcat(helpbuff,"',");
								 asciival(com_stat-1,helpbuff);
								 strcat(helpbuff,",");
								 asciival(com_stat+NUMOFCUR-1,helpbuff);
								 strcat(helpbuff,"\r\n");
								 strcpy (combuff,"AT+CIPSEND=");
							   strncat(combuff,(char *) &cannel,1);
								 strcat(combuff,",");								
								 itoa(strlen(helpbuff),combuff);
		             strcat(combuff,"\r\n");
								 if (com_stat>=NUMOFCUR)
									 com_stat=1;
                 SendToESP(combuff,0,0);									
								 break;
							 }
							 if (strncmp(epsBuff,"BRN",3)==0)  //brigntess
							 {
								 for (k=3;k<11;k++)
								  {
										epsBuff[k]-='0';
										if (epsBuff[k]>9)
											epsBuff[k]-=7;
									}
								  k=epsBuff[3];
									k<<=4;
									k+=epsBuff[4];
									my_data.dat.bright[0]=k;
									//===
								  k=epsBuff[5];
									k<<=4;
									k+=epsBuff[6];
									my_data.dat.bright[1]=k;
									//===
								  k=epsBuff[7];
									k<<=4;
									k+=epsBuff[8];
									my_data.dat.bright[2]=k;
										//===
								  k=epsBuff[9];
									k<<=4;
									k+=epsBuff[10];
									my_data.dat.bright[3]=k;	
									for (j=0;j<NUMOFCUR*2;j++) // print rates
									{
										gotoxy(0,j+TIMER);
										prnval(j);
										WATCHDOG;
									}	
								  date_time(&tim,t);
									write_time();
									write_date();
								 rtc_writeram();
								 strcpy (combuff,"AT+CIPSEND=");
								 strncat(combuff,(char *) &cannel,1);
								 strcat (combuff,",8\r\n");	
								 strcpy (helpbuff,"BRN OK\r\n");
								 com_stat=0; //dummy
								 SendToESP(combuff,0,0);		
							 }
							 
							 if (strncmp(epsBuff,"RST",3)==0)  //RESTART in mode 3
							 {
								 my_data.dat.mode=3;
								 rtc_writeram();
								 while(1);
							 }
						 }
						 if (com_stat>1)
						 {
							 if (strncmp(epsBuff,"GET",3)==0)
							 {
								 strcpy(helpbuff,"C");
								 itoa(com_stat-1,&helpbuff[1]);
								 strcat(helpbuff,",'");
								 k=(com_stat-1)*4;
								 k+=NAMES;
								 WriteI2C(EEADDR,&k,1);
								 ReadI2C(EEADDR,(char *) combuff,4);
								 for (k=0;k<4;k++)
								  if (!isalnum(combuff[k]))
										break; //iner loop
								 combuff[k]=0;
								 strcat(helpbuff,combuff);
								 strcat(helpbuff,"',");
								 asciival(com_stat-1,helpbuff);
								 strcat(helpbuff,",");
								 asciival(com_stat+NUMOFCUR-1,helpbuff);
								 strcat(helpbuff,"\r\n");
								 strcpy (combuff,"AT+CIPSEND=");
							   strncat(combuff,(char *) &cannel,1);
								 strcat(combuff,",");								
								 itoa(strlen(helpbuff),combuff);
		             strcat(combuff,"\r\n");
								 if (com_stat>=NUMOFCUR)
									 com_stat=0;
                 SendToESP(combuff,0,0);
									
								 break;
							 }
						 }
						 SendToESP("AT+CIPCLOSE=5\r\n",10,5);	
						 com_stat=0;	
						break;
						case 255:
							 answer=0;
						   if (!SendToESP(helpbuff,14,5))
								 SendToESP("AT\r\n",14,5);
							 else
								 com_stat++;    //answer sent go to next status
							break;
						default:
							answer=0;
						 break;
			   }
				 
		   }
	
 }

}