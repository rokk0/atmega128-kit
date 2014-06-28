#include "main.h"

u8g_t u8g;

const char cpu_freq_str_P[] PROGMEM = "CPU Frequency: ";
const char cpu_freq_val_str_P[] PROGMEM = "16 MHz";
const char pwm_top_str_P[] PROGMEM = "PWM (TOP + 1): ";
const char pwm_freq_str_P[] PROGMEM = "PWM Frequency: ";

// Fast PWM: fOCnxPWM = fclk / ( pre-scaler * (1 + TOP) )
// 16 000 000 / ( 1 * (1 + 255) ) = 62 500 Hz (1 period of PWM)
// Phace Correct PWM: fOCnxPCPWM = fclk / ( 2 * pre-scaler * TOP )
// 16 000 000 / ( 2 * 1 * 255) ) = 31 372 Hz (1 period of PWM)
// 16 000 000 / 32 000 = 500 Hz
//uint32_t pwm_counter_top = 32000;

// PWM init values
#define PWM_PIN_CNT 3

uint8_t pwm_menu_current_index = 0;

uint8_t pwm_pin_array[PWM_PIN_CNT] = { 5, 6, 7 };
uint8_t pwm_duty_array[PWM_PIN_CNT] = { 0, 0, 0 };

uint8_t pwm_menu_pin;
uint8_t pwm_menu_duty;

unsigned int pwm_duty = 0;

uint32_t pwm_counter_top = 31999;
uint32_t tmp_opt_menu_pwm_top = 32000;
uint32_t ten_percents = 3199;
char tmp_menu_pwm_freq_label[9] = "500 Hz";

// PWM chart
#define PWM_CHART_HEIGHT 10

uint8_t pwm_zoom_idx = 1;
uint8_t pwm_cycle_width = 32;	// pixels
uint8_t pwm_samples_count = 4;	// (128 / pwm_cycle_width)

uint8_t chart_samples[PWM_PIN_CNT][8];

// forward declaration of the root menu
M2_EXTERN_ALIGN(top_el_root_menu);

void pwm_duty_index_to_str(uint8_t value, char *dest) {
	switch(value) {
		case 0: strcpy(dest, "low"); break;
		case 1: strcpy(dest, "10%"); break;
		case 2: strcpy(dest, "20%"); break;
		case 3: strcpy(dest, "30%"); break;
		case 4: strcpy(dest, "40%"); break;
		case 5: strcpy(dest, "50%"); break;
		case 6: strcpy(dest, "60%"); break;
		case 7: strcpy(dest, "70%"); break;
		case 8: strcpy(dest, "80%"); break;
		case 9: strcpy(dest, "90%"); break;
		case 10: strcpy(dest, "high"); break;
	}
}

unsigned int pwm_duty_index_to_analog(uint8_t index) {
	switch(index) {
		case 0: return 0;
		case 1: return ten_percents;
		case 2: return ten_percents * 2;
		case 3: return ten_percents * 3;
		case 4: return ten_percents * 4;
		case 5: return ten_percents * 5;
		case 6: return ten_percents * 6;
		case 7: return ten_percents * 7;
		case 8: return ten_percents * 8;
		case 9: return ten_percents * 9;
	}
	return pwm_counter_top;
}

void pwm_save_duty(uint8_t pwm_pin, uint8_t pwm_menu_duty) {
	pwm_duty = pwm_duty_index_to_analog(pwm_menu_duty);
	switch (pwm_pin) {
		case 5: OCR1A = pwm_duty; break;
		case 6: OCR1B = pwm_duty; break;
		case 7: OCR1C = pwm_duty; break;
	}
}

// GENERAL OPTIONS MENU
/******************************************************************************/
void pwm_zoom_index_to_str(uint8_t value, char *dest) {
	switch(value) {
		case 0: strcpy(dest, " X1"); break;
		case 1: strcpy(dest, " X2"); break;
		case 2: strcpy(dest, " X4"); break;
		case 3: strcpy(dest, " X8"); break;
	}
}

uint8_t pwm_zoom_index_to_pixels(uint8_t index) {
	switch(index) {
		case 0: return 16;
		case 1: return 32;
		case 2: return 64;
	}
	return 128;
}

void change_pwm_freq() {
	itoa((int)(16000000 / tmp_opt_menu_pwm_top), tmp_menu_pwm_freq_label, 10);
	strcat(tmp_menu_pwm_freq_label, " Hz");
}

