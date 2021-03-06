/*
 * Author: James Hagerman
 * Date: December 7, 2013
 *
 * This project is a basic boiler plate/template for using the the UBW32
 * development board as a development core for a digital synthesizer. It is
 * basically a fork from the UBW_32_starter_project.X project that I put
 * together a while back. That project was patched together from the UBW32
 * schematic, the PIC32MX datasheet, and a few of the tutorials located here:
 *
 * http://umassamherstm5.org/tech-tutorials/pic32-tutorials/ubw32-tutorials
 *
 * Those tutorials were a big help in building out some of this basic
 * template.
 *
 * This code includes:
 * - the PIC42 peripheral libraries
 * - the basic configuration setup lines
 * - the lines needed to configure the UBW32 LED's and buttons
 *
 * This code will include (in the future):
 * - SPI communication setup configuration
 * - SPI send and receive commands
 * - Wolfson WM8731 CODEC Chip control interface (over SPI)
 * - Wolfson WM8731 PCM->I2S interface.
 * - A basic example of using the WM8731 for ADC/DAC.
 * - Serial communication (at least send)
 *
 *
 * SPI Info groked from the UMASS Tutorials:
 *
 * The PIC32MX4xxH series on the CUI32 has one SPI channel.
 * The PIC32MX7xxH series on the UBW32 has three SPI channels.
 *
 * SPI1 Setup: (NOT USED IN CODE BELOW!!!)
 * ? Chip Select (CS) : PIC32 pin RD9  (SS1) This is under OUR MANUAL control!
 * ?  Data Out (MOSI) : PIC32 pin RD0  (SDO1)
 * ?   Data In (MISO) : PIC32 pin RC4  (SDI1)
 * ?   SPICLOCK (SCK) : PIC32 pin RD10 (SCK1)
 *
 * SPI2 Setup:
 *  Chip Select (CS) : PIC32 pin RG9  (SS2) This is under OUR MANUAL control!
 *   Data Out (MOSI) : PIC32 pin RG8  (SDO2)
 *    Data In (MISO) : PIC32 pin RG7  (SDI2)
 *    SPICLOCK (SCK) : PIC32 pin RG6  (SCK2)
 *
 *
 * Wolfson CODEC Setup:
 * SPI1 for Control:
 *   Wolfson pin 22 (CSB) : PIC32 pin RD9  (SS1)
 *  Wolfson pin 23 (SDIN) : PIC32 pin RD0  (SDO1)
 *                     NC : PIC32 pin RC4  (SDI1)
 *  Wolfson pin 24 (SCLK) : PIC32 pin RD10 (SCK1)
 *
 * SPI2 For I2S:
 *  Wolfson pin 5 (DACLRC)
 *          AND 7 (ADCLRC) : PIC32 pin RG9 (SS2)
 *  Wolfson pin 4 (DACDAT) : PIC32 pin RG8 (SDO2)
 *  Wolfson pin 6 (ADCDAT) : PIC32 pin RG7 (SDI2)
 *    Wolfson pin 3 (BCLK) : PIC32 pin RG6 (SCK2)
 *  
 *  

 *
 *
 *
 * I2C Module configuration:
 *
 * I2C1 Setup:
 * Clock (SCL) : PIC32 pin RA14 (SCL1)
 *  Data (SDA) : PIC32 pin RA15 (SDA1)
 *
 * I2C2 Setup:
 * Clock (SCL) : PIC32 pin RA2 (SCL2)
 *  Data (SDA) : PIC32 pin RA3 (SDA2)
 *
 *
 */


/******************************************************************************/
/*  Files to Include                                                          */
/******************************************************************************/

#include <plib.h>           /* Include to use PIC32 peripheral libraries      */
#include <stdint.h>         /* For uint32_t definition                        */
#include <stdbool.h>        /* For true/false definition                      */

#include "system.h"         /* System funct/params, like osc/periph config    */
#include "user.h"           /* User funct/params, such as InitApp             */

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

bool statusLed = false;


