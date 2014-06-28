#ifndef MAIN_H_
#define MAIN_H_

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include "u8g.h"
#include "m2.h"
#include "m2utl.h"
#include "m2ghu8g.h"

#define TRUE				1
#define FALSE				0

// Buttons
#define BTN1				PN(4, 4)
#define BTN2				PN(4, 5)
#define BTN3				PN(4, 6)
#define BTN4				PN(4, 7)
#define BUTTONS				(1<<BTN1|1<<BTN2|1<<BTN3|1<<BTN4)
// LED 1
#define LED1				PD6
#define LED1_OFF			(PORTD &= ~(1<<LED1))
#define LED1_ON				(PORTD |= (1<<LED1))
#define LED1_TOGGLE			(PORTD ^= (1<<LED1))
// LED 2
#define LED2				PD7
#define LED2_OFF			(PORTD &= ~(1<<LED2))
#define LED2_ON				(PORTD |= (1<<LED2))
#define LED2_TOGGLE			(PORTD ^= (1<<LED2))
// MIC
#define MIC					PB1
#define MIC_OFF				(PORTB &= ~(1<<PB1))
#define MIC_ON				(PORTB |= (1<<PB1))
#define MIC_TOGGLE			(PORTB ^= (1<<PB1))

/* CONSTANTS IN FLASH MEMORY */

/* FUNCTION PROTOTYPES */

#endif /* MAIN_H_ */