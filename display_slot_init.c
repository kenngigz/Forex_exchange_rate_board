/////////////////////////////////////
//  Generated Initialization File  //
/////////////////////////////////////

#include "compiler_defs.h"
#include "C8051F410_defs.h"

// Peripheral specific initialization functions,
// Called from the Init_Device() function
void Reset_Sources_Init()
{
    int i = 0;
    VDM0CN    = 0x80;
    for (i = 0; i < 20; i++);  // Wait 5us for initialization
    RSTSRC    = 0x06;
}

void PCA_Init()
{
    PCA0CN    = 0x40;
    PCA0MD    &= ~0x40;
    PCA0MD    = 0x00;
    PCA0CPM0  = 0x46;
    PCA0CPL5  = 0xFF;
    PCA0MD    |= 0x20;
    PCA0CPH0  = 0x02;
}

void Timer_Init()
{
    TCON      = 0x50;
    TMOD      = 0x22;
    CKCON     = 0x4C;
    TH0       = 0x5D;
    TH1       = 0x2B;
    TMR2RLL   = 0x7F;
    TMR2RLH   = 0x60;
    TMR2L     = 0x7F;
    TMR2H     = 0x60;
}

void UART_Init()
{
    SCON0     = 0x10;
}

void SMBus_Init()
{
    SMB0CF    = 0xC0;
}

void SPI_Init()
{
    SPI0CFG   = 0x40;
    SPI0CN    = 0x01;
}

void ADC_Init()
{
    ADC0MX    = 0x0F;
    ADC0CF    = 0xFA;
    ADC0CN    = 0x80;
}

void DAC_Init()
{
    IDA0CN    = 0xF3;
}

void Comparator_Init()
{
    int i = 0;
    CPT0CN    = 0x80;
    for (i = 0; i < 35; i++);  // Wait 10us for initialization
    CPT0CN    &= ~0x30;
    CPT0MX    = 0x70;
}

void Voltage_Reference_Init()
{
    REF0CN    = 0x1A;
}

void Port_IO_Init()
{
    // P0.0  -  Skipped,     Open-Drain, Analog
    // P0.1  -  Skipped,     Open-Drain, Digital
    // P0.2  -  SCK  (SPI0), Push-Pull,  Digital
    // P0.3  -  MISO (SPI0), Open-Drain, Digital
    // P0.4  -  TX0 (UART0), Push-Pull,  Digital
    // P0.5  -  RX0 (UART0), Open-Drain, Digital
    // P0.6  -  MOSI (SPI0), Push-Pull,  Digital
    // P0.7  -  SDA (SMBus), Open-Drain, Digital

    // P1.0  -  SCL (SMBus), Open-Drain, Digital
    // P1.1  -  CEX0 (PCA),  Push-Pull,  Digital
    // P1.2  -  Unassigned,  Push-Pull,  Digital
    // P1.3  -  Unassigned,  Push-Pull,  Digital
    // P1.4  -  Unassigned,  Open-Drain, Digital
    // P1.5  -  Unassigned,  Open-Drain, Digital
    // P1.6  -  Unassigned,  Push-Pull,  Digital
    // P1.7  -  Unassigned,  Open-Drain, Digital

    // P2.0  -  Unassigned,  Open-Drain, Digital
    // P2.1  -  Unassigned,  Open-Drain, Digital
    // P2.2  -  Unassigned,  Open-Drain, Digital
    // P2.3  -  Unassigned,  Open-Drain, Digital
    // P2.4  -  Unassigned,  Push-Pull,  Digital
    // P2.5  -  Unassigned,  Push-Pull,  Digital
    // P2.6  -  Unassigned,  Push-Pull,  Digital
    // P2.7  -  Unassigned,  Open-Drain, Digital

    P0MDIN    = 0xFE;
    P0MDOUT   = 0x54;
    P1MDOUT   = 0x4E;
    P2MDOUT   = 0x70;
    P0SKIP    = 0x03;
    XBR0      = 0x07;
    XBR1      = 0xC1;
}

void Oscillator_Init()
{
    int i = 0;
    PFE0CN    &= ~0x20;
    FLSCL     = 0x10;
    PFE0CN    |= 0x20;
    CLKMUL    = 0x80;
    for (i = 0; i < 20; i++);    // Wait 5us for initialization
    CLKMUL    |= 0xC0;
    while ((CLKMUL & 0x20) == 0);
    CLKSEL    = 0x02;
    OSCICN    = 0x87;
}

void Interrupts_Init()
{
    EIE1      = 0x80;
    EIP1      = 0x80;
    IE        = 0x90;
}

// Initialization function for device,
// Call Init_Device() from your main program
void Init_Device(void)
{
    Reset_Sources_Init();
    PCA_Init();
    Timer_Init();
    UART_Init();
    SMBus_Init();
    SPI_Init();
    ADC_Init();
    DAC_Init();
    Comparator_Init();
    Voltage_Reference_Init();
    Port_IO_Init();
    Oscillator_Init();
    Interrupts_Init();
}
