/*
 * main.c
 *
 * Created: 12-Feb-14 22:58:43
 *  Author: rokko
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "sensors/dht22.h"
#include "lcd/hd44780.h"

// Sensors
#define DHT22 PD0

// LEDs
#define LED1  PD6
#define LED2  PD7

// Buttons
#define BTN1  PE4
#define BTN2  PE5
#define BTN3  PE6
#define BTN4  PE7

// Flash memory variables
const char timer_str_P[] PROGMEM = "%02ld:%02ld:%02ld"; // leading zero, precision 2, long type

// Global variables (variable should be defined as volatile, if it's value may be changed between consecutive statements)
volatile unsigned long timer = 0;	// second timer
char printbuff[16];					// LCD buffer string[16]
float min_t, max_t, min_h, max_h;	// sensor boundary values
struct {
	float t, h;
	int error;
} sensor_data;						// sensor data structure (t, h, error)

void init() {
	// Peripherals
	DDRE = ~((1<<BTN1)|(1<<BTN2)|(1<<BTN3));	// buttons PE4-6
	PORTE = (1<<BTN1)|(1<<BTN2)|(1<<BTN3);		// enable pull-up (logic 1 by default)
	DDRD = (1<<LED1)|(1<<LED2);					// output for LEDs PD6-7
	DDRD &= ~(1<<DHT22);						// input for DHT22 PD0

	// Interrupts
	EIMSK = (1<<INT4)|(1<<INT5)|(1<<INT6);		// enable interrupts INT4 & INT5
	EICRB = (1<<ISC41)|(1<<ISC51)|(1<<ISC61);	// interrupt on button pressed down (logic 0 - falling edge)

	// Timers
	//TCNT0 = 0;								// timer 0 start value of counter (from 0 to 255) - 1 second (when reaches 255)
	ASSR = (1<<AS0);							// enable asynchronous clock from TOSC1 (32,768kHz)
	TIMSK = (1<<TOIE0);							// enable overflow interrupt (TIMER0_OVF_vect)
	TCCR0 = (0<<WGM01)|(0<<WGM00);				// timer 0 mode - Normal
	TCCR0 |= (1<<CS02)|(0<<CS01)|(1<<CS00);		// timer 0 pre-scaler (32.768 / 128 = 256 ticks per second)
	
	//TCNT1 = 0;								// timer 1 start value of counter (from 0 to 65535) - 1 second (when reaches 15.624)
	OCR1A = 15;									// timer 1 top value A for compare interrupt calling - 1 millisecond (15.625 / 1000 = 15)
	TIMSK |= (1<<OCIE1A);						// enable compare interrupt (TIMER1_COMPA_vect)
	TCCR1B = (0<<WGM13)|(1<<WGM12);				// timer 1 mode - CTC (OCRnA TOP)
	TCCR1B |= (1<<CS12)|(0<<CS11)|(1<<CS10);	// timer 1 (another mask <!>) pre-scaler (16.000.000 / 1024 = 15.625 ticks per second)

	// Load values from EEPROM
	min_t = eeprom_read_float(( float *)0);
	max_t = eeprom_read_float(( float *)4);
	min_h = eeprom_read_float(( float *)8);
	max_h = eeprom_read_float(( float *)12);

	lcd_init(LCD_DISP_ON);
	lcd_puts_p(PSTR("Loading..."));
}

void lcd_print_sensor_data(float t, float h, int error) {
	if (error == 0) {
		cli();
		// Print temperature
		lcd_gotoxy(0,0);
		sprintf_P(printbuff, PSTR("Temp: %2.2fC"), (double)t);
		lcd_puts(printbuff);

		// Print humidity
		lcd_gotoxy(0,1);
		sprintf_P(printbuff, PSTR("Humi: %2.2f%%"), (double)h);
		lcd_puts(printbuff);
		sei();
		} else {
		// Print error
		lcd_gotoxy(0,0);
		sprintf_P(printbuff, PSTR("DHT-22 ERROR: %i"), error);
		lcd_puts(printbuff);
	}
}

void lcd_print_time() {
	cli();
	lcd_clrscr();
	lcd_gotoxy(4,0);
	sprintf_P(printbuff, timer_str_P, (timer/3600), ((timer/60)%60), (timer%60));
	lcd_puts(printbuff);
	sei();
}

void save_boundary_values(float t, float h) {
	// Save MIN temperature
	if (fmin(min_t, t) != min_t) {
		min_t = t;
		cli();
		eeprom_write_float(( float *)0 , t);
		sei();
	}
	// Save MAX temperature
	if (fmax(max_t, t) != max_t) {
		max_t = t;
		cli();
		eeprom_write_float(( float *)4 , t);
		sei();
	}
	// Save MIN humidity
	if (fmin(min_h, h) != min_h) {
		min_h = h;
		cli();
		eeprom_write_float(( float *)8 , h);
		sei();
	}
	// Save MAX humidity
	if (fmax(max_h, h) != max_h) {
		max_h = h;
		cli();
		eeprom_write_float(( float *)12 , h);
		sei();
	}
}

void read_sensor_data() {
	sensor_data.error = dht22_read(&sensor_data.t, &sensor_data.h);
	if (sensor_data.error == 0) save_boundary_values(sensor_data.t, sensor_data.h);

	lcd_clrscr();
	lcd_print_sensor_data(sensor_data.t, sensor_data.h, sensor_data.error);
}

// Button 1 interrupt: show MIN temperature and humidity
ISR (INT4_vect) {
	lcd_clrscr();
	lcd_print_sensor_data(min_t, min_h, 0);
	lcd_gotoxy(13,0);
	lcd_puts_p(PSTR("MIN"));
	_delay_ms(100);
}

// Button 2 interrupt: show MAX temperature and humidity
ISR (INT5_vect) {
	lcd_clrscr();
	lcd_print_sensor_data(max_t, max_h, 0);
	lcd_gotoxy(13,0);
	lcd_puts_p(PSTR("MAX"));
	_delay_ms(100);
}

// Button 3 interrupt: show timer
ISR (INT6_vect) {
	lcd_print_time();
	_delay_ms(100);
}

// Timer 0 overflow interrupt
ISR (TIMER0_OVF_vect) {
	timer++;
	if (timer > 86400) timer = 0; // reset after 24 hours
	if ((timer % 2) == 0) read_sensor_data(); // read DHT-22 every 2 seconds
	PORTD ^= (1<<LED1); // toggle LED every second
}

// Timer 1 compare A interrupt
ISR (TIMER1_COMPA_vect) {
	// every 1 ms
	PORTD ^= (1<<LED2);
}

int main(void) {
	init();
	sei(); // enable interrupts

	while(1) {
	}
}

