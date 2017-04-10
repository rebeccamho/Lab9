// ADCTestmain.c
// Runs on LM4F120/TM4C123
// Provide a function that initializes Timer0A to trigger ADC
// SS3 conversions and request an interrupt when the conversion
// is complete.
// UART runs at 115,200 baud rate 
// Daniel Valvano
// May 3, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// center of X-ohm potentiometer connected to PE3/AIN0
// bottom of X-ohm potentiometer connected to ground
// top of X-ohm potentiometer connected to +3.3V through X/10-ohm ohm resistor
#include <stdint.h>
#include <stdbool.h>
#include "../ValvanoWareTM4C123/ValvanoWareTM4C123/inc/tm4c123gh6pm.h"
#include "ADCT0ATrigger.h"
#include "PLL.h"
#include "UART.h"
#include "ST7735.h"
#include "FIFO.h"
#include "Calib.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

#define F1KHZ 80000000/1000
#define F100HZ 80000000/100
#define fs 100
#define N 64

//debug code
//
// This program periodically samples ADC0 channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.


int main(void){
  PLL_Init(Bus80MHz);                      // 80 MHz system clock
	UART_Init();              // initialize UART device
	UART_OutString("\n\rUART Initialized\n");
  SYSCTL_RCGCGPIO_R |= 0x00000020;         // activate port F
  ADC0_InitTimer0ATriggerSeq3(0, F100HZ); // ADC channel 0, 1 Hz sampling
  GPIO_PORTF_DIR_R |= 0x04;                // make PF2 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x04;             // disable alt funct on PF2
  GPIO_PORTF_DEN_R |= 0x04;                // enable digital I/O on PF2
                                           // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;                  // disable analog functionality on PF
  GPIO_PORTF_DATA_R &= ~0x04;              // turn off LED
	Output_Init();
	UART_OutString("\rHere\n");
	ST7735_FillScreen(ST7735_BLACK); 
	ST7735_SetCursor(0,0); 
	EnableInterrupts();
	
	//Init ST7735
	ST7735_SetCursor(0,0); ST7735_OutString("My awesome machine");
	ST7735_PlotClear(1000,4000);  // range from 0 to 4095
	ST7735_SetCursor(0,1); ST7735_OutString("N=");
	ST7735_SetCursor(0,2); ST7735_OutString("T="); ST7735_sDecOut2(2500);
                        ST7735_OutString(" C");

	int j = 0;
  while(1){
    WaitForInterrupt();
    GPIO_PORTF_DATA_R ^= 0x04;             // toggle LED
		
		bool new_data = false;
		uint32_t ADC_val;
		while(!new_data) { // wait for new data in the FIFO
			new_data = FIFO_Get(&ADC_val);
		}
		
		// convert ADC to Temperature
		// plot Temperature data
		uint32_t Temperature = ConvertADCToTemp(ADC_val);
		ST7735_PlotPoint(Temperature);  // Measured temperature
    if((j&(N-1))==0){          // fs sampling, fs/N samples plotted per second
      ST7735_PlotNextErase();  // overwrites N points on same line
    }
    if((j%fs)==0){    // fs sampling, 1 Hz display of numerical data
      ST7735_SetCursor(3,1); ST7735_OutUDec(ADC_val);            // 0 to 4095
      ST7735_SetCursor(3,2); ST7735_sDecOut2(Temperature); // 0.01 C 
    }
    j++;                       // counts the number of samples
		
		/* used for prep
		uint16_t count = CheckCount();
		if(count == 5) { // ADC values array is full
			break;
		}
		*/
  }
	
	/* used for prep
	uint32_t* ADC_values = GetADCvalues();
	UART_OutString("\n\rADC data =");
	for(int i = 0; i < 5; i++) { // output ADC values to UART
		uint32_t data = ADC_values[i];
		UART_OutUDec(data);
		if(i < 4) { // output comma between all data values
			UART_OutString("\n\r");
		}
	}
	ST7735_OutUDec(ADC_values[0]); 
	*/

}

