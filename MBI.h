#define BIG

//#ifndef BIG
//the small display
#define NUM_OF_MBI 18
#define NUM_OF_COLUMNS 3
#define NUMOFCUR 8
#define SIOSPD 0x0a
#define VERSION "VER:1,1,8,"
#define TIMER 2
/*#else
#define NUM_OF_MBI 12
#define NUM_OF_COLUMNS 3
#define NUMOFCUR 5
#define SIOSPD 0x0a
#define VERSION "VER:1,2,5,"
#define TIMER 2
#endif
*/
//VERSION,SERIAL NUMBER,NUMBER FOR CURENCYES


/*
//the big display
#define NUM_OF_MBI 38
#define NUM_OF_COLUMNS 3
#define NUMOFCUR 18
#define SIOSPD 0x0a
*/

/*
#define SIOSPD 0x16
#define DATALACH 0xFED5
#define GLOBALLACH 0xFF31
*/

#define DATALACH 0x10000-(0xD*(SIOSPD+1))
#define GLOBALLACH 0x10000-(0x9*(SIOSPD+1))
