#include "display_slot.h"
#include <intrins.h>




#define NO_LACH            -1

#define DATA_LACH          1
#define GLOBAL_LACH        3
#define READ_CONFIGURATION 5
#define EABLE_ERRORDETECT  7
#define READ_ERRORSTATUS   9
#define WRITE_CONFIG       11
#define RESET_PWM          13



unsigned char xdata screen[NUM_OF_MBI*NUM_OF_COLUMNS*8] _at_ 0x100-0x20;


 
//b1,a1,f2,f1,g1,c2,b2,a2,D1,d2,e2,e1,d1,c1,D2,g2
//0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
 
const unsigned char code cs1[8]={ 1, 0,13,12,11, 3, 4, 8};    
const unsigned char code cs2[8]={ 7,  6, 5, 9,10, 2,15,14};


struct caddr {
                unsigned int address;
								unsigned char column;
								unsigned char row;
								unsigned char segment;
							} complicate;


void address()
 {
   unsigned int chip,col,seg;
						   
   chip = complicate.row;
   col=	complicate.column/2; 
   if ((complicate.column&1)==0)	
    seg=cs1[complicate.segment];
   else
    seg=cs2[complicate.segment];		 
   complicate.address=NUM_OF_MBI*(16*col+seg)+chip;
}
 

                                     //  0    1    2    3    4    5    6    7    8    9    - 
const unsigned char code numbers[16]= {0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6,0x02,0x00,0x00,0x00,0x00};


extern union my_data idata my_data;

void ptr(char c) //c ->PBBXNNNN POINT,BRIGHTNESS,NUMBER
 {
	  unsigned char d,k;
    d=numbers[c&0x0f];
	  if (c&0x80) 
			d|=0x1; //add a point
		k=my_data.dat.bright[(c>>4)&0x3];
	  for (complicate.segment=0;complicate.segment<8;complicate.segment++)
	   {			 
 			 address();
			 if ((d&0x80)!=0)
				 screen[complicate.address]=k;
			 else
				 screen[complicate.address]=0;
			 d<<=1;
	   }
		 complicate.column++;
	   if (complicate.column>=6)
		 {
			 complicate.column-=6;
			 if (++complicate.row>=NUM_OF_MBI)
				 complicate.row=0;
				  
		 }
		 return;
 }                       

void gotoxy(unsigned char x,unsigned char y)
 {
	 complicate.column=x;
	 complicate.row=y;
 }
 
void SetPoint(unsigned char b,bit sr) //Set the two led's for the clock
 {
	 if (sr)
	  {
		 screen[16*NUM_OF_MBI+15*NUM_OF_MBI+1]=my_data.dat.bright[b]; //set G2
		 screen[16*NUM_OF_MBI+ 7*NUM_OF_MBI+1]=my_data.dat.bright[b]; //set A2		 
	  }
	 else
	  {
		 screen[16*NUM_OF_MBI+15*NUM_OF_MBI+1]=0; //set G2
		 screen[16*NUM_OF_MBI+ 7*NUM_OF_MBI+1]=0; //set A2		 
	  }		 
 }

 