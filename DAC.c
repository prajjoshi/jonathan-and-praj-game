// dac.c
// This software configures DAC output
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 3/6/17 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#include <stdint.h>
#include "tm4c123gh6pm.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
	
	SYSCTL_RCGCGPIO_R |= 0x23; // Activate ports A and B
	for(uint8_t i = 0; i < 100; i++); 
	GPIO_PORTB_DIR_R |= 0x0F;		//Port B pins 0-3 are outputs
	GPIO_PORTB_AFSEL_R &= 0x00;
	GPIO_PORTB_DEN_R |= 0x0F;		//Port B pins 0-3 enabled
	GPIO_PORTB_DATA_R &= 0x00;
	GPIO_PORTB_DR8R_R |= 0x0F;
	
	//INIT HEARTBEAT
	GPIO_PORTF_LOCK_R = 0x4C4F434B;
	GPIO_PORTF_CR_R = 0xFF; 
	GPIO_PORTF_DIR_R |= 0x0A;		//Port F pins 3 and 1 are outputs
	GPIO_PORTF_AFSEL_R &= 0x00;
	GPIO_PORTF_PUR_R |= 0x11;
	GPIO_PORTF_DEN_R |= 0x1B; //Port F pins 0,1,3, and 4 are enabled
	GPIO_PORTF_DATA_R &= 0x00;
	GPIO_PORTF_DATA_R |= 0x08;

}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(uint32_t data){
	
	#define PB3210 *((volatile uint32_t *)0x4000503C))


	GPIO_PORTB_DATA_R = data;		//Pins 3-0 output value from waveform array

	
}