// ===========================================================================
// ShortDelay - Delays (blocking) for a very short period (in CoreTimer Ticks)
// ---------------------------------------------------------------------------
// The DelayCount is specified in Core-Timer Ticks.
// This function uses the CoreTimer to determine the length of the delay.
// The CoreTimer runs at half the system clock.
// If CPU_CLOCK_HZ is defined as 80000000UL, 80MHz/2 = 40MHz or 1LSB = 25nS).
// Use US_TO_CT_TICKS to convert from uS to CoreTimer Ticks.
// ---------------------------------------------------------------------------

void ShortDelay(                       // Short Delay
  UINT32 DelayCount)                   // Delay Time (CoreTimer Ticks)
{
  UINT32 StartTime;                    // Start Time
  StartTime = ReadCoreTimer();         // Get CoreTimer value for StartTime
  while ( (UINT32)(ReadCoreTimer() - StartTime) < DelayCount ) {};
}

// delay in microseconds function
void delay_us( uint32_t delay ){
        // note that 1 core tick = 2 SYS cycles (this is fixed)
        int us_ticks=(SYS_FREQ/1000000)/2;
        WriteCoreTimer( 0 );
        while( ReadCoreTimer() < delay*us_ticks );
} // END delay_us()

void delay_ms(unsigned char delay)	// Max input is 255, add second delay if you need more time...
{
   unsigned char one_ms;
   one_ms = 0xFF;
   while (delay--)
   {
       do
       {
           one_ms--;
       } while (one_ms);
   }
}

/******************************************************************************/
/* SPI Methods:                                                               */
/******************************************************************************/
/* SPI1 */
// send one byte of data and receive one back at the same time
char writeSPI1_8( char i ){
    SPI1BUF = i; // write to buffer for TX
    while( !SPI1STATbits.SPIRBF ); // wait for TX complete
    return SPI1BUF; // read the received values
} // END writeSPI1()

// send one byte of data and receive one back at the same time
uint16_t writeSPI1_16( uint16_t i ){
    SPI1BUF = i; // write to buffer for TX
    while( !SPI1STATbits.SPIRBF ); // wait for TX complete
    return SPI1BUF; // read the received values
} // END writeSPI1_16()

// send one byte of data and receive one back at the same time
uint32_t writeSPI1_32( uint32_t i ){
    SPI1BUF = i; // write to buffer for TX
    while( !SPI1STATbits.SPIRBF ); // wait for TX complete
    return SPI1BUF; // read the received values
} // END writeSPI1_32()

/* SPI2 */
// send one byte of data and receive one back at the same time
char writeSPI2_8( char i ){
    SPI2BUF = i; // write to buffer for TX
    while( !SPI2STATbits.SPIRBF ); // wait for TX complete
    return SPI2BUF; // read the received values
} // END writeSPI2()

// send one byte of data and receive one back at the same time
uint16_t writeSPI2_16( uint16_t i ){
    SPI2BUF = i; // write to buffer for TX
    while( !SPI2STATbits.SPIRBF ); // wait for TX complete
    return SPI2BUF; // read the received values
} // END writeSPI2_16()

// send one byte of data and receive one back at the same time
uint32_t writeSPI2_32( uint32_t i ){
    SPI2BUF = i; // write to buffer for TX
    while( !SPI2STATbits.SPIRBF ); // wait for TX complete
    return SPI2BUF; // read the received values
} // END writeSPI2_32()



/******************************************************************************/
/* AD5206 digital pot                                                          */
/******************************************************************************/
//// write data to AD5206 digital pot
//void writePot( char addr, char potVal ){
//    CS = 0; // select chip
//    writeSPI2_8( addr );
//    writeSPI2_8( potVal );
//    CS = 1; // release chip
//} // END writePot()


/******************************************************************************/
/* MCP4921 12bit DAC                                                          */
/******************************************************************************/
//// Pin definitions should probably be done in user.c but for now... whatever.
//#define DAC_CS _RG9 // DAC chip select
//#define DAC_TCS _TRISG9 // DAC tris control for CS pin
//
//// Write data to the MCP4921 12bit DAC:
//// Maximum input value is 4095 (0xFFF). Anything higher gets bit masked TO HELL.
//void writeDAC(uint16_t data) {
//    DAC_CS = 0;
//    // config bits: 0x3 = 0011 -> output a, unbuffered, gain of 1x, enable the output buffer
//    int16_t config = 0x3 << 12;
//    data = data & 0x0FFF; // strip off just the 12 bits of data we actually have
//    writeSPI2_16(config | data);
//    DAC_CS = 1;
//}


