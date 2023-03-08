#include "MBI.h"
$NOMOD51
;------------------------------------------------------------------------------
;  This file is part of the C51 Compiler package
;  Copyright (c) 1988-2005 Keil Elektronik GmbH and Keil Software, Inc.
;  Version 8.01
;
;  *** <<< Use Configuration Wizard in Context Menu >>> ***
;------------------------------------------------------------------------------
;  STARTUP.A51:  This code is executed after processor reset.
;
;  To translate this file use A51 with the following invocation:
;
;     A51 STARTUP.A51
;
;  To link the modified STARTUP.OBJ file to your application use the following
;  Lx51 invocation:
;
;     Lx51 your object file list, STARTUP.OBJ  controls
;
;------------------------------------------------------------------------------
;
;  User-defined <h> Power-On Initialization of Memory
;
;  With the following EQU statements the initialization of memory
;  at processor reset can be defined:
;
; <o> IDATALEN: IDATA memory size <0x0-0x100>
;     <i> Note: The absolute start-address of IDATA memory is always 0
;     <i>       The IDATA space overlaps physically the DATA and BIT areas.
IDATALEN        EQU     80H
;
; <o> XDATASTART: XDATA memory start address <0x0-0xFFFF> 
;     <i> The absolute start address of XDATA memory
XDATASTART      EQU     0x100     
;
; <o> XDATALEN: XDATA memory size <0x0-0xFFFF> 
;     <i> The length of XDATA memory in bytes.
XDATALEN        EQU     0x800     
;
; <o> PDATASTART: PDATA memory start address <0x0-0xFFFF> 
;     <i> The absolute start address of PDATA memory
PDATASTART      EQU     0H
;
; <o> PDATALEN: PDATA memory size <0x0-0xFF> 
;     <i> The length of PDATA memory in bytes.
PDATALEN        EQU     0xff
;
;</h>
;------------------------------------------------------------------------------
;
;<h> Reentrant Stack Initialization
;
;  The following EQU statements define the stack pointer for reentrant
;  functions and initialized it:
;
; <h> Stack Space for reentrant functions in the SMALL model.
;  <q> IBPSTACK: Enable SMALL model reentrant stack
;     <i> Stack space for reentrant functions in the SMALL model.
IBPSTACK        EQU     0       ; set to 1 if small reentrant is used.
;  <o> IBPSTACKTOP: End address of SMALL model stack <0x0-0xFF>
;     <i> Set the top of the stack to the highest location.
IBPSTACKTOP     EQU     0xFF +1     ; default 0FFH+1  
; </h>
;
; <h> Stack Space for reentrant functions in the LARGE model.      
;  <q> XBPSTACK: Enable LARGE model reentrant stack
;     <i> Stack space for reentrant functions in the LARGE model.
XBPSTACK        EQU     0       ; set to 1 if large reentrant is used.
;  <o> XBPSTACKTOP: End address of LARGE model stack <0x0-0xFFFF>
;     <i> Set the top of the stack to the highest location.
XBPSTACKTOP     EQU     0xFFFF +1   ; default 0FFFFH+1 
; </h>
;
; <h> Stack Space for reentrant functions in the COMPACT model.    
;  <q> PBPSTACK: Enable COMPACT model reentrant stack
;     <i> Stack space for reentrant functions in the COMPACT model.
PBPSTACK        EQU     0       ; set to 1 if compact reentrant is used.
;
;   <o> PBPSTACKTOP: End address of COMPACT model stack <0x0-0xFFFF>
;     <i> Set the top of the stack to the highest location.
PBPSTACKTOP     EQU     0xFF +1     ; default 0FFH+1  
; </h>
;</h>
;------------------------------------------------------------------------------
;
;  Memory Page for Using the Compact Model with 64 KByte xdata RAM
;  <e>Compact Model Page Definition
;
;  <i>Define the XDATA page used for PDATA variables. 
;  <i>PPAGE must conform with the PPAGE set in the linker invocation.
;
; Enable pdata memory page initalization
PPAGEENABLE     EQU     1       ; set to 1 if pdata object are used.
;
; <o> PPAGE number <0x0-0xFF> 
; <i> uppermost 256-byte address of the page used for PDATA variables.
PPAGE           EQU     0
;
; <o> SFR address which supplies uppermost address byte <0x0-0xFF> 
; <i> most 8051 variants use P2 as uppermost address byte
PPAGE_SFR       DATA    0A0H
;
; </e>
;------------------------------------------------------------------------------

