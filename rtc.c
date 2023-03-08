#include "display_slot.h"


 
union my_data idata my_data;

void rtc_write(unsigned char reg,unsigned char dat)
{
  while ((RTC0ADR&0x80)==0x80); //wait bussy flag
  RTC0ADR=reg;
  RTC0DAT=dat;
}

unsigned char rtc_read(unsigned char reg)
{
   while ((RTC0ADR&0x80)==0x80); //wait bussy flag
	 RTC0ADR=reg;
   RTC0ADR=reg|0x80;     //write busy 1
   while ((RTC0ADR&0x80)==0x80); //wait bussy flag
   return RTC0DAT;
} 

bit rtc_readram()
{
    unsigned char addr;
// Read from the entire smaRTClock RAM
    CRC0CN=0x18;    //init crc generator
	RTC0ADR = 0x0E;// address the RAMADDR register
	RTC0DAT = 0x00;// write the address of 0x00 to RAMADDR
	while ((RTC0ADR & 0x80) == 0x80); // poll on the BUSY bit
	RTC0ADR = 0x0F; // address the RAMDATA register
	for (addr = 0; addr < sizeof(union my_data)-2; addr++)
	{
	 RTC0ADR |= 0x80; // initiate a read of the RAMDATA register
	 while ((RTC0ADR & 0x80) == 0x80); // poll on the BUSY bit
	 my_data.rtc_ram[addr] = RTC0DAT; // copy the data from the entire RAM
	 CRC0IN=my_data.rtc_ram[addr];
	}

	RTC0ADR |= 0x80; // initiate a read of the RAMDATA register
	while ((RTC0ADR & 0x80) == 0x80); // poll on the BUSY bit
	CRC0CN=0x10;   //select crc byte 0
	if (RTC0DAT!=CRC0DAT) 
	 return 0;

	RTC0ADR |= 0x80; // initiate a read of the RAMDATA register
	while ((RTC0ADR & 0x80) == 0x80); // poll on the BUSY bit
	CRC0CN=0x11;   //select crc byte 0
	if (RTC0DAT!= CRC0DAT) 
	 return 0;
  return 1;

}

void rtc_writeram()
{
   unsigned char addr;
   CRC0CN=0x18;    //init crc generator
// Write to the entire smaRTClock RAM
	RTC0ADR = 0x0E;// address the RAMADDR register
	RTC0DAT = 0x00;// write the address of 0x00 to RAMADDR
	while ((RTC0ADR & 0x80) == 0x80); // poll on the BUSY bit
	RTC0ADR = 0x0F;// address the RAMDATA register
	for (addr = 0; addr < sizeof(union my_data)-2; addr++)
	{
	 CRC0IN=my_data.rtc_ram[addr];
	 RTC0DAT = my_data.rtc_ram[addr]; // write 0xA5 to every RAM address
	 while ((RTC0ADR & 0x80) == 0x80);// poll on the BUSY bit
	}
	CRC0CN=0x10;   //select crc byte 0
	RTC0DAT = CRC0DAT; // write 0xA5 to every RAM address
	while ((RTC0ADR & 0x80) == 0x80);// poll on the BUSY bit

 	CRC0CN=0x11;   //select crc byte 1
	RTC0DAT = CRC0DAT; // write 0xA5 to every RAM address
	while ((RTC0ADR & 0x80) == 0x80);// poll on the BUSY bit   

}

//setup rtc in self oscilating mode !!!!!
bit rtc_init()
{
  bit res;
  unsigned char c;

  res=1;
  if (RTC0KEY==3)
   // problem RTC locked need reset
    RSTSRC|=0x20;    //software reset
    //unlock RTC
  if (RTC0KEY==0)
   {
     //unlock the RTC is no locked
     RTC0KEY = 0xA5;
     RTC0KEY = 0xF1;
   };

  c=rtc_read(6); //READ RTC0CN
  if (((c&0x20)==0x20) || ((c&0x80)==0))  //check OSCFAIL
   {
    res=0;   //save status for return value
	  rtc_write(6,0); //disable RTC  
	  rtc_write(7,0x40); //enale crystal mode
		rtc_write(7,0xc0); //enale atogain
	  rtc_write(6,0x80); //enable auto gain
	  while ((rtc_read(7)&0x10)!=0x10); //clock be stable not nessery for self oscilating mode
	 // rtc_write(7,0x40); //disable autogain
   //	rtc_write(6,0xd0); //start counting enable missingo clock

   }
   return res;
}

unsigned long rtc_readTime()
{
  unsigned long time;
  unsigned char c,d;

   rtc_write(6,0xD1);            //transwer timer value to capture register
   while ((rtc_read(6)&0x1)==1); // wait time transwer to capture register

   for (c=2,d=4;d!=0;c++)  
      ((unsigned char *) &time)[--d]=rtc_read(c);

   return time;

}

void rtc_writeTime( unsigned long time)
{

  unsigned char c,d;

   rtc_write(0,0);
	 rtc_write(1,0);
   for (c=2,d=4;d!=0;c++)  
     rtc_write(c,((unsigned char *) &time)[--d]);

   rtc_write(6,0xD2);    //transwer timer value to capture register
   while ((rtc_read(6)&0x2)==2); // wait time transwer to capture register

}


