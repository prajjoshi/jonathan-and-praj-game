// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 3/6/2015 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014

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
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "Timer0.h"
#include "Sound.h"
#include "DAC.h"


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
void movePlayerUp(void);
void movePlayerDown(void);
void movePlayerLeft(void);
void movePlayerRight(void);

void moveEnemy1Up(void);
void moveEnemy1Down(void);
void moveEnemy1Left(void);
void moveEnemy1Right(void);

void moveEnemy2Up(void);
void moveEnemy2Down(void);
void moveEnemy2Left(void);
void moveEnemy2Right(void);

void moveEnemy3Up(void);
void moveEnemy3Down(void);
void moveEnemy3Left(void);
void moveEnemy3Right(void);

void moveEnemy4Up(void);
void moveEnemy4Down(void);
void moveEnemy4Left(void);
void moveEnemy4Right(void);

void moveEnemy5Up(void);
void moveEnemy5Down(void);
void moveEnemy5Left(void);
void moveEnemy5Right(void);

void joystickio(void);
void checkwave(void);
void checkhit(void);
void enemy1movement(void);
void enemy2movement(void);
void checkattack(void);

void endgame(void);
void win(void);

// *************************** Images ***************************
// enemy ship that starts at the top of the screen (arms/mouth closed)
// width=16 x height=16
const unsigned short EnemyA10HP[] = {

 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x53D7, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x53D7, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x53D7, 0x0000, 0x0000, 0x0000, 0x53D7, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x0000, 0x0000, 0x53D7, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x0000, 0x53D7, 0x53D7, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x7BEF, 0x20FD, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x53D7, 0x53D7, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,


};



// enemy ship that starts at the top of the screen (arms/mouth open)
// width=16 x height=16
const unsigned short EnemyB10HP[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x20FD, 0x0000, 0x0000, 0x0000, 0x0000, 0x20FD, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x20FD, 0x0000, 0x0000, 0x0000, 0x0000, 0x20FD, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x20FD, 0x20FD, 0x0000, 0x0000, 0x20FD, 0x20FD, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x20FD, 0x0000, 0x0000, 0x20FD, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x20FD, 0x20FD, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x23FF, 0x20FD, 0x0000, 0x20FD, 0x20FD, 0x0000, 0x20FD, 0x23FF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x23FF, 0x23FF, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0x23FF, 0x23FF, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,


};



// enemy ship that starts in the middle of the screen (arms together)
// width=16 x height=16
const unsigned short EnemyA20HP[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xCA47, 0xCA47, 0xCA47, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xCA47, 0xCA47, 0xCA47, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x4D84, 0x4D84, 0xCA47, 0xCA47, 0xCA47, 0x4D84, 0x4D84, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x4D84, 0x4D84, 0x53D7, 0x53D7, 0x53D7, 0x4D84, 0x4D84, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xCA47, 0xCA47, 0xCA47, 0xCA47, 0xCA47, 0xCA47, 0xCA47, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xCA47, 0xCA47, 0xCA47, 0xCA47, 0xCA47, 0xCA47, 0xCA47, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xCA47, 0xCA47, 0xCA47, 0xCA47, 0xCA47, 0xCA47, 0xCA47, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x4D84, 0x4D84, 0x4D84, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x4D84, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x4D84, 0x4D84, 0x4D84, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,

};



// enemy ship that starts at the bottom of the screen (arms down)
// width=16 x height=16
const unsigned short EnemyB20HP[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0xA254, 0xA254, 0xA254, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xA254, 0xA254, 0xA254, 0x0000, 0x0000,
 0x0000, 0x0000, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0x0000, 0x0000,
 0x0000, 0x0000, 0xA254, 0xA254, 0xA254, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xA254, 0xA254, 0xA254, 0x0000, 0x0000,
 0x0000, 0x0000, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0x0000, 0x0000,
 0x0000, 0x0000, 0xA254, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0xA254, 0x0000, 0x0000,
 0x0000, 0x0000, 0xA254, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0xA254, 0x0000, 0x0000,
 0x0000, 0x0000, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0xA254, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xA254, 0x0000, 0x0000, 0x0000, 0x0000, 0xA254, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xA254, 0xA254, 0x0000, 0x0000, 0x0000, 0x0000, 0xA254, 0xA254, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0xA254, 0xA254, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xA254, 0xA254, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,

};

// enemy ship that starts at the bottom of the screen (arms up)
// width=24 x height=24
const unsigned short Boss50HP[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7,
 0x53D7, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x53D7, 0x53D7,
 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7,
 0x53D7, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x53D7, 0x53D7,
 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF,
 0x7BEF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7BEF, 0x7BEF,
 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF,
 0x7BEF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7BEF, 0x7BEF,
 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0xC618, 0xC618, 0xC618, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x079F, 0x079F, 0x079F, 0x53D7, 0x53D7, 0x53D7,
 0x53D7, 0xC618, 0xC618, 0xC618, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x079F, 0x079F, 0x53D7, 0x53D7,
 0x53D7, 0x53D7, 0x079F, 0x079F, 0x079F, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x079F, 0x079F, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0xC618, 0xC618, 0xC618, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF,
 0x7BEF, 0xC618, 0xC618, 0xC618, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xC618, 0xC618, 0xC618, 0x7BEF, 0x7BEF,
 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0xC618, 0xC618, 0xC618, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0xC618, 0xC618, 0xC618, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF,
 0x7BEF, 0xC618, 0xC618, 0xC618, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF,
 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF,
 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF,
 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xC618, 0xC618, 0xC618, 0xFFFF, 0xC618, 0xC618, 0xC618, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0xC618, 0xC618, 0xFFFF, 0xFFFF, 0xFFFF, 0xC618, 0xC618, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0xC618, 0xFFFF, 0xFFFF, 0xC618, 0xFFFF, 0xFFFF, 0xC618, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F,
 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x0000, 0x079F, 0x0000, 0x079F, 0x0000, 0x079F, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,

};