/******************************************************************************/
/* MCP3204 ADC                                                        */
/******************************************************************************/
//// Pin definitions should probably be done in user.c but for now... whatever.
//#define ADC_CS _RE8 // ADC chip select
//#define ADC_TCS _TRISE8 // ADC tris control for CS pin
//
//// Read value from channel 0 of the MCP3204 DAC:
//// ToDo: manage channel selection as a parameter
//uint32_t readADC() {
//    uint32_t toRet;
//    ADC_CS = 0;
//
//    // config bits: 0x18 = 11000 -> start bit, single ended, channel 0 (3 bits)
////    int32_t config = 0x18 << 14; // shift 14 bits -> sample time, null, data (12 bits)
////    toRet = writeSPI2_32(config);
//
//    // 16 bit version:
//    // config bits: 0x18 = 11000 -> start bit, single ended, channel 0 (3 bits)
//    uint16_t config = 0x18 >> 2; // shift 14 bits -> sample time, null, data (12 bits)
//    toRet = writeSPI2_16(config);
//    config = 0;
////    config = 0x18 << 14; // shift 14 bits -> sample time, null, data (12 bits)
//    toRet = writeSPI2_16(config);
//
//    ADC_CS = 1;
//    return toRet;
//}


/******************************************************************************/
/* Wolfson WM8731 CODEC Methods
 * SPI1 for Control:
 *   Wolfson pin 22 (CSB) : PIC32 pin RD9  (SS1)
 *  Wolfson pin 23 (SDIN) : PIC32 pin RD0  (SDO1)
 *                     NC : PIC32 pin RC4  (SDI1)
 *  Wolfson pin 24 (SCLK) : PIC32 pin RD10 (SCK1)
 *
 * SPI2 For I2S:
 *  Wolfson pin 5 (DACLRC)
 *          AND 7 (ADCLRC) : PIC32 pin RG9 (SS2)
 *  Wolfson pin 4 (DACDAT) : PIC32 pin RG8 (SDO2)
 *  Wolfson pin 6 (ADCDAT) : PIC32 pin RG7 (SDI2)
 *    Wolfson pin 3 (BCLK) : PIC32 pin RG6 (SCK2)
/******************************************************************************/

#define CSB _RD9 // CODEC LR select
#define CSB_TRIS _TRISD9 // tris control for CODEC LR select pin
//void setCodec(uint16_t addr, uint16_t data) {
//    uint16_t addr_data = addr << 9;
//    addr_data = addr_data & data;
//    // config bits: 0x3 = 0011 -> output a, unbuffered, gain of 1x, enable the output buffer
////    int32_t config = 0x3 << 12;
////    data_l = data_l & 0x0FFF; // strip off just the 12 bits of data we actually have
//    writeSPI1_16(addr_data);
//    CSB = 0;
//    delay_us(1);
//    CSB = 1;
//}
void setCodec(uint16_t config) {
    // config bits: 0x3 = 0011 -> output a, unbuffered, gain of 1x, enable the output buffer
//    int32_t config = 0x3 << 12;
//    data_l = data_l & 0x0FFF; // strip off just the 12 bits of data we actually have

    CSB = 0;
    writeSPI1_16(config);
    CSB = 1;
//    delay_us(1);
//    ShortDelay(5);
    
}

