#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "sensors/dht22.h"
#include "lcd/hd44780.h"
#include "fsm/messages.h"
#include "fsm/timers.h"
#include "fsm/fsm.h"

/* CONSTANTS & DEFINITIONS */
#define u08 unsigned char		// 0 to 255
#define s08 signed char			// -128 to 127

#define u16 unsigned int		// 0 to 65535
#define s16 signed int			// -32768 to 32767

#define u32 unsigned long int	// 0 to 4294967295
#define s32 signed long int		// -2147483648 to 2147483647

#define f32 float				// ±1.175e-38 to ±3.402e38
#define d32 double				// ±1.175e-38 to ±3.402e38
// Buttons
#define BTN1				PE4
#define BTN2				PE5
#define BTN3				PE6
#define BTN4				PE7
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
// LCD
#define LCD_ROW_1			0
#define LCD_ROW_2			1
#define LCD_MAX_POS			16
// Sensors
#define DHT22				PB0
#define SENSOR_DATA_CURRENT 0
#define SENSOR_DATA_MIN		1
#define SENSOR_DATA_MAX		2
// Error codes
#define SYS_ERR_OK			0
#define SYS_ERR_UNKNOWN		1
// Time constants (ms)
#define LCD_REFRESH_DELAY			200
#define LED_BLINK_DELAY				500
#define SENSOR_READ_DELAY			2000
#define BUTTON_CHECK_DELAY			50
#define BUTTON_DEBOUNCE_DELAY		25
#define BUTTON_DOUBLE_CLICK_DELAY	300
#define BUTTON_LONG_PRESS_DELAY		1000
#define MENU_SHOW_DELAY				3000
#define MENU_REFRESH_DELAY			1000

/* CONSTANTS IN FLASH MEMORY */
#define P_CLOCK_STR			"%02ld:%02ld:%02ld" // leading zero, precision 2, long type
#define P_ERR_CODE_0		"UNKNOWN ERROR"
#define P_TEST_MSG			"Test message"

/* FUNCTION PROTOTYPES */
// LCD
void lcd_screen_refresh(char clear);
void lcd_menu_print(char type);
// sensor
void read_sensor_data();

#endif /* MAIN_H_ */