// image of the player's ship
// includes two blacked out columns on the left and right sides of the image to prevent smearing when moved 2 pixels to the left or right
// width=16 x height=16
const unsigned short player0[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x53D7, 0x53D7, 0x53D7, 0x53D7, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xED00, 0xED00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xED00, 0xED00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xB73D, 0xB73D, 0xED00, 0xED00, 0xB73D, 0x53D7, 0x7BEF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xCA47, 0xED00, 0xED00, 0xED00, 0xED00, 0xCA47, 0x7BEF, 0x7BEF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xCA47, 0xCA47, 0xCA47, 0xCA47, 0xCA47, 0xCA47, 0x0000, 0x7BEF, 0x7BEF, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xB73D, 0xB73D, 0x0000, 0x0000, 0x0000, 0x0000, 0x7BEF, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xB73D, 0x0000, 0x0000, 0xB73D, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xB73D, 0x20FD, 0x20FD, 0xB73D, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xB73D, 0xB73D, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,


};

const unsigned short black[] = {
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};




const unsigned short brick16x16[] = {
 0x52AA, 0x8430, 0x8430, 0x528A, 0x8C71, 0x8430, 0x8430, 0x528A, 0x8430, 0x8430, 0x94B2, 0x8410, 0x8430, 0x8430, 0x8C51, 0x528A,
 0x8C51, 0x8410, 0x8430, 0x528A, 0x8430, 0x8430, 0x8430, 0x630C, 0x8430, 0x8430, 0x8C71, 0x8430, 0x8430, 0x8430, 0x8430, 0x528A,
 0x8430, 0x8430, 0x8430, 0x52AA, 0x8430, 0x8430, 0x8430, 0x632C, 0x8430, 0x8430, 0x8430, 0x8430, 0x8C51, 0x7BCF, 0x8C51, 0x6B4D,
 0x8430, 0x8430, 0x8430, 0x630C, 0x6B4D, 0x8410, 0x8430, 0x528A, 0x52AA, 0x5ACB, 0x528A, 0x52AA, 0x73AE, 0x73AE, 0x73AE, 0x8410,
 0x52AA, 0x5ACB, 0x8430, 0x528A, 0x528A, 0x528A, 0x8410, 0x4A69, 0x632C, 0x8430, 0x8430, 0x4A69, 0x528A, 0x52AA, 0x52AA, 0x528A,
 0x52AA, 0x8410, 0x8430, 0x6B4D, 0x528A, 0x8C51, 0x8430, 0x8410, 0x8430, 0x8C51, 0x8430, 0x8430, 0x8430, 0x8430, 0x8430, 0x6B6D,
 0x528A, 0x8C51, 0x8430, 0x8430, 0x528A, 0x8430, 0x8430, 0x8430, 0x8430, 0x8430, 0x8430, 0x8C51, 0x8430, 0x8C51, 0x8430, 0x8C51,
 0x528A, 0x8C71, 0x8C51, 0x8430, 0x528A, 0x8430, 0x8430, 0x8430, 0x8430, 0x8410, 0x8430, 0x630C, 0x8C51, 0x8430, 0x8430, 0x52AA,
 0x6B4D, 0x5ACB, 0x528A, 0x8410, 0x52AA, 0x8C51, 0x8430, 0x630C, 0x8430, 0x8430, 0x8430, 0x528A, 0x8430, 0x8430, 0x8430, 0x4A69,
 0x8410, 0x528A, 0x6B4D, 0x52AA, 0x52AA, 0x6B6D, 0x8430, 0x5ACB, 0x8410, 0x8430, 0x52AA, 0x4A69, 0x630C, 0x6B4D, 0x8C71, 0x4A69,
 0x528A, 0x8430, 0x8C71, 0x52AA, 0x528A, 0x528A, 0x632C, 0x4A69, 0x528A, 0x8430, 0x73AE, 0x4A69, 0x52AA, 0x4A69, 0x5ACB, 0x73AE,
 0x6B6D, 0x8C51, 0x8410, 0x8410, 0x9492, 0x8430, 0x8430, 0x5ACB, 0x8430, 0x8430, 0x8430, 0x5ACB, 0x8430, 0x8430, 0x8410, 0x8430,
 0x8C51, 0x8430, 0x8C51, 0x630C, 0x8430, 0x8430, 0x6B4D, 0x52AA, 0x8430, 0x8430, 0x8430, 0x52AA, 0x8430, 0x9492, 0x8430, 0x8C51,
 0x52AA, 0x8430, 0x8430, 0x4A69, 0x8410, 0x8430, 0x8430, 0x528A, 0x8430, 0x8430, 0x8430, 0x5ACB, 0x8430, 0x8430, 0x8430, 0x8430,
 0x4A69, 0x8430, 0x8430, 0x528A, 0x8430, 0x8C51, 0x8430, 0x528A, 0x5ACB, 0x632C, 0x8C51, 0x6B4D, 0x8430, 0x7BCF, 0x8430, 0x8410,
 0x4A69, 0x528A, 0x8430, 0x528A, 0x8430, 0x8430, 0x5ACB, 0x4A69, 0x528A, 0x5ACB, 0x5ACB, 0x52AA, 0x8430, 0x8410, 0x8410, 0x528A,


};


uint32_t Convert(uint32_t input){
  uint32_t pos = 0;
	pos = (104.28*input + 24085)/256;
	return pos;
}



// *************************** Capture image dimensions out of BMP**********

void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = 2000000-1;   // reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1  , or 2?
  NVIC_ST_CTRL_R = 0x07;    // enable SysTick with core clock and interrupts
}

void movement(void){
		joystickio();
		GPIO_PORTF_DATA_R	^= 0x04;	
		enemy1movement();	
		enemy2movement();
		GPIO_PORTF_DATA_R	^= 0x04;	
		checkattack();
		checkhit();
		
		
}



void PortF_Init(void){
	unsigned long volatile delay;
  SYSCTL_RCGCGPIO_R |= 0x21;
	for(int x = 0; x < 100; x++);
	GPIO_PORTF_DIR_R |= 0x0E;	//bits 1,2,3
	GPIO_PORTF_AFSEL_R &= 0x00;
	GPIO_PORTF_DEN_R |= 0x0E;
}

