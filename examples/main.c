/*
 * main.c
 *
 * Created: 12-Feb-14 22:58:43
 *  Author: rokko
 */ 

#include "main.h"

/* GLOBAL VARIABLES (variable should be defined as volatile, if it's value may be changed between consecutive actions) */
volatile unsigned long s_timer = 0;	// second timer
unsigned long ms_timer = 0;			// millisecond timer
float min_t, max_t, min_h, max_h;	// sensor boundary values
volatile char sys_error = SYS_ERR_OK;

struct S_LCD_SCREEN_BUFF {
	char row1[LCD_MAX_POS];
	char row2[LCD_MAX_POS];
} lcd_screen_buff; // LCD 2x16

struct S_SENSOR_DATA {
	float t, h;
	char error;
} sensor_data; // DHT-22

/* FLASH MEMORY */
const char clock_str[] PROGMEM = P_CLOCK_STR;
const char test_msg[] PROGMEM = P_TEST_MSG;
const char err_str_0[] PROGMEM = P_ERR_CODE_0;
PGM_P const err_strings[] PROGMEM = { err_str_0 };

/* FUNCTIONS */
void lcd_screen_refresh(char clear) {
	lcd_clrscr();
	if (clear) { return; }
	lcd_gotoxy(0, LCD_ROW_1);
	lcd_puts(lcd_screen_buff.row1);
	lcd_gotoxy(0, LCD_ROW_2);
	lcd_puts(lcd_screen_buff.row2);
}

void lcd_screen_buff_update(char row, char col, char *str) {
	switch(row) {
		case LCD_ROW_1: { memmove((lcd_screen_buff.row1 + col), str, strlen(str)); } break;
		case LCD_ROW_2: { memmove((lcd_screen_buff.row2 + col), str, strlen(str)); } break;
		default: {} break;
	}
}

void lcd_buff_clear() {
	memset(lcd_screen_buff.row1, ' ', LCD_MAX_POS);
	memset(lcd_screen_buff.row2, ' ', LCD_MAX_POS);
	// add NULL terminated strings
	lcd_screen_buff.row1[LCD_MAX_POS-1] = '\0';
	lcd_screen_buff.row2[LCD_MAX_POS-1] = '\0';
}

void lcd_print_sensor_data(float t, float h, char error) {
	char print_buff[LCD_MAX_POS]; // LCD buffer for sprintf
	if (error == 0) {
		// Print temperature
		sprintf_P(print_buff, PSTR("Temp: %2.2fC"), (double)t);
		lcd_screen_buff_update(LCD_ROW_1, 0, print_buff);
		// Print humidity
		sprintf_P(print_buff, PSTR("Humi: %2.2f%%"), (double)h);
		lcd_screen_buff_update(LCD_ROW_2, 0, print_buff);
	} else {
		// Print error
		sprintf_P(print_buff, PSTR("DHT-22 ERROR: %i"), error);
		lcd_screen_buff_update(LCD_ROW_1, 0, print_buff);
	}
}

void lcd_menu_print(char type) {
	lcd_buff_clear();
	char print_buff[LCD_MAX_POS]; // LCD buffer for sprintf
	switch(type) {
		case _MENU_MAIN:
			sprintf_P(print_buff, clock_str, (s_timer/3600), ((s_timer/60)%60), (s_timer%60));
			lcd_screen_buff_update(LCD_ROW_1, 4, print_buff);
			// TODO: print PROGMEM message type
			//lcd_screen_buff_update(LCD_ROW_2, 0, test_msg);
			break;
		case _MENU_SENSOR:
			lcd_print_sensor_data(sensor_data.t, sensor_data.h, sensor_data.error);
			break;
		case _MENU_SENSOR_MIN:
			lcd_print_sensor_data(min_t, min_h, 0);
			lcd_screen_buff_update(LCD_ROW_1, 13, "MIN");
			break;
		case _MENU_SENSOR_MAX:
			lcd_print_sensor_data(max_t, max_h, 0);
			lcd_screen_buff_update(LCD_ROW_1, 13, "MAX");
			break;
		default:
			break;
	}
}

void save_boundary_values(float *t, float *h) {
	// Save MIN temperature
	if (fmin(min_t, *t) != min_t) {
		cli();
		min_t = *t;
		eeprom_write_float((float*)0 , min_t);
		sei();
	}
	// Save MAX temperature
	if (fmax(max_t, *t) != max_t) {
		cli();
		max_t = *t;
		eeprom_write_float((float*)4 , max_t);
		sei();
	}
	// Save MIN humidity
	if (fmin(min_h, *h) != min_h) {
		cli();
		min_h = *h;
		eeprom_write_float((float*)8 , min_h);
		sei();
	}
	// Save MAX humidity
	if (fmax(max_h, *h) != max_h) {
		cli();
		max_h = *h;
		eeprom_write_float((float*)12 , max_h);
		sei();
	}
}

void read_sensor_data() {
	sensor_data.error = dht22_read(&sensor_data.t, &sensor_data.h);
	if (sensor_data.error == 0) save_boundary_values(&sensor_data.t, &sensor_data.h);
}

