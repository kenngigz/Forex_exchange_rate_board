#include "display_slot.h"
#include <intrins.h>
#include <ctype.h>


						
unsigned char ReadI2C(unsigned char address,char *dat,char num)
{
            STO = 0;                 // stop condition bit must be zero
            STA = 1;                 // manually clear STA
						while (!SI);             // wait for start to complete					
            SMB0DAT = address  | 1;  // A6..A0 + read
						STA = 0;                 // manually clear STA
            SI = 0;                  // continue
            while (!SI);             // wait for completion
            if (!ACK)                // if not acknowledged, stop
             {
							STO = 1;               // stop condition bit
							SI = 0;                // generate stop condition
							return 0;              //return error
             }
						 
						while (num--)
						{
              SI = 0;								 // continue
              while (!SI);					 // wait for completion
					    *dat=SMB0DAT;
							ACK = (num!=0); 			// NACK, last byte
							dat++;
					//		ACK = (num!=0); 		// NACK, last byte
						}
           STO = 1; 								// stop condition bit
           SI = 0; 									// generate stop condition
           return 1; 							  //success
}

unsigned char WriteI2C(unsigned char address,char *dat,char num)
{
     STO=0; 
     STA=1;             //generate start
     while(!SI);
     SMB0DAT=address;   //Write to keyboard
 
     STA=0;
     SI=0;
     while(!SI);

	  if (!ACK)         // if not acknowledged, stop
      {
        STO = 1;      // stop condition bit
        SI = 0;       // generate stop condition
        return 0;     //No ACK
      }
	  while (num--)
		{
       SMB0DAT=*dat;   
       SI=0;
			 while(!SI);
	     if (!ACK)         // if not acknowledged, stop
        {
          STO = 1;      // stop condition bit
          SI = 0;       // generate stop condition
          return 0;     //No ACK
        }
			 dat++;
		}
   
    STO=1;
    SI=0;
    while(STO);	 
	  return 1;
}

void init_i2c()
 {
	  	TR0=1; //start timer0 for the I2C
			SMB0CF &= ~0x80; // Reset communication
			SMB0CF |= 0x80;
			STA = 0;
			STO = 0;
			ACK = 0;

 }
      