; Standard SFR Symbols 
ACC     DATA    0E0H
B       DATA    0F0H
SP      DATA    81H
DPL     DATA    82H
DPH     DATA    83H
PCA0MD  DATA    0D9H
PCA0CPH5	DATA	0D3H
;---------------- Mitkos
SPI0DAT			DATA	0A3H
TMR3H	        DATA	095H
TMR3L	        DATA	094H
P2				DATA	0A0H
TMR3CN			DATA    091H
RS0             BIT     PSW.3
RS1             BIT     PSW.4
SPIF	        BIT	    0F8H.7
mbiLE			BIT     P2.4
sel0            BIT     P2.5
sel1            BIT     P2.6
;----------------------------------
                NAME    ?C_STARTUP


?C_C51STARTUP   SEGMENT   CODE
?STACK          SEGMENT   IDATA

                RSEG    ?STACK
                DS      1

                EXTRN CODE (?C_START)
                PUBLIC  ?C_STARTUP

                CSEG    AT      0
?C_STARTUP:     LJMP    STARTUP1

                RSEG    ?C_C51STARTUP

STARTUP1:
			    ANL     PCA0MD,#00 ;CLEAR WDE	
IF IDATALEN <> 0
                MOV     R0,#IDATALEN - 1
                CLR     A
IDATALOOP:      MOV     @R0,A
                DJNZ    R0,IDATALOOP
ENDIF

IF XDATALEN <> 0
                MOV     DPTR,#XDATASTART
                MOV     R7,#LOW (XDATALEN)
  IF (LOW (XDATALEN)) <> 0
                MOV     R6,#(HIGH (XDATALEN)) +1
  ELSE
                MOV     R6,#HIGH (XDATALEN)
  ENDIF
                CLR     A
XDATALOOP:      MOVX    @DPTR,A
                INC     DPTR
                DJNZ    R7,XDATALOOP
                DJNZ    R6,XDATALOOP
ENDIF

IF PPAGEENABLE <> 0
                MOV     PPAGE_SFR,#PPAGE
ENDIF

IF PDATALEN <> 0
                MOV     R0,#LOW (PDATASTART)
                MOV     R7,#LOW (PDATALEN)
                CLR     A
PDATALOOP:      MOVX    @R0,A
                INC     R0
                DJNZ    R7,PDATALOOP
ENDIF

IF IBPSTACK <> 0
EXTRN DATA (?C_IBP)

                MOV     ?C_IBP,#LOW IBPSTACKTOP
ENDIF

IF XBPSTACK <> 0
EXTRN DATA (?C_XBP)

                MOV     ?C_XBP,#HIGH XBPSTACKTOP
                MOV     ?C_XBP+1,#LOW XBPSTACKTOP
ENDIF

IF PBPSTACK <> 0
EXTRN DATA (?C_PBP)
                MOV     ?C_PBP,#LOW PBPSTACKTOP
ENDIF

                MOV     SP,#?STACK-1

; This code is required if you use L51_BANK.A51 with Banking Mode 4
;<h> Code Banking
; <q> Select Bank 0 for L51_BANK.A51 Mode 4
#if 0   
;     <i> Initialize bank mechanism to code bank 0 when using L51_BANK.A51 with Banking Mode 4.
EXTRN CODE (?B_SWITCH0)
                CALL    ?B_SWITCH0      ; init bank mechanism to code bank 0