/* INTERRUPT HANDLERS */
ISR (INT4_vect) {
	send_message_w_param(MSG_BUTTON, _BUTTON_1);
	reset_timer(TIMER_BUTTON);
} // Button 1: show CURRENT temperature and humidity

ISR (INT5_vect) {
	send_message_w_param(MSG_BUTTON, _BUTTON_2);
	reset_timer(TIMER_BUTTON);
} // Button 2: show MIN temperature and humidity

ISR (INT6_vect) {
	send_message_w_param(MSG_BUTTON, _BUTTON_3);
	reset_timer(TIMER_BUTTON);
} // Button 3: show MAX temperature and humidity

ISR (INT7_vect) {
	send_message_w_param(MSG_BUTTON, _BUTTON_4);
} // Button 4: LED 2

/* TIMER INTERRUPT HANDLERS */
ISR (TIMER0_OVF_vect) {
	s_timer++;
	if (s_timer > 86400) s_timer = 0; // reset after 24 hours
} // Timer 0 overflow

ISR (TIMER1_COMPA_vect) {
	ms_timer++;		// !!! used for SW FSM timers (process_timers() function resets value to 0) !!!
	wdt_reset();	// reset Watchdog timer
} // Timer 1 compare A

/* INITIALIZATION */
inline void init_gpio() {
	// Peripherals
	DDRE = ~BUTTONS;			// buttons
	PORTE = BUTTONS;			// enable pull-up (logic 1 by default)
	DDRD = (1<<LED1)|(1<<LED2);	// output for LEDs
	DDRB &= ~(1<<DHT22);		// input for DHT22
	DDRB = (1<<MIC);	// output for LEDs
}

inline void init_interrupts_timers() {
	EIMSK = (1<<INT4)|(1<<INT5)|
			(1<<INT6)|(1<<INT7);				// enable interrupts INT4-7
	EICRB = (1<<ISC41)|(1<<ISC51)|
			(1<<ISC61)|(1<<ISC71);				// interrupt on button pressed down (logic 0 - falling edge)

	//TCNT0 = 0;								// timer 0 start value of counter (from 0 to 255) - 1 second (when reaches 255)
	ASSR = (1<<AS0);							// enable asynchronous clock from TOSC1 (32,768kHz)
	TIMSK = (1<<TOIE0);							// enable overflow interrupt (TIMER0_OVF_vect)
	TCCR0 = (0<<WGM01)|(0<<WGM00);				// timer 0 mode - Normal
	TCCR0 |= (1<<CS02)|(0<<CS01)|(1<<CS00);		// timer 0 pre-scaler (32.768 / 128 = 256 ticks per second)

	//TCNT1 = 0;								// timer 1 start value of counter (from 0 to 65535) - 1 second (when reaches 15.624)
	OCR1A = 250;								// timer 1 top value A for compare interrupt calling - 1 millisecond (15.625 / 1000 = 15)
	TIMSK |= (1<<OCIE1A);						// enable compare interrupt (TIMER1_COMPA_vect)
	TCCR1B = (0<<WGM13)|(1<<WGM12);				// timer 1 mode - CTC (OCRnA TOP)
	TCCR1B |= (0<<CS12)|(1<<CS11)|(1<<CS10);	// timer 1 (another mask <!>) pre-scaler (16.000.000 / 64 = 250,000 ticks per second)
/*
	0 0 0 No clock source. (Timer/Counter stopped)
	0 0 1 1 (No pre-scaling)
	0 1 0 8
	0 1 1 64
	1 0 0 256
	1 0 1 1024
	1 1 0 External clock source on Tn pin. Clock on falling edge
	1 1 1 External clock source on Tn pin. Clock on rising edge
*/
}

void init() {
	cli(); // disable interrupts

	/* 1 SEC WATCHDOG TIMEOUT */
	wdt_enable(WDTO_1S);

	/* LOAD DATA FROM EEPROM */
	min_t = eeprom_read_float((float*)0);
	max_t = eeprom_read_float((float*)4);
	min_h = eeprom_read_float((float*)8);
	max_h = eeprom_read_float((float*)12);

	/* SYSTEM INITIALIZATION */
	init_gpio();
	init_interrupts_timers();
	init_messages();
	lcd_init(LCD_DISP_ON);

	/* FSM INITIALIZATION */
	fsm_button_init(_BUTTON_OFF);
	fsm_menu_init(_MENU_MAIN);
	fsm_lcd_init(_LCD_ON);
	fsm_sensor_init(_SENSOR_READ);
	//fsm_led_init(_LED_ON);

	sei(); // enable interrupts
}

int main(void) {
	init();

	while(1) {
		// Process FSM timers
		process_timers(&ms_timer);

		/* RUN FSM TASKS */
		fsm_button();
		fsm_menu();
		fsm_lcd();
		fsm_sensor();
		//fsm_led();

		// Process FSM messages
		process_messages();
	}
}