// Pin definitions should probably be done in user.c but for now... whatever.
#define DACLRC _RG9 // CODEC LR select
#define DACLRC_TRIS _TRISG9 // tris control for CODEC LR select pin
void writeCodec(uint32_t data_l, uint32_t data_r) {
    DACLRC = 0;
    // config bits: 0x3 = 0011 -> output a, unbuffered, gain of 1x, enable the output buffer
//    int32_t config = 0x3 << 12;
//    data_l = data_l & 0x0FFF; // strip off just the 12 bits of data we actually have
    writeSPI2_32(data_l);
    writeSPI2_32(data_r);
    DACLRC = 1;
}

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int32_t main(void)
{

    /*Refer to the C32 peripheral library compiled help file for more
    information on the SYTEMConfig function.
    
    This function sets the PB divider, the Flash Wait States, and the DRM
    /wait states to the optimum value.  It also enables the cacheability for
    the K0 segment.  It could has side effects of possibly alter the pre-fetch
    buffer and cache.  It sets the RAM wait states to 0.  Other than
    the SYS_FREQ, this takes these parameters.  The top 3 may be '|'ed
    together:
    
    SYS_CFG_WAIT_STATES (configures flash wait states from system clock)
    SYS_CFG_PB_BUS (configures the PB bus from the system clock)
    SYS_CFG_PCACHE (configures the pCache if used)
    SYS_CFG_ALL (configures the flash wait states, PB bus, and pCache)*/

    /* TODO Add user clock/system configuration code if appropriate.  */
    SYSTEMConfig(SYS_FREQ, SYS_CFG_ALL | SYS_CFG_PCACHE);


    /* Initialize I/O and Peripherals for application */
    InitApp();
    
    //========================================
    // UBW32 Specific setup and configuration:

    // This is how you get the values of the buttons on the UBW32:
    //something = PORTEbits.RE7 // PRG button
    //something = PORTEbits.RE6 // USER button

    // Set up the SPI peripheral:
    // CKP (clock polarity control) = 0
    // CKE (clock edge control) = 1
    // 8-bit, Master Mode
    // Baud = 4MHz (Fpb/20 = 80/20 MHz)
//    SpiChnOpen( 2, SPICON_MSTEN | SPICON_CKE | SPICON_ON | SPICON_MODE32, 20 ); // Old variable names

    // New SPI Peripheral setup config lines:
    // The last value is the baud rate.
    // (80MHz/20MHz) = 4 
    // 3.3333 = 24MHz
    //      8 = 10MHz
    //     20 = 4MHz

    //===================
    // SPI1 setup:
    SpiChnOpen( SPI_CHANNEL1, SPI_OPEN_MSTEN | SPI_OPEN_MODE16, 10 ); // New variable names

    //===================
    // SPI2 setup:
    // 8/16/32 bit transfers at 4MHz:
//    SpiChnOpen( SPI_CHANNEL2, SPI_OPEN_MSTEN | SPI_OPEN_MODE8, 20 ); // New variable names
//    SpiChnOpen( SPI_CHANNEL2, SPI_OPEN_MSTEN | SPI_OPEN_MODE16, 20 ); // 4MHz New variable names
//    SpiChnOpen( SPI_CHANNEL2, SPI_OPEN_MSTEN | SPI_OPEN_MODE32, 20 ); // New variable names

    // 32 bit transfer at 10MHz:
//    SpiChnOpen( SPI_CHANNEL2, SPI_OPEN_MSTEN | SPI_OPEN_MODE32, 8 ); // New variable names

    // 32 bit transfer at 20MHz:
    SpiChnOpen( SPI_CHANNEL2, SPI_OPEN_MSTEN | SPI_OPEN_MODE32, 4 ); // 20MHz New variable names


    //======================
    // SPI Pin setup:
    //
    // Set the demo CS pin (_RG9) as an output, and set it high to release the
    // chip. The pin will be set LOW when we need to write to the device:

//    DAC_TCS = 0; // make CS pin output
//    DAC_CS = 1; // release chip
//    ADC_TCS = 0; // make CS pin output
//    ADC_CS = 1; // release chip

    //======================
    // CODEC SPI Pin setup:
    //
    // SPI1 Control Data/Chip setup:
    CSB_TRIS = 0;
    CSB = 1;

    // SPI2 Audio data setup:
    // In the case of the DACLRC pin, we might be selecting Left/Right channel
    // data portion, or we might be "latching" the data in DSP mode. DSP mode
    // will latch data on high-to-low transition of BCLK.
    //
    // This is important because the BCLK falling edge and the DACLRC must be
    // sync'd with each other. (crap. how are we gonna do that with SPI?)
    //
    DACLRC_TRIS = 0; // Make the DAC L/R Channel select pin an output
    DACLRC = 1;      // Default the DAC L/R Channel select pin to HIGH


    //===================================
    // Wolfson CODEC Configuration setup:
    uint16_t address = 0x0000;
    uint16_t data = 0x000;

    // Power on the whole WM8731!:
    // 0000110 | 0 | 0 0 0 0 0 0 0 0(power ON!, clock on, osc on, line out on, DAC on, ADC on, mic on, line in on)
    // 0000 1100 0000 0000
    setCodec(0xC00);

    // Digital audio interface configuration:
    // 0000 111 | 0 | 0 1 0 0 11 11 (don't invert blck, master mode, dac right data right, msb first rising blck, 32 bit, dsp )
    // 0000 1110 0100 1111
//    setCodec(0xE4F); // master mode
    setCodec(0xE0F); // slave mode

    // Clock configuration:
    // 0x10 is clock config address
    // BC is 88kHz
    // A0 is 44kHz
    setCodec(0x10A0);

    // Activate the audio interface:
    // 0001 001 | 00000000 | 1 (activate interface)
    // 0001 0010 0000 0001
    setCodec(0x1201);

    // Just a random 16 bit value:
//    uint16_t dataValue = 0;
    while(1)
    {
//        LATEbits.LATE0 = PORTFbits.RF0;
        LATEbits.LATE0 = PORTEbits.RE7; // Good idea
        LATEbits.LATE1 = PORTEbits.RE6; // Good idea
//        if (statusLed == false) {
//            statusLed = true;
//        } else {
//            statusLed = false;
//        }
//        LATEbits.LATE1 = statusLed;
        LATEbits.LATE2 = 1; // Good idea
        LATEbits.LATE3 = 1; // Good idea

        // ADC -> DAC passthrough:
//        delay_us(20);
//        dataValue = readADC();
//        delay_us(20);
//        writeDAC(dataValue);
//        IFS0CLR = 0x03800000;   //Clears any existing event (rx / tx/ fault interrupt)

        // Square wave output
//        writeDAC(0x0);
//        delay_us(20);
//        writeDAC(0xFFF);
//        delay_us(20);

        // Read knob value and change a square wave timing based on the value:
//        dataValue = readADC();
//        delay_us(50000);

//        writeDAC(0);
//        delay_us(dataValue);
//        writeDAC(0xFFF);
//        delay_us(dataValue);


//        writeCodec(0xFFFFFFFF, 0x0);
//        writeSPI2_32(0xAAAAAAAF);
//        delay_us(10);

    }

    return 1;
}