void PortD_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x08;
	while((SYSCTL_RCGCGPIO_R&0x08) == 0){};
	GPIO_PORTD_DIR_R &= ~0x01; //PD0 is input for attack button
	GPIO_PORTD_DEN_R |= 0x01;

}



////////////////////////////////////////////// ALL GLOBAL VARIABLES ARE GOING HERE!!!!!!!!!!!!!!!!!!!!!////////////////////////////////////////////////////


uint32_t mailbox;
uint32_t ADC_Status = 0x00;
uint32_t XPrequired = 4;
uint32_t attackflag  = 0; // 0 when not attacking, 1 when attacking (Player)
uint32_t collisionflag1 = 0; //for Enemy1
uint32_t collisionflag2 = 0; //for Enemy2
uint32_t collisionflag3 = 0; //for enemy3
uint32_t collisionflag4 = 0; //for enemy4
uint32_t collisionflag5 = 0; //for enemy5
uint32_t killcount = 0;
uint32_t wave = 1;
uint8_t i=0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct StateP {  // struct for player
	int x; // x coordinate
	int y; // y-coordinate
	int health; // player's starting health
	int currenthealth; // player's current health
	int damage; // player's damage output (changes based on level)
	int XP; // player's current XP
	int level; //player's current level (changes based on level)
	const unsigned short *image; //ptr -> image
};
	
struct StateE { //struct for enemy
	int x; // x coordinate
	int y; // y-coordinate
	int health; // enemy's starting health
	int currenthealth; // enemy's current health
	int damage; // enemy's damage output
	int XP_Given; // XP given upon death
	int alive; // 1 for alive, 0 for dead
	const unsigned short *image; //ptr -> image
};
	


typedef struct StateP STypP;
typedef struct StateE STypE;

STypP Player;
void InitP(void){
		Player.x = 52; //replace with starting place of player
		Player.y = 160; // same as above
		Player.health = 20;
		Player.currenthealth = 20;
		Player.damage = 5;
		Player.XP = 0;
		Player.level = 0;
		Player.image = player0;
}
STypE Enemy1;
STypE Enemy2;
STypE Enemy3;
STypE Enemy4;
STypE Enemy5;
void InitE1(void){

		Enemy1.x = 0;
		Enemy1.y = 0;
		Enemy1.health = 10;
		Enemy1.currenthealth = 10;
		Enemy1.damage = 0;
		Enemy1.XP_Given = 2;
		Enemy1.alive = 1;
		Enemy1.image = EnemyA10HP;

}	
void InitE2(void){
		Enemy2.x = 0;
		Enemy2.y = 0;
		Enemy2.health = 10;
		Enemy2.currenthealth = 10;
		Enemy2.damage = 0;
		Enemy2.XP_Given = 2;
		Enemy2.alive = 1;
		Enemy2.image = EnemyB10HP;
}	
void InitE3(void){

		Enemy3.x = 0;
		Enemy3.y = 0;
		Enemy3.health = 20;
		Enemy3.currenthealth = 20;
		Enemy3.damage = 5;
		Enemy3.XP_Given = 4;
		Enemy3.alive = 1;
		Enemy3.image = EnemyA20HP;
	
}	
void InitE4(void){

		Enemy4.x = 0;
		Enemy4.y = 0;
		Enemy4.health = 20;
		Enemy4.currenthealth = 20;
		Enemy4.damage = 10;
		Enemy4.XP_Given = 4;
		Enemy4.alive = 1;
		Enemy4.image = EnemyB20HP;
	}

void InitE5(void){
		Enemy5.x = 0;
		Enemy5.y = 0;
		Enemy5.health = 50;
		Enemy5.currenthealth = 50;
		Enemy5.damage = 20;
		Enemy5.XP_Given = 100;
		Enemy5.alive = 1;
		Enemy5.image = Boss50HP;
	}

void endgame(void){
	ST7735_FillScreen(0x0000);
	ST7735_OutString("GAME OVER");
}

void win(void){
	ST7735_FillScreen(0x0000);
	ST7735_OutString("YOU HAVE DEFEATED THE SKELETON KING");
}
	
int random_number;

void randomnumber(void)
	{
  srand (Random());
  random_number = rand();
	random_number = (random_number >>24)%60;
}


void checkattack(void){
	if (GPIO_PORTD_DATA_R == 0x01){ // attack on PD0
		attackflag = 1;
		Sound_Attack();
	}
//	else{
//		attackflag =0;
//	}
	
}


void enemy1movement(void){
	static uint8_t down =1;
	randomnumber();
	uint32_t move = random_number; // 0-59
//	if (0< move < 18){
//		moveEnemy1Up();
//	}
//	if (17 < move < 36){
//		moveEnemy1Left();
//	}
//	if (35 < move < 48){
//		moveEnemy1Right();
//	}
//	if (47 < move < 60){
//		moveEnemy1Down();
//	}
	if(Enemy1.y<140 && down ==1){
	moveEnemy1Down();
	//ST7735_DrawBitmap(Enemy1.x, Enemy1.y, Enemy1.image, 16, 16);
		if(Enemy1.y==139){
			down=0;}
	}
	if(Enemy1.y>70 && down ==0){
		moveEnemy1Up();
		if(Enemy1.y==80){
			down=1;}
		}
}
void enemy2movement(void){
	static uint8_t lr=1;
	randomnumber();
	uint32_t move = random_number; // 0-59
//	if (0< move < 18){
//		moveEnemy1Up();
//	}
//	if (17 < move < 36){
//		moveEnemy1Left();
//	}
//	if (35 < move < 48){
//		moveEnemy1Right();
//	}
//	if (47 < move < 60){
//		moveEnemy1Down();
//	}
	if(Enemy2.x< 89 && lr ==1){
	moveEnemy2Right();
	//ST7735_DrawBitmap(Enemy1.x, Enemy1.y, Enemy1.image, 16, 16);
		if(Enemy2.x==88){
			lr=0;}
	}
	if(Enemy1.x>20 && lr ==0){
		moveEnemy2Left();
		if(Enemy2.x==30){
			lr=1;}
		}
}