// this procedure is called by the "ok" button
void opt_fn_ok(m2_el_fnarg_p fnarg) {
	// save user entry	
	pwm_counter_top = tmp_opt_menu_pwm_top - 1;
	ten_percents = pwm_counter_top / 10;
	ICR1 = pwm_counter_top;
	pwm_save_duty(5, pwm_duty_array[0]);
	pwm_save_duty(6, pwm_duty_array[1]);
	pwm_save_duty(7, pwm_duty_array[2]);

	// go back to parent menu
	m2_SetRoot(&top_el_root_menu);
}

// this procedure is called by the "ok" button
void opt_fn_cancel(m2_el_fnarg_p fnarg) {
	// reset menu value
	tmp_opt_menu_pwm_top = pwm_counter_top + 1;
	change_pwm_freq();

	// go back to parent menu
	m2_SetRoot(&top_el_root_menu);
}

void pwm_top_inc(m2_el_fnarg_p fnarg) {
	if (tmp_opt_menu_pwm_top < 99000) tmp_opt_menu_pwm_top += 1000;
	change_pwm_freq();
}

void pwm_top_dec(m2_el_fnarg_p fnarg) {
	if (tmp_opt_menu_pwm_top > 10000) tmp_opt_menu_pwm_top -= 1000;
	change_pwm_freq();
}

M2_LABELP(el_opt_clck_label, NULL, cpu_freq_str_P);
M2_LABELP(el_opt_clck_val_label, NULL, cpu_freq_val_str_P);

M2_LABELP(el_opt_pwm_top_label, NULL, pwm_top_str_P);
M2_U32NUM(el_opt_pwm_top, "r1c5x5y1", &tmp_opt_menu_pwm_top);
M2_BUTTON(el_opt_pwm_top_dec, "x0y1", "-", pwm_top_dec);
M2_BUTTON(el_opt_pwm_top_inc, "x30y1", "+", pwm_top_inc);
M2_LIST(pwm_top_list) = { &el_opt_pwm_top, &el_opt_pwm_top_dec, &el_opt_pwm_top_inc };
M2_XYLIST(el_pwm_top_list, NULL, pwm_top_list);

M2_LABELP(el_opt_pwm_freq_label, NULL, pwm_freq_str_P);
M2_LABEL(el_opt_pwm_freq_val_label, NULL, tmp_menu_pwm_freq_label);

// cancel: do not store user values, go back directly
M2_BUTTON(el_opt_cancel, "f4", "Cancel", opt_fn_cancel);
// ok: write user values back to the array and apply values to the hardware
M2_BUTTON(el_opt_ok, "f4", "Ok", opt_fn_ok);

// following grid-list will arrange the elements on the display
M2_LIST(list_opt_menu) = {
	&el_opt_clck_label, &el_opt_clck_val_label,
	&el_opt_pwm_top_label, &el_pwm_top_list,
	&el_opt_pwm_freq_label, &el_opt_pwm_freq_val_label,
	&el_opt_cancel, &el_opt_ok
};
M2_GRIDLIST(el_opt_grid, "c2", list_opt_menu);

// center the menu on the display
M2_ALIGN(top_el_opt_menu, "-1|1W64H64", &el_opt_grid);
/******************************************************************************/

// PWM SETTINGS MENU
/******************************************************************************/
void pwm_fn_ok(m2_el_fnarg_p fnarg) {
	pwm_duty_array[pwm_menu_current_index] = pwm_menu_duty;
	pwm_save_duty(pwm_menu_pin, pwm_menu_duty);

	m2_SetRoot(&top_el_root_menu);
}

const char *pwm_fn_duty(uint8_t idx) {
	static char buf[8];
	pwm_duty_index_to_str(idx, buf);
	return buf;
}

const char *xmenu_pwm_submenu(uint8_t idx, uint8_t msg) {
	if (msg == M2_STRLIST_MSG_SELECT) {
		pwm_menu_current_index = idx - 2; // idx - position_in_xmenu_entry
		pwm_menu_pin = pwm_pin_array[pwm_menu_current_index];
		pwm_menu_duty = pwm_duty_array[pwm_menu_current_index];
	}
	return "";
}

M2_LABEL(el_pwm_pin_label, NULL, "Pin: ");
M2_U8NUM(el_pwm_pin_num, "r1c2", 0, 255, &pwm_menu_pin);

M2_LABEL(el_pwm_duty_label, NULL, "Duty: ");
M2_COMBO(el_pwm_duty, NULL, &pwm_menu_duty, 11, pwm_fn_duty);