// Timer2 Interrupt Service Routine:
// Timer2 has been set to fire at 44.1kHz. This will have to be
void __ISR(_TIMER_2_VECTOR, ipl2) handlesTimer2Ints(void){
    // **make sure iplx matches the timer's interrupt priority level
    // For example: T2_INT_PRIOR_2 = ipl2

    // If we toggle a pin every time this interrupt is called, we should get
    // a square wave out at 22.05kHz; half of the 44.1kHz sampling rate.
    //
    // And we do! Toggling the pin can be done a few ways:
//    if (statusLed == false) {
//        statusLed = true;
//    } else {
//        statusLed = false;
//    }
//    LATEbits.LATE9 = statusLed;
    // This is an easier way to invert LATE9:
//    LATEINV = 0x0200;       // Atomic (completes in a single cycle
//    LATEbits.LATE9 ^= 1;  //Non-atomic (takes longer)

    // So we can run this interrupt EASILY at 44.1kHz and write out our max freq
    // square wave through the DAC:
//    if (statusLed == false) {
//        statusLed = true;
//        writeDAC(0x000);
//    } else {
//        statusLed = false;
//        writeDAC(0xFFF);
//    }

//     Now let's dump that 44.1kHz square wave through the CODEC at 32bit:
    if (statusLed == false) {
        statusLed = true;
//        writeCodec(0x0, 0x0);
    } else {
        statusLed = false;
//        writeCodec(0xFFFFFFFF, 0xFFFFFFFF);
    }
//    LATEbits.LATE9 = statusLed;
//    LATBbits.LATB5 = statusLed;
//    LATEINV = 0x0200;

    // Clear the interrupt flag so that the program returns to the main loop:
    mT2ClearIntFlag();
}