void joystickio(void){
	unsigned int datain[2];
	ADC_In(datain);
	if (datain[1] <1500){
		movePlayerDown();
	}
	if (datain[1] >3000){
		movePlayerUp();
	}
	if (datain[0] <1500){
		movePlayerLeft();
	}
	if (datain[0] >3000){
		movePlayerRight();
	}
}


int LevelChange(){
		if(Player.level <= 5){
		Player.damage = Player.damage + 5;
		XPrequired = XPrequired + 4;
		Player.health = Player.health + 5;
		Player.currenthealth = Player.health;
		return XPrequired;
		}
		else 
			return Player.level = 5;
}
	
void checkwave(void){
	if(wave == 1){
		//static uint8_t i=0;
		if(i==0){
		Enemy1.x = 52;
		Enemy1.y = 100;
		Enemy2.x = 60;
		Enemy2.y = 80;
		i++;
		Enemy3.alive=Enemy4.alive=Enemy5.alive=0;
		}
	//	ST7735_DrawBitmap(Enemy1.x,Enemy1.y, Enemy1.image, 16,16);
	//	ST7735_DrawBitmap(Enemy2.x,Enemy2.y, Enemy2.image, 16,16);
		
		if(Enemy1.alive==1){
		ST7735_DrawBitmap(Enemy1.x,Enemy1.y, Enemy1.image, 16,16);
		}
		else{
			static uint8_t s=0;
				if (s==0){
					ST7735_DrawBitmap(Enemy1.x, Enemy1.y, black, 16, 16);
					s++;
				}
			}
		if(Enemy2.alive==1){
		ST7735_DrawBitmap(Enemy2.x,Enemy2.y, Enemy2.image, 16,16);
		}
		else{
			static uint8_t v=0;
				if (v==0){
					ST7735_DrawBitmap(Enemy2.x, Enemy2.y, black, 16, 16);
					v++;
				}
			}
	}
	
	if(killcount == 2 && wave == 1){ 										// wave 1 finished, health restored and enemy 2 coordinates changed
		wave++;
	//	ST7735_DrawBitmap(Enemy1.x, Enemy1.y, black, 16, 16);
		//ST7735_DrawBitmap(Enemy2.x, Enemy2.y, black, 16, 16);
		Enemy2.x = 70;
		Enemy2.y = 123;
		Enemy2.currenthealth = Enemy2.health;
		Enemy2.alive = 1;
		Player.currenthealth = Player.health;
		Enemy3.x = 68;
		Enemy3.y = 100;
		Enemy3.alive=1;
		killcount = 0;
	}
	if(wave == 2){
//		static uint8_t h=0;
//		if(h==0){
//		Enemy3.x = 68;
//		Enemy3.y = 100;
//			h++;
//		}
		if(Enemy2.alive==1){
		ST7735_DrawBitmap(Enemy2.x,Enemy2.y, Enemy2.image, 16,16);
		}
		else{
			static uint8_t j=0;
				if (j==0){
					ST7735_DrawBitmap(Enemy2.x, Enemy2.y, black, 16, 16);
					j++;
				}
			}
		if(Enemy3.alive==1){
		ST7735_DrawBitmap(Enemy3.x,Enemy3.y, Enemy3.image, 16,16);
		}
		else{
			static uint8_t k=0;
				if (k==0){
					ST7735_DrawBitmap(Enemy3.x, Enemy3.y, black, 16, 16);
					k++;
				}
			}
}
	if(killcount == 2 && wave == 2){ 										// wave 2 finished, health restored and enemy 3 coordinates changed
		wave++;
	//	ST7735_DrawBitmap(Enemy2.x, Enemy2.y, black, 16, 16);
	//	ST7735_DrawBitmap(Enemy3.x, Enemy3.y, black, 16, 16);
		Enemy3.x = 70;
		Enemy3.y = 123;
		Enemy3.currenthealth = Enemy3.health;
		Enemy3.alive = 1;
		Player.currenthealth = Player.health;
		Enemy4.x = 90;
		Enemy4.y = 70;
		Enemy4.alive=1;
		Enemy4.currenthealth=Enemy4.health;
		killcount = 0;
	}
	
	if(wave == 3){
	//	Enemy4.x = 90;
	//	Enemy4.y = 70;
	//	ST7735_DrawBitmap(Enemy3.x,Enemy3.y, Enemy3.image, 16,16);
	//	ST7735_DrawBitmap(Enemy4.x,Enemy4.y, Enemy4.image, 16,16);
		if(Enemy3.alive==1){
		ST7735_DrawBitmap(Enemy3.x,Enemy3.y, Enemy3.image, 16,16);
		}
		else{
			static uint8_t q=0;
				if (q==0){
					ST7735_DrawBitmap(Enemy3.x, Enemy3.y, black, 16, 16);
					q++;
				}
			}
		if(Enemy4.alive==1){
		ST7735_DrawBitmap(Enemy4.x,Enemy4.y, Enemy4.image, 16,16);
		}
		else{
			static uint8_t b=0;
				if (b==0){
					ST7735_DrawBitmap(Enemy4.x, Enemy4.y, black, 16, 16);
					b++;
				}
			}
}
	if(killcount == 2 && wave == 3){ 										// wave 3 finished, health restored and enemy 1,2,3,4 coordinates changed
		wave++;
	//	ST7735_DrawBitmap(Enemy3.x, Enemy3.y, black, 16, 16);
	//	ST7735_DrawBitmap(Enemy4.x, Enemy4.y, black, 16, 16);
		Enemy4.x = 45;
		Enemy4.y = 70;
		Enemy4.currenthealth = Enemy4.health;
		Enemy1.currenthealth = Enemy1.health;
		Enemy2.currenthealth = Enemy2.health;
		Enemy3.currenthealth = Enemy3.health;
		Enemy4.currenthealth = Enemy4.health;
		Enemy1.alive = 1;
		Enemy2.alive = 1;
		Enemy3.alive = 1;
		Enemy4.alive = 1;
		Player.currenthealth = Player.health;
		killcount = 0;
	}
	
		if(wave == 4){
//		ST7735_DrawBitmap(Enemy1.x,Enemy1.y, Enemy1.image, 16,16);
//		ST7735_DrawBitmap(Enemy2.x,Enemy2.y, Enemy2.image, 16,16);
//		ST7735_DrawBitmap(Enemy3.x,Enemy3.y, Enemy3.image, 16,16);
//		ST7735_DrawBitmap(Enemy4.x,Enemy4.y, Enemy4.image, 16,16);
			
		if(Enemy1.alive==1){
		ST7735_DrawBitmap(Enemy1.x,Enemy1.y, Enemy1.image, 16,16);
		}
		else{
			static uint8_t r=0;
				if (r==0){
					ST7735_DrawBitmap(Enemy1.x, Enemy1.y, black, 16, 16);
					r++;
				}
			}
		if(Enemy2.alive==1){
		ST7735_DrawBitmap(Enemy2.x,Enemy2.y, Enemy2.image, 16,16);
		}
		else{
			static uint8_t t=0;
				if (t==0){
					ST7735_DrawBitmap(Enemy2.x, Enemy2.y, black, 16, 16);
					t++;
				}
			}
		if(Enemy3.alive==1){
		ST7735_DrawBitmap(Enemy3.x,Enemy3.y, Enemy3.image, 16,16);
		}
		else{
			static uint8_t y=0;
				if (y==0){
					ST7735_DrawBitmap(Enemy3.x, Enemy3.y, black, 16, 16);
					y++;
				}
			}
		if(Enemy4.alive==1){
		ST7735_DrawBitmap(Enemy4.x,Enemy4.y, Enemy4.image, 16,16);
		}
		else{
			static uint8_t e=0;
				if (e==0){
					ST7735_DrawBitmap(Enemy4.x, Enemy4.y, black, 16, 16);
					e++;
				}
			}

}
	if(killcount == 4 && wave == 4){ 										// wave 4 finished, health restored and boss round next
		wave++;
//		ST7735_DrawBitmap(Enemy1.x, Enemy1.y, black, 16, 16);
//		ST7735_DrawBitmap(Enemy2.x, Enemy2.y, black, 16, 16);
//		ST7735_DrawBitmap(Enemy3.x, Enemy3.y, black, 16, 16);
//		ST7735_DrawBitmap(Enemy4.x, Enemy4.y, black, 16, 16);
		Player.currenthealth = Player.health;
		killcount = 0;
		Enemy5.x = 52;
		Enemy5.y = 100;				
		Enemy5.alive=1;
	}
	
	if(wave == 5){
//		Enemy5.x = 52;
//		Enemy5.y = 100;									
		// BOSS WAVE / FINAL WAVE FUCK YEAH
		if(Enemy5.alive==1){
		ST7735_DrawBitmap(Enemy5.x,Enemy5.y, Enemy5.image, 24,24);
		}
		else{
			ST7735_DrawBitmap(Enemy5.x, Enemy5.y, black, 24, 24);
	}
	
	if(killcount == 1 && wave == 5){
		
		//ST7735_DrawBitmap(Enemy5.x, Enemy5.y, black, 24, 24);
		win(); //IMPLEMENT THE WIN!!!!!!!!!
	}
	
		
}
	}
	
	