M2_ROOT(el_pwm_cancel, "f4", "Cancel", &top_el_root_menu);
M2_BUTTON(el_pwm_ok, "f4", "Ok", pwm_fn_ok);

M2_LIST(list_pwm_menu) = {
	&el_pwm_pin_label, &el_pwm_pin_num,
	&el_pwm_duty_label, &el_pwm_duty,
	&el_pwm_cancel, &el_pwm_ok
};
M2_GRIDLIST(el_pwm_grid, "c2", list_pwm_menu);

M2_ALIGN(top_el_pwm_menu, "-1|1W64H64", &el_pwm_grid);
/******************************************************************************/

// define callback procedure, which returns a menu entry with a value
const char *xmenu_value(uint8_t idx, uint8_t msg) {
	if (msg == M2_STRLIST_MSG_GET_STR) {
		static char buf[4];
		pwm_cycle_width = pwm_zoom_index_to_pixels(pwm_zoom_idx);
		pwm_samples_count = 128 / pwm_cycle_width;
		pwm_zoom_index_to_str(pwm_zoom_idx, buf);
		return buf;
	}
	return "";
}

// define callback procedures which increment and decrement a value
const char *xmenu_inc(uint8_t idx, uint8_t msg) {
	if (msg == M2_STRLIST_MSG_SELECT && pwm_zoom_idx < 3) pwm_zoom_idx++;
	return "";
}

const char *xmenu_dec(uint8_t idx, uint8_t msg) {
	if (msg == M2_STRLIST_MSG_SELECT && pwm_zoom_idx > 0) pwm_zoom_idx--;
	return "";
}

// this is the overall menu structure for the menu */
m2_xmenu_entry xmenu_data[] =
{
	{ "Options", &top_el_opt_menu, NULL },
	{ "PWM settings", NULL, NULL },
	{ ". PB5", &top_el_pwm_menu, xmenu_pwm_submenu },
	{ ". PB6", &top_el_pwm_menu, xmenu_pwm_submenu },
	{ ". PB7", &top_el_pwm_menu, xmenu_pwm_submenu },
	{ "Zoom", NULL, NULL },
	{ ".", NULL, xmenu_value },
	{ ". +", NULL, xmenu_inc },
	{ ". -", NULL, xmenu_dec },
	{ NULL, NULL, NULL },
};

uint8_t el_x2l_first = 0;
uint8_t el_x2l_cnt = 4;

M2_X2LMENU(el_x2l_strlist, "l4e15W50", &el_x2l_first, &el_x2l_cnt, xmenu_data, '+','-','\0');
M2_SPACE(el_x2l_space, "W1h1");
M2_VSB(el_x2l_vsb, "l4W2r1", &el_x2l_first, &el_x2l_cnt);
M2_LIST(list_x2l) = { &el_x2l_strlist, &el_x2l_space, &el_x2l_vsb };
M2_HLIST(el_x2l_hlist, NULL, list_x2l);
M2_ALIGN(top_el_root_menu, "-1|2W64H64", &el_x2l_hlist);