#endif
;</h>
; init for spi interrupr
				MOV      15H,#LOW(screen)
				MOV      16h,#HIGH(screen)
				MOV      17H,#NUM_OF_MBI    ;LSB COUNT OF MBI
				MOV      13H,#NUM_OF_COLUMNS
				MOV		 14H,#10H              ;Number of register
				CLR      sel0
				CLR      sel1
                LJMP    ?C_START
; MY Code Mitkoo



 EXTRN          XDATA  (screen)
				CSEG    AT 033H   //SPI INTERRUPT
				LJMP     SPI_INT
				
			    CSEG    AT 073H   //Timer3 interrup
			; I'll do no jump
				SETB     mbiLE
				ANL      TMR3CN,#07BH  //TR3 = 0 AND TF3H = 0
				RETI
				
FLAGS           SEGMENT  BIT
                RSEG     FLAGS
				
NIBLLE:			DBIT     1 ;first/second byte of MBI 16 bit register
NEW_REFRESH:    DBIT     1 ;next screen refresh
				
				RSEG     ?C_C51STARTUP
				USING    2
SWITCH_COLUMN:
;                
                DJNZ	 R3,SEND_GLOBAL
				MOV      R5,#LOW(screen)
				MOV      R6,#HIGH(screen)
				MOV      R3,#NUM_OF_COLUMNS
	//R2 IS ZERO LAST COLUMN MUST DISPLAYED
			    SETB     sel0
				SETB     sel1
				SJMP     SEND_GLOBAL1
SEND_GLOBAL:   
                CJNE     R3,#1,SEND_GLOBAL2
				CLR      sel1
				SETB     sel0
				SJMP     SEND_GLOBAL1
				
SEND_GLOBAL2:				
   // R2 IS NOT 0 AND NOT 1 -> IS 2
				SETB     sel1
				CLR      sel0
				
SEND_GLOBAL1:
				MOV      TMR3H,#HIGH(GLOBALLACH)
				MOV      TMR3L,#LOW(GLOBALLACH)
				ORL      TMR3CN,#04H
				MOV      SPI0DAT,#0FFH	         ; only 8 bits used msb=0
				POP      PSW
				RETI	
;------------------------------------------------------
; INTERRUPT

SPI_INT:				
				CLR      mbiLE
				PUSH     PSW				
				CLR      RS0
				SETB     RS1
                CLR      SPIF			
NO_FIRST:
				JBC      NIBLLE,SECOND       ;SECOND BYTE FROM MBI WORD
				JB	     NEW_REFRESH,DUMMY
				PUSH     ACC
				PUSH     DPL
				PUSH     DPH
				MOV      DPL,R5
				MOV      DPH,R6	
				MOVX     A,@DPTR
				MOV      SPI0DAT,A	         ; only 8 bits used msb=0
				SETB     NIBLLE
                INC      DPTR				
				MOV      R5,DPL
				MOV      R6,DPH				
				POP      DPH
				POP      DPL				
				POP      ACC
				POP      PSW
				RETI
DUMMY:
				MOV      SPI0DAT,#0FFH
				SETB     NIBLLE
				POP      PSW
				RETI
SECOND:
				JBC		 NEW_REFRESH,SWITCH_COLUMN				
				DJNZ     R7,REG_COUNT
				MOV      R7,#NUM_OF_MBI
				
				MOV      TMR3H,#HIGH(DATALACH)
				MOV      TMR3L,#LOW(DATALACH)
				
				DJNZ     R4,LE_PULSE
				
				MOV      R4,#10H
				SETB     NEW_REFRESH
				
				CLR      sel0
				CLR      sel1

				
LE_PULSE:
				ORL      TMR3CN,#04H
REG_COUNT:

				MOV      SPI0DAT,#0

				
				POP      PSW
				RETI

				
                END