void XPcheck(void){
	if(Player.XP == XPrequired){
		Player.XP = 0;
		Player.level ++;
		LevelChange();
	}
	
	
	else if(Player.XP >= XPrequired){
		Player.XP = Player.XP - XPrequired;
		Player.level ++;
		LevelChange();
		
	}
	
}



void checkhit(void){
	if(Player.x + 16 >= Enemy1.x && Enemy1.x + 16 >= Player.x && Player.y + 16 >= Enemy1.y && Enemy1.y + 16 >= Player.y ){
		collisionflag1 = 1;
	}

	if(Player.x + 8 >= Enemy2.x && Enemy2.x + 8 >= Player.x && Player.y + 8 >= Enemy2.y && Enemy2.y + 8 >= Player.y){
		collisionflag2= 1;
	}

	if(Player.x + 8 >= Enemy3.x && Enemy3.x + 8 >= Player.x && Player.y + 8 >= Enemy3.y && Enemy3.y + 8 >= Player.y){
		collisionflag3= 1;
	}

	if(Player.x + 8 >= Enemy4.x && Enemy4.x + 8 >= Player.x && Player.y + 8 >= Enemy4.y && Enemy4.y + 8 >= Player.y){
		collisionflag4 = 1;
	}

	if(Player.x + 8 >= Enemy5.x && Enemy5.x + 8 >= Player.x && Player.y + 8 >= Enemy5.y && Enemy5.y + 8 >= Player.y){
		collisionflag5 = 1;
	}

	if(collisionflag1 == 1 && Enemy1.alive == 1){
	checkattack();
	if(attackflag == 1){
		Enemy1.currenthealth = Enemy1.currenthealth - Player.damage;
		attackflag = 0;
		if(Enemy1.currenthealth <= 0){
			Player.XP = Player.XP + Enemy1.XP_Given;
			XPcheck();
			killcount++;
			Enemy1.alive = 0;
			
	}
}
	else{
		Player.currenthealth = Player.currenthealth - Enemy1.damage;
		if(Player.currenthealth == 0){
			endgame(); // implement this function!
		}

	}
	}
	else if(collisionflag2 == 1 && Enemy2.alive == 1){
	checkattack();
	if(attackflag == 1){
		Enemy2.currenthealth = Enemy2.currenthealth - Player.damage;
		attackflag = 0;
		if(Enemy2.currenthealth <= 0){
			Player.XP = Player.XP + Enemy2.XP_Given;
			XPcheck();
			killcount++;
			Enemy2.alive = 0;
	}
}
	else{
		Player.currenthealth = Player.currenthealth - Enemy2.damage;
		if(Player.currenthealth == 0){
			endgame(); // implement this function!
		}
	}

	}
	else if(collisionflag3 == 1 && Enemy3.alive == 1){
	checkattack();
	if(attackflag == 1){
		Enemy3.currenthealth = Enemy3.currenthealth - Player.damage;
		attackflag = 0;
		if(Enemy3.currenthealth <= 0){
			Player.XP = Player.XP + Enemy3.XP_Given;
			XPcheck();
			killcount++;
			Enemy3.alive = 0;
	}
}
	
else{
		Player.currenthealth = Player.currenthealth - Enemy3.damage;
		if(Player.currenthealth == 0){
			endgame(); // implement this function!
		}
	}


	}
	else if(collisionflag4 == 1 && Enemy4.alive == 1){
	checkattack();
	if(attackflag == 1){
		Enemy4.currenthealth = Enemy4.currenthealth - Player.damage;
		attackflag = 0;
		if(Enemy4.currenthealth <= 0){
			Player.XP = Player.XP + Enemy4.XP_Given;
			XPcheck();
			killcount++;
			Enemy4.alive = 0;
	}
}
	
else{
		Player.currenthealth = Player.currenthealth - Enemy4.damage;
		if(Player.currenthealth == 0){
			endgame(); // implement this function!
		}
	}


	}
	else if(collisionflag5 == 1 && Enemy5.alive == 1){
	checkattack();
	if(attackflag == 1){
		Enemy5.currenthealth = Enemy5.currenthealth - Player.damage;
		attackflag = 0;
		if(Enemy5.currenthealth <= 0){
			Player.XP = Player.XP + Enemy5.XP_Given;
			XPcheck();
			killcount++;
			Enemy5.alive = 0;
	}
}
else{
		Player.currenthealth = Player.currenthealth - Enemy5.damage;
		if(Player.currenthealth == 0){
			endgame(); // implement this function!
		}
	}	


	}
		else{
		collisionflag1 = 0;
		collisionflag2 = 0;
		collisionflag3 = 0;
		collisionflag4 = 0;
		collisionflag5 = 0;
	}

}	





