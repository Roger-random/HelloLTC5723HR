/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using MPLAB(c) Code Configurator

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - 4.15
        Device            :  PIC16F18345
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.35
        MPLAB             :  MPLAB X 3.40
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include "mcc_generated_files/mcc.h"

/* Holds the current bit pattern to load into LATC for each 
   of the four digits */
static uint8_t currentPattern[] = {0x0, 0x0, 0x0, 0x0};

/* Which digit we are currently displaying */
static size_t  currentDigit = 0;

/* Whether we are waiting before taking the next measurement */
static bool waitToMeasure = true;

/* The previous voltage reading */
static uint16_t voltPrev = 0;

/*
   Converts unsigned int value from 0 to 15 (0x0 to 0xF) inclusive
   to the byte pattern used to display it on the 7-segment LED. 
   
    Bit to LED position mapping
    
            0
            
        5       1
        
            6
            
        4       2
        
            3       7
            
    
    Binary to Hexadecimal lookup table
    
    3210
    
    0000 0
    0001 1
    0010 2
    0011 3
    0100 4
    0101 5
    0110 6
    0111 7
    1000 8
    1001 9
    1010 A
    1011 B
    1100 C
    1101 D
    1110 E
    1111 F  
*/

uint8_t digitToByte(size_t i)
{
    uint8_t byteValue = 0x00;
    static uint8_t bytePattern[] = 
    {
        0x3F, /* 0 -> 0011 1111 */
        0x06, /* 1 -> 0000 0110 */
        0x5B, /* 2 -> 0101 1011 */
        0x4F, /* 3 -> 0100 1111 */
        0x66, /* 4 -> 0110 0110 */ 
        0x6D, /* 5 -> 0110 1101 */
        0x7D, /* 6 -> 0111 1101 */
        0x07, /* 7 -> 0000 0111 */
        0x7F, /* 8 -> 0111 1111 */
        0x67, /* 9 -> 0110 0111 */
        0x77, /* A -> 0111 0111 */
        0x7C, /* B -> 0111 1100 */
        0x39, /* C -> 0011 1001 */
        0x5E, /* D -> 0101 1110 */
        0x79, /* E -> 0111 1001 */
        0x71  /* F -> 0111 0001 */
    };
    
    if (i < 16)
    {
        byteValue = bytePattern[i];
    }
    
    return byteValue;
}


/* Turn off all digits */
void Digit_Disable()
{
    DIG_0_SetHigh();        
    DIG_1_SetHigh();
    DIG_2_SetHigh();
    DIG_3_SetHigh();
}

/*  To avoid overloading the pin, only one of these may be low 
    at any give time. This function turns everything else off
    before turning the specified one on.
*/
void Digit_Enable(size_t digit)
{
    switch(digit)
    {
        case 0:
            DIG_1_SetHigh();
            DIG_2_SetHigh();
            DIG_3_SetHigh();
            DIG_0_SetLow();
            break;
        case 1:
            DIG_0_SetHigh();
            DIG_2_SetHigh();
            DIG_3_SetHigh();
            DIG_1_SetLow();
            break;
        case 2:
            DIG_0_SetHigh();        
            DIG_1_SetHigh();
            DIG_3_SetHigh();
            DIG_2_SetLow();
            break;
        case 3:
            DIG_0_SetHigh();        
            DIG_1_SetHigh();
            DIG_2_SetHigh();
            DIG_3_SetLow();
            break;
        default:
            Digit_Disable();
            break;
    }
}


/* Called every millisecond by timer 0 interrupt */
void DisplayNextDigit(void)
{
    currentDigit++;
    
    if(currentDigit > 3)
    {
        currentDigit = 0;
    }
    
    Digit_Disable();
    LATC = currentPattern[currentDigit];
    Digit_Enable(currentDigit);
}

/* Called every millisecond by timer 0 interrupt */
void UpdateWaitCount(void)
{
    static uint16_t waitCount = 0;
    
    waitCount++;
    
    if (waitCount > 1000)
    {
        // One second has elapsed, perform measure
        waitToMeasure = false;
        waitCount = 0;
    }
}

/*
                         Main application
 */
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    
    while (1)
    {
        /* Display the raw ADC results in hexadecimal
        
        adc_result_t adcVal = ADC_GetConversion(channel_FVR);
        
        currentPattern[0] = digitToByte(0);
        currentPattern[1] = digitToByte((adcVal>>8) & 0xF);
        currentPattern[2] = digitToByte((adcVal>>4) & 0xF);
        currentPattern[3] = digitToByte(adcVal & 0xF);
        */
        /*
            When FVR is set to X2, the reference value is 2.048V.
            ADC would place 2.048V on a scale of 0-1023 between 0V
            and the input voltage.
            
            ADC result = (2.048/Vin)* 1023
            
            or
            
            Vin = (2.048*1023)/(ADC result)
            
            Since floating-point math on a PIC is expensive, we will
            change the unit of output to thounsandths-of-a-volt so
            an integer division would suffice.
            
            Vin(thousandths) = (2048*1023)/(ADC result)
            Vin(thousandths) = (2095104)/(ADC result)
        */
        
        // Note: ADC_GetConversion will sit in a tight loop waiting
        // for ADC to complete.
        adc_result_t adcVal = ADC_GetConversion(channel_FVR);
        uint32_t adcMult = 2095104;
        
        uint16_t voltRaw = adcMult/adcVal;
        uint16_t voltIn  = voltRaw;
        
        /////////////////////////////////////////////////////////////
        // Raw values are jittery, this section tries to smooth it out.
        
        if (voltRaw > voltPrev && (voltRaw-voltPrev) > 25)
        {
            // Skip smoothing for larger voltage swings.
            voltIn = voltRaw;
        }
        else if (voltPrev > voltRaw && (voltPrev-voltRaw) > 25)
        {
            // Skip smoothing for larger voltage swings.
            voltIn = voltRaw;
        }
        else
        {
            // Adjust smoothing ratio as necessary to get good output.
            uint32_t voltSmooth32 = (uint32_t)voltPrev * 15;
            voltSmooth32 = voltSmooth32 + voltRaw;
            
            voltIn = voltSmooth32/16;
        }
        voltPrev = voltIn;
        
        
        /////////////////////////////////////////////////////////////
        // Translate voltIn into a base-10 number to be displayed.
        
        // 0.00X digit
        currentPattern[3] = digitToByte(voltIn % 10);
        
        // 0.0X0 digit
        voltIn = voltIn / 10;        
        currentPattern[2] = digitToByte(voltIn % 10);
        
        // 0.X00 digit
        voltIn = voltIn / 10;        
        currentPattern[1] = digitToByte(voltIn % 10);
        
        // X.000 digit
        voltIn = voltIn / 10;
        
        // Add decimal point to the X.000 digit
        currentPattern[0] = (uint8_t)(digitToByte(voltIn % 10) | 0x80);
        
        // currentPattern[] is transferred to the LED in DisplayNextDigit
        // which executes as an ISR. This allows the digit update to
        // run at regular intervals independent of how long the ADC
        // process (and follow-up numeric conversion) would take.
        
        // The nature of the ISR means it is possible for the digit update
        // to occur in the middle of receiving a new updated ADC value. So
        // if we were "1.234" and going to "5.678" there's a chance we'll
        // briefly show "1.278" Given the short duration of this event,
        // this program is not worried about it.
        
        waitToMeasure = true;
        while(waitToMeasure)
        {
            // Sit and wait for the next measurement.  waitToMeasure will be 
            // set to false after one second by UpdateWaitCount()
            ;
        }
    }
}
/**
 End of File
*/