long map(long x, long in_min, long in_max, long out_min, long out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void draw_pwm(uint8_t x, uint8_t pos, uint8_t high_width, uint8_t low_width) {
	uint8_t y = pos + 20 + pos * 10;
	if (x != 0 ) u8g_DrawVLine(&u8g, (x * pwm_cycle_width), y, PWM_CHART_HEIGHT);					// FRONT
	u8g_DrawHLine(&u8g, (x * pwm_cycle_width), y, high_width);										// HIGH
	u8g_DrawVLine(&u8g, high_width + (x * pwm_cycle_width), y, PWM_CHART_HEIGHT);					// FRONT
	u8g_DrawHLine(&u8g, high_width + (x * pwm_cycle_width), y + PWM_CHART_HEIGHT - 1, low_width);	// LOW
}

void draw_pwm_chart(unsigned int pwm_duty, uint8_t y) {
	uint8_t high_width = map(pwm_duty, 0, pwm_counter_top, 0, pwm_cycle_width);
	uint8_t low_width = pwm_cycle_width - high_width;
	for (uint8_t x = 0; x < pwm_samples_count; x++) {
		draw_pwm(x, y, high_width, low_width);
	}
}

// high level draw and update procedures
void draw_graphics(void) {
	if (m2_GetRoot() == &top_el_root_menu) {
		draw_pwm_chart(OCR1A, 1);
		draw_pwm_chart(OCR1B, 2);
		draw_pwm_chart(OCR1C, 3);
	}
}

// update graphics, will return none-zero if an update is required
uint8_t update_graphics(void) {
	// update PWM chart
	//if (m2_GetRoot() == &top_el_root_menu && redraw_chart == TRUE) {
	//	return 1;
	//}
	// no update for the graphics required
	return 0;
}

// draw procedure of the u8g picture loop
void draw(void) {
	// call the m2 draw procedure
	m2_Draw();
	// call the additional u8 draw methods
	draw_graphics();
}

void setup_u8g_m2(void) {
	// 1. Setup and create u8g device: http://code.google.com/p/u8glib/wiki/avr#U8glib_Init
	u8g_Init8Bit(&u8g, &u8g_dev_ks0108_128x64, PN(0, 0), PN(0, 1), PN(0, 2), PN(0, 3), PN(0, 4), PN(0, 5), PN(0, 6), PN(0, 7), PN(2, 5), PN(2, 7), PN(2, 6), PN(6, 1), PN(6, 0), U8G_PIN_NONE);

	// 2. Setup m2
	m2_Init(&top_el_root_menu, m2_es_avr_u8g, m2_eh_6bs, m2_gh_u8g_bfs);
	//m2_Init(&top_el_root_menu, m2_es_avr_rotary_encoder_u8g, m2_eh_4bd, m2_gh_u8g_bfs);

	// 3. Connect u8g display to m2
	m2_SetU8g(&u8g, m2_u8g_box_icon);

	// 4. Set a font, use normal u8g_font's
	m2_SetFont(0, (const void *)u8g_font_5x8r);

	// 5. Define keys
	m2_SetPin(M2_KEY_DATA_DOWN, BTN1);
	m2_SetPin(M2_KEY_SELECT, BTN2);
	m2_SetPin(M2_KEY_NEXT, BTN4);
	m2_SetPin(M2_KEY_PREV, BTN3);
}

void init_sys(void) {
	DDRB = (1<<PB5)|(1<<PB6)|(1<<PB7);
	DDRD = (1<<LED1)|(1<<LED2);
}

inline void init_timers() {
	// TIMER COUNTERS (65535 - max value for 16-bit timer)
	TCNT1 = 0;				// BOTTOM value
	ICR1 = pwm_counter_top;	// TOP value
	OCR1A = pwm_duty;		// COMPARE interrupt value
	OCR1B = pwm_duty;
	OCR1C = pwm_duty;

	TIMSK = (1<<TOIE1)|(1<<OCIE1A)|(1<<OCIE1B);	// enable compare interrupt (TIMER1_COMPA_vect)
	TCCR1A = (1<<WGM11)|(0<<WGM10);				// Fast PWM
	TCCR1B = (1<<WGM13)|(1<<WGM12);				// TOP = ICRn, Update of OCR1A at BOTTOM
	TCCR1A |= (1<<COM1A1)|(0<<COM1A0);			// Clear OCnA/OCnB/OCnC on compare match, set at BOTTOM, (non-inverting mode)
	TCCR1A |= (1<<COM1B1)|(0<<COM1B0);
	TCCR1A |= (1<<COM1C1)|(0<<COM1C0);
	TCCR1B |= (0<<CS12)|(0<<CS11)|(1<<CS10);	// no pre-scaler (16MHz)
	//TCCR1B |= (1<<CS12)|(0<<CS11)|(0<<CS10);	// pre-scaler (16.000.000 / 256 = 62,500 ticks per second (16us tick))
	//TCCR1B |= (0<<CS12)|(1<<CS11)|(1<<CS10);	// pre-scaler (16.000.000 / 64 = 250,000 ticks per second (4us tick))
}

ISR (TIMER1_OVF_vect) {
	if (OCR1A != 0) LED1_ON;
	if (OCR1B != 0) LED2_ON;
}

ISR (TIMER1_COMPA_vect) {
	if (OCR1A != pwm_counter_top) LED1_OFF;
}

ISR (TIMER1_COMPB_vect) {
	if (OCR1B != pwm_counter_top) LED2_OFF;
}

int main(void) {
	init_sys();
	setup_u8g_m2(); // setup u8g and m2 libraries
	init_timers();
	sei();

	for(;;) {
		m2_CheckKey();
		if (m2_HandleKey() || update_graphics()) {
			// picture loop
			u8g_FirstPage(&u8g);
			do {
				draw();
				m2_CheckKey();
			} while(u8g_NextPage(&u8g));
		}
	}
}