int main(void){
  TExaS_Init();  // set system clock to 80 MHz
  Random_Init(1);
	ADC_Init();
	DAC_Init();
	//SysTick_Init();
//	Sound_Init();
	PortF_Init();
	PortD_Init();

	
	

  Output_Init();
  ST7735_FillScreen(0x0000);            // set screen to black

  InitP();
	InitE1();
	InitE2();
	InitE3();
	InitE4();
	InitE5();
	
	
  ST7735_DrawBitmap(Player.x, Player.y, player0, 16,16); // player ship middle bottom
	ST7735_DrawBitmap(Enemy1.x,Enemy1.y, Enemy1.image, 16,16);
	ST7735_FillRect(0,0,128,33,0xBC8E);


  ST7735_DrawBitmap(0,52, brick16x16, 16,16);
  ST7735_DrawBitmap(0,68, brick16x16, 16,16);
	ST7735_DrawBitmap(0,80, brick16x16, 16,16);
	ST7735_DrawBitmap(0,96, brick16x16, 16,16);
	ST7735_DrawBitmap(0,112, brick16x16, 16,16);
	ST7735_DrawBitmap(16,52, brick16x16, 16,16);
	ST7735_DrawBitmap(32,52, brick16x16, 16,16);
	
	ST7735_DrawBitmap(0,160, brick16x16, 16,16);
  ST7735_DrawBitmap(0,128, brick16x16, 16,16);
	ST7735_DrawBitmap(0,144, brick16x16, 16,16);
	ST7735_DrawBitmap(16,160, brick16x16, 16,16);
	ST7735_DrawBitmap(32,160, brick16x16, 16,16);
	
	ST7735_DrawBitmap(83,52, brick16x16, 16,16);
  ST7735_DrawBitmap(111, 68, brick16x16, 16,16);
	ST7735_DrawBitmap(111,80, brick16x16, 16,16);
	ST7735_DrawBitmap(111,96, brick16x16, 16,16);
	ST7735_DrawBitmap(111,112, brick16x16, 16,16);
	ST7735_DrawBitmap(99,52, brick16x16, 16,16);
	ST7735_DrawBitmap(111,52, brick16x16, 16,16);
	
	ST7735_DrawBitmap(83,160, brick16x16, 16,16);
  ST7735_DrawBitmap(99,160, brick16x16, 16,16);
	ST7735_DrawBitmap(111,160, brick16x16, 16,16);
	ST7735_DrawBitmap(111,144, brick16x16, 16,16);
	ST7735_DrawBitmap(111,128, brick16x16, 16,16);
	

		
		EnableInterrupts();
		
		Timer0_Init(*movement, 800000);
	
	
		while( Player.currenthealth >0){
			
			if(Player.currenthealth <= 0){
				endgame();
			}
			ST7735_DrawBitmap(Player.x, Player.y, Player.image, 16, 16);
			checkwave();
		}
		DisableInterrupts();
		ST7735_FillScreen(0x000000);
		ST7735_SetCursor(0,0);
		ST7735_OutString("End of Game");
		ST7735_SetCursor(0,20);
		ST7735_OutString("XP:");
		ST7735_SetCursor(25,20);
		ST7735_OutUDec(Player.XP);
			
		

}

void movePlayerUp(void){
	if ((Player.x> 40) && ((Player.x +16) < 90)){
		if(Player.y > 52){
		Player.y= (Player.y) -1;
	
		}
	}
	else /*(Player.x>16 && (Player.x + 16) < 111)*/{
		if((Player.y)> 68){
		Player.y= (Player.y) -1;
		
		}
	}
//	else{
//		if((Player.y+16) > 80){
//		Player.y= (Player.y) -1;
//		
//		}
//	}
			
}

