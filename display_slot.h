#include "compiler_defs.h"
#include "C8051F410_defs.h"
#include  "MBI.h"

#define WATCHDOG PCA0CPH5=0
#define I2CKEY
#define USE_RTC
//#define USE_SPI
#define USE_MBI
#define ROULETTE
#define USE_NRF24
#define uint8_t unsigned char
void Init_Device();


unsigned long rtc_readTime();
void rtc_writeTime( unsigned long time);
bit rtc_init();
void rtc_writeram();
bit rtc_readram();



struct currency {
	unsigned char vals[3];
	unsigned char point;
};

union  my_data
 {
	  
	  struct dat {
			 unsigned char bright[4];           //4
       unsigned char pass[10];            //14
			 unsigned char mode;                //15
			 unsigned long syntime;             //19
		//	 unsigned char dummy;             //20
			 unsigned int  crc;               
		} dat;
		unsigned char rtc_ram[sizeof(struct dat)];
 };



void mbiInit();


sbit led     = P1^2; //test led
sbit mbiLE   = P2^4; //SPI CE for MBI

// RES   = P2.6 ->SEL
// CNT   = P2.5 ->SEL 
sbit sel0   =  P2^5;
sbit sel1   =  P2^6;

#define  RST sel1
#define  CNT sel0

 

sbit Dclk   =   P0^2;
sbit sdi    =   P0^6;
sbit sdo    =   P0^3;
#define MOSI sdi
#define MISO sdo
#define SCK  Dclk
sbit CE24L01  = P1^3;
sbit CSN24L01 = P1^6;

// I2c

#define EEADDR 0xA0
#define NAMES 0xa0
void init_i2c();
unsigned char ReadI2C(unsigned char address,char *dat,char num);
unsigned char WriteI2C(unsigned char address,char *dat,char num);