void movePlayerDown(void){
	if ((Player.x> 49) && ((Player.x +16) < 83)){
		if(Player.y < 160){
		Player.y= (Player.y) +1;
		
		}
	}
	else /*if (Player.x>16 && (Player.x + 16) < 111)*/{
		if(Player.y < 144){
		Player.y= (Player.y) +1;
		
		}
	}
/*	else{
		if(Player.y < 112){
		Player.y= (Player.y) +1;
		
		}
	}
		*/	
}

void movePlayerRight(void){
//	if ((Player.y<112 ) && ((Player.y +16) > 80)){
//		if((Player.x + 16) < 128){
//		Player.x= (Player.x) +1;
//		
//		}
//	}
	if (Player.y<144 && Player.y > 65){
		if(Player.x < 95){
		Player.x= (Player.x) +1;
		
		}
	}
	else{
		if(Player.x < 67){
		Player.x= (Player.x) +1;
		
		}
	}
			
}

void movePlayerLeft(void){
//	if ((Player.y<112) && ((Player.y +16) > 80)){
//		if(Player.x> 0){
//		Player.x= (Player.x) -1;
//		
//		}
//	}
	if (Player.y<150 && Player.y > 68){
		if(Player.x > 16){
		Player.x= (Player.x) -1;
		
		}
	}
	else{
		if(Player.x > 48){
		Player.x= (Player.x) -1;
		}
	}
			
}


void moveEnemy1Up(void){
	if ((Enemy1.x> 40) && ((Enemy1.x +16) < 90)){
		if(Enemy1.y > 52){
		Enemy1.y= (Enemy1.y) -1;
	
		}
	}
	else /*(Player.x>16 && (Player.x + 16) < 111)*/{
		if((Enemy1.y)> 68){
		Enemy1.y= (Enemy1.y) -1;
		
		}
	}
//	else{
//		if((Player.y+16) > 80){
//		Player.y= (Player.y) -1;
//		
//		}
//	}
			
}
void moveEnemy1Down(void){
	if ((Enemy1.x> 49) && ((Enemy1.x +16) < 83)){
		if(Enemy1.y < 160){
		Enemy1.y= (Enemy1.y) +1;
		
		}
	}
	else /*if (Player.x>16 && (Player.x + 16) < 111)*/{
		if(Enemy1.y < 144){
		Enemy1.y= (Enemy1.y) +1;
		
		}
	}
/*	else{
		if(Player.y < 112){
		Player.y= (Player.y) +1;
		
		}
	}
		*/	
}

void moveEnemy1Right(void){
//	if ((Player.y<112 ) && ((Player.y +16) > 80)){
//		if((Player.x + 16) < 128){
//		Player.x= (Player.x) +1;
//		
//		}
//	}
	if (Enemy1.y<144 && Enemy1.y > 65){
		if(Enemy1.x < 95){
		Enemy1.x= (Enemy1.x) +1;
		
		}
	}
	else{
		if(Enemy1.x < 67){
		Enemy1.x= (Enemy1.x) +1;
		
		}
	}
			
}

void moveEnemy1Left(void){
//	if ((Player.y<112) && ((Player.y +16) > 80)){
//		if(Player.x> 0){
//		Player.x= (Player.x) -1;
//		
//		}
//	}
	if (Enemy1.y<150 && Enemy1.y > 68){
		if(Enemy1.x > 16){
		Enemy1.x= (Enemy1.x) -1;
		
		}
	}
	else{
		if(Enemy1.x > 48){
		Enemy1.x= (Enemy1.x) -1;
		}
	}
			
}








void moveEnemy2Up(void){
	if ((Enemy2.x> 40) && ((Enemy2.x +16) < 90)){
		if(Enemy2.y > 52){
		Enemy2.y= (Enemy2.y) -1;
	
		}
	}
	else /*(Player.x>16 && (Player.x + 16) < 111)*/{
		if((Enemy2.y)> 68){
		Enemy2.y= (Enemy2.y) -1;
		
		}
	}
//	else{
//		if((Player.y+16) > 80){
//		Player.y= (Player.y) -1;
//		
//		}
//	}
			
}
void moveEnemy2Down(void){
	if ((Enemy2.x> 49) && ((Enemy2.x +16) < 83)){
		if(Enemy2.y < 160){
		Enemy2.y= (Enemy2.y) +1;
		
		}
	}
	else /*if (Player.x>16 && (Player.x + 16) < 111)*/{
		if(Enemy2.y < 144){
		Enemy2.y= (Enemy2.y) +1;
		
		}
	}
/*	else{
		if(Player.y < 112){
		Player.y= (Player.y) +1;
		
		}
	}
		*/	
}

void moveEnemy2Right(void){
//	if ((Player.y<112 ) && ((Player.y +16) > 80)){
//		if((Player.x + 16) < 128){
//		Player.x= (Player.x) +1;
//		
//		}
//	}
	if (Enemy2.y<144 && Enemy2.y > 65){
		if(Enemy2.x < 95){
		Enemy2.x= (Enemy2.x) +1;
		
		}
	}
	else{
		if(Enemy2.x < 67){
		Enemy2.x= (Enemy2.x) +1;
		
		}
	}
			
}

void moveEnemy2Left(void){
//	if ((Player.y<112) && ((Player.y +16) > 80)){
//		if(Player.x> 0){
//		Player.x= (Player.x) -1;
//		
//		}
//	}
	if (Enemy2.y<150 && Enemy2.y > 68){
		if(Enemy2.x > 16){
		Enemy2.x= (Enemy2.x) -1;
		
		}
	}
	else{
		if(Enemy2.x > 48){
		Enemy2.x= (Enemy2.x) -1;
		}
	}
			
}


void moveEnemy3Up(void){
	if ((Enemy3.x> 40) && ((Enemy3.x +16) < 90)){
		if(Enemy3.y > 52){
		Enemy3.y= (Enemy3.y) -1;
	
		}
	}
	else /*(Player.x>16 && (Player.x + 16) < 111)*/{
		if((Enemy3.y)> 68){
		Enemy3.y= (Enemy3.y) -1;
		
		}
	}
//	else{
//		if((Player.y+16) > 80){
//		Player.y= (Player.y) -1;
//		
//		}
//	}
			
}
void moveEnemy3Down(void){
	if ((Enemy3.x> 49) && ((Enemy3.x +16) < 83)){
		if(Enemy3.y < 160){
		Enemy3.y= (Enemy3.y) +1;
		
		}
	}
	else /*if (Player.x>16 && (Player.x + 16) < 111)*/{
		if(Enemy3.y < 144){
		Enemy3.y= (Enemy3.y) +1;
		
		}
	}
/*	else{
		if(Player.y < 112){
		Player.y= (Player.y) +1;
		
		}
	}
		*/	
}

void moveEnemy3Right(void){
//	if ((Player.y<112 ) && ((Player.y +16) > 80)){
//		if((Player.x + 16) < 128){
//		Player.x= (Player.x) +1;
//		
//		}
//	}
	if (Enemy3.y<144 && Enemy3.y > 65){
		if(Enemy3.x < 95){
		Enemy3.x= (Enemy3.x) +1;
		
		}
	}
	else{
		if(Enemy3.x < 67){
		Enemy3.x= (Enemy3.x) +1;
		
		}
	}
			
}

void moveEnemy3Left(void){
//	if ((Player.y<112) && ((Player.y +16) > 80)){
//		if(Player.x> 0){
//		Player.x= (Player.x) -1;
//		
//		}
//	}
	if (Enemy3.y<150 && Enemy3.y > 68){
		if(Enemy3.x > 16){
		Enemy3.x= (Enemy3.x) -1;
		
		}
	}
	else{
		if(Enemy3.x > 48){
		Enemy3.x= (Enemy2.x) -1;
		}
	}
			
}








void moveEnemy4Up(void){
	if ((Enemy4.x> 40) && ((Enemy4.x +16) < 90)){
		if(Enemy4.y > 52){
		Enemy4.y= (Enemy4.y) -1;
	
		}
	}
	else /*(Player.x>16 && (Player.x + 16) < 111)*/{
		if((Enemy4.y)> 68){
		Enemy4.y= (Enemy4.y) -1;
		
		}
	}
//	else{
//		if((Player.y+16) > 80){
//		Player.y= (Player.y) -1;
//		
//		}
//	}
			
}
void moveEnemy4Down(void){
	if ((Enemy4.x> 49) && ((Enemy4.x +16) < 83)){
		if(Enemy4.y < 160){
		Enemy4.y= (Enemy4.y) +1;
		
		}
	}
	else /*if (Player.x>16 && (Player.x + 16) < 111)*/{
		if(Enemy4.y < 144){
		Enemy4.y= (Enemy4.y) +1;
		
		}
	}
/*	else{
		if(Player.y < 112){
		Player.y= (Player.y) +1;
		
		}
	}
		*/	
}

void moveEnemy4Right(void){
//	if ((Player.y<112 ) && ((Player.y +16) > 80)){
//		if((Player.x + 16) < 128){
//		Player.x= (Player.x) +1;
//		
//		}
//	}
	if (Enemy4.y<144 && Enemy4.y > 65){
		if(Enemy4.x < 95){
		Enemy4.x= (Enemy4.x) +1;
		
		}
	}
	else{
		if(Enemy4.x < 67){
		Enemy4.x= (Enemy4.x) +1;
		
		}
	}
			
}

void moveEnemy4Left(void){
//	if ((Player.y<112) && ((Player.y +16) > 80)){
//		if(Player.x> 0){
//		Player.x= (Player.x) -1;
//		
//		}
//	}
	if (Enemy4.y<150 && Enemy4.y > 68){
		if(Enemy4.x > 16){
		Enemy4.x= (Enemy4.x) -1;
		
		}
	}
	else{
		if(Enemy4.x > 48){
		Enemy4.x= (Enemy4.x) -1;
		}
	}
			
}









void moveEnemy5Up(void){
	if ((Enemy5.x> 40) && ((Enemy5.x +16) < 90)){
		if(Enemy5.y > 52){
		Enemy5.y= (Enemy5.y) -1;
	
		}
	}
	else /*(Player.x>16 && (Player.x + 16) < 111)*/{
		if((Enemy5.y)> 68){
		Enemy5.y= (Enemy5.y) -1;
		
		}
	}
//	else{
//		if((Player.y+16) > 80){
//		Player.y= (Player.y) -1;
//		
//		}
//	}
			
}
void moveEnemy5Down(void){
	if ((Enemy5.x> 49) && ((Enemy5.x +16) < 83)){
		if(Enemy5.y < 160){
		Enemy5.y= (Enemy5.y) +1;
		
		}
	}
	else /*if (Player.x>16 && (Player.x + 16) < 111)*/{
		if(Enemy5.y < 144){
		Enemy5.y= (Enemy5.y) +1;
		
		}
	}
/*	else{
		if(Player.y < 112){
		Player.y= (Player.y) +1;
		
		}
	}
		*/	
}

void moveEnemy5Right(void){
//	if ((Player.y<112 ) && ((Player.y +16) > 80)){
//		if((Player.x + 16) < 128){
//		Player.x= (Player.x) +1;
//		
//		}
//	}
	if (Enemy5.y<144 && Enemy5.y > 65){
		if(Enemy5.x < 95){
		Enemy5.x= (Enemy5.x) +1;
		
		}
	}
	else{
		if(Enemy5.x < 67){
		Enemy5.x= (Enemy5.x) +1;
		
		}
	}
			
}

void moveEnemy5Left(void){
//	if ((Player.y<112) && ((Player.y +16) > 80)){
//		if(Player.x> 0){
//		Player.x= (Player.x) -1;
//		
//		}
//	}
	if (Enemy5.y<150 && Enemy5.y > 68){
		if(Enemy5.x > 16){
		Enemy5.x= (Enemy5.x) -1;
		
		}
	}
	else{
		if(Enemy5.x > 48){
		Enemy5.x= (Enemy5.x) -1;
		}
	}
			
}







// You can use this timer only if you learn how it works

void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 72724;  // 0.1sec at 80 MHz
    while(time>0){
	  	time--;
    }
    count--;
  }

}

