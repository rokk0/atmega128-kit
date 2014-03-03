#include "fsm.h"

char fsm_led_state;
char fsm_lcd_state;
char fsm_sensor_state;
char fsm_button_state;
char fsm_menu_state;

// Lock selected menu screen
char locked_menu = _MENU_MAIN;

char current_button = UNDEFINED;
char previous_button = UNDEFINED;
char button_double_click_flag = FALSE;	// to prevent long press action after double click
char button_long_press_flag = FALSE;	// to prevent double click after long press

// FSM button init
void fsm_button_init(char message) {
	fsm_button_state = _BUTTON_OFF;
	reset_timer(TIMER_BUTTON);
	send_message_w_param(MSG_BUTTON, message);
}

// FSM menu init
void fsm_menu_init(char message) {
	fsm_menu_state = _MENU_MAIN;
	reset_timer(TIMER_MENU);
	send_message_w_param(MSG_MENU, message);
}

// FSM LCD init
void fsm_lcd_init(char message) {
	fsm_lcd_state = _LCD_OFF;
	reset_timer(TIMER_LCD);
	send_message_w_param(MSG_LCD, message);
}

// FSM sensor init
void fsm_sensor_init(char message) {
	fsm_sensor_state = _SENSOR_IDLE;
	reset_timer(TIMER_SENSOR);
	send_message_w_param(MSG_SENSOR, message);
}

// FSM LED init
void fsm_led_init(char message) {
	fsm_led_state = _LED_OFF;
	reset_timer(TIMER_LED);
	send_message_w_param(MSG_LED, message);
}

// FSM button code
int get_button_pin() {
	switch(current_button) {
		case _BUTTON_1:
			return _BUTTON_1_PIN;
			break;

		case _BUTTON_2:
			return _BUTTON_2_PIN;
			break;

		case _BUTTON_3:
			return _BUTTON_3_PIN;
			break;

		case _BUTTON_4:
			return _BUTTON_4_PIN;
			break;

		default:
			return UNDEFINED;
			break;
	}
}

int is_button_pressed() {
	char button_pin = get_button_pin();

	if (button_pin != UNDEFINED) {
		char bit_mask = (PINE | ~(BUTTONS));
		return !(bit_mask & (1<<button_pin));
	} else {
		return 0;
	}
}

void fsm_button(void) {
	char fsm_message_state = UNDEFINED;
	if (get_message(MSG_BUTTON)) {
		fsm_message_state = get_message_param(MSG_BUTTON);
	}

	switch (fsm_button_state) {
		case _BUTTON_OFF:
			// TODO: check button state from the list (_BUTTON_1, _BUTTON_2, ..., _BUTTON_n)
			if (fsm_message_state != UNDEFINED && fsm_message_state != _BUTTON_OFF) {
				current_button = fsm_message_state; // set current button value
				fsm_button_state = _BUTTON_ON;
				reset_timer(TIMER_BUTTON);
				reset_timer(TIMER_BUTTON_DEBOUNCE);
			}
			break;

		case _BUTTON_DEBOUNCED:
			if (previous_button == current_button) {
				button_double_click_flag = TRUE;
				send_message_w_param(MSG_BUTTON, _BUTTON_DOUBLE_CLICKED);
			} else {
				reset_timer(TIMER_BUTTON_DOUBLE);	// start double click timer (300ms) after FIRST click
				reset_timer(TIMER_BUTTON_LONG);		// start long press timer (1000ms) after FIRST click
			}
			previous_button = current_button;
			fsm_button_state = current_button; // call current button action
			break;

		case _BUTTON_1:
			// Toggle MAIN/SENSOR menu
			fsm_menu_state = _MENU_MAIN;
			locked_menu = locked_menu ? _MENU_MAIN : _MENU_SENSOR;
			send_message_w_param(MSG_MENU, locked_menu);
			fsm_button_state = _BUTTON_OFF; // to prevent another actions
			break;

		case _BUTTON_2:
			// Show MIN SENSOR menu
			send_message_w_param(MSG_MENU, _MENU_SENSOR_MIN);
			fsm_button_state = _BUTTON_OFF; // to prevent another actions
			break;

		case _BUTTON_3:
			// Show MAX SENSOR menu
			send_message_w_param(MSG_MENU, _MENU_SENSOR_MAX);
			fsm_button_state = _BUTTON_OFF; // to prevent another actions
			break;

		case _BUTTON_4:
			if (fsm_message_state == _BUTTON_LONG_PRESSED) {
				// long press action
				MIC_TOGGLE;
			} else if (fsm_message_state == _BUTTON_DOUBLE_CLICKED) {
				// double click action
				LED1_TOGGLE;
			} else {
				// click action
				LED2_TOGGLE;
			}
			fsm_button_state = _BUTTON_OFF; // to prevent another actions
			break;

		default: break;
	}

	// Scan buttons every 50ms (detects long pressed button and resets button fsm to default state)
	if (get_timer(TIMER_BUTTON) >= BUTTON_CHECK_DELAY) {
		if (is_button_pressed()) {
			button_long_press_flag = TRUE;
		} else {
			button_long_press_flag = FALSE;
			fsm_button_state = _BUTTON_OFF;
		}
		reset_timer(TIMER_BUTTON);
	}

	// Check debounce only once, when button state is _BUTTON_ON (after the button interrupt)
	if (fsm_button_state == _BUTTON_ON) {
		// Check button PIN value after 25ms delay
		if (get_timer(TIMER_BUTTON_DEBOUNCE) >= BUTTON_DEBOUNCE_DELAY) {
			// PIN value 0 (pressed) after debounce delay
			if (is_button_pressed()) {
				// TODO: add variable flash_button check (handles only single click)
				fsm_button_state = _BUTTON_DEBOUNCED;
			}
			reset_timer(TIMER_BUTTON_DEBOUNCE);
		}
	}

	// Don't check double click if long press is in action
	if (button_long_press_flag == FALSE) {
		// Reset double click event after 300ms (counter starts after first click, not second!)
		if (get_timer(TIMER_BUTTON_DOUBLE) >= BUTTON_DOUBLE_CLICK_DELAY) {
			button_double_click_flag = FALSE;				// reset double click flag
			previous_button = current_button = UNDEFINED;	// reset button value
			reset_timer(TIMER_BUTTON_DOUBLE);
		}
	}

	// Check long press if it's only first click (to prevent long press action firing after second click)
	if (button_double_click_flag == FALSE) {
		// Send _BUTTON_LONG_PRESSED message to the button, that was pressed more than 1000ms (counter starts after first click, not second!)
		if (get_timer(TIMER_BUTTON_LONG) >= BUTTON_LONG_PRESS_DELAY) {
			fsm_button_state = current_button;						// call current button action
			button_long_press_flag = FALSE;							// reset long press flag
			previous_button = current_button = UNDEFINED;			// reset button value
			send_message_w_param(MSG_BUTTON, _BUTTON_LONG_PRESSED); // send long press message
			reset_timer(TIMER_BUTTON_LONG);
		}
	}
}

// Interrupt current menu with the new one
void interrupt_menu(char new_menu) {
	if (new_menu != UNDEFINED) {
		fsm_menu_state = locked_menu = _MENU_MAIN; // reset menu lock
		reset_timer(TIMER_MENU);
		send_message_w_param(MSG_MENU, new_menu);
	}
}

// FSM menu code
void fsm_menu(void) {
	char fsm_message_state = UNDEFINED;
	if (get_message(MSG_MENU)) {
		fsm_message_state = get_message_param(MSG_MENU);
	}
	switch (fsm_menu_state) {
		case _MENU_MAIN:
			if (fsm_message_state != UNDEFINED) {
				// Print new menu
				fsm_menu_state = fsm_message_state;
				lcd_menu_print(fsm_menu_state);
				reset_timer(TIMER_MENU);
			} else {
				// Refresh main menu
				lcd_menu_print(fsm_menu_state);
			}
			break;

		default:
			if (fsm_message_state == _MENU_MAIN) {
				fsm_menu_state = _MENU_MAIN;
			} else if (fsm_message_state != fsm_menu_state) {
				interrupt_menu(fsm_message_state);
			}
			break;
	}

	if (locked_menu == _MENU_MAIN) {
		// Return to main menu after 3s.
		if ((get_timer(TIMER_MENU) >= MENU_SHOW_DELAY)) {
			fsm_menu_state = _MENU_MAIN;
			reset_timer(TIMER_MENU);
		}
	} else {
		// Refresh locked menu data every 1s.
		if ((get_timer(TIMER_MENU) >= MENU_REFRESH_DELAY)) {
			lcd_menu_print(fsm_menu_state);
			reset_timer(TIMER_MENU);
		}
	}
}

// FSM LCD code
void fsm_lcd(void) {
	char fsm_message_state = UNDEFINED;
	if (get_message(MSG_LCD)) {
		fsm_message_state = get_message_param(MSG_LCD);
	}

	switch (fsm_lcd_state) {
		case _LCD_OFF:
			if (fsm_message_state == _LCD_ON) {
				lcd_command(LCD_DISP_ON);
				fsm_lcd_state = _LCD_REFRESH;
			}
			break;

		case _LCD_ON:
			switch (fsm_message_state) {
				case _LCD_OFF:
					lcd_command(LCD_DISP_OFF);
					fsm_lcd_state = _LCD_OFF;
					break;

				case _LCD_REFRESH: { fsm_lcd_state = _LCD_REFRESH; } break;

				case _LCD_CLEAR: { fsm_lcd_state = _LCD_CLEAR; } break;

				default: break;
			}
			if (get_timer(TIMER_LCD) >= LCD_REFRESH_DELAY) {
				fsm_lcd_state = _LCD_REFRESH;
				reset_timer(TIMER_LCD);
			}
			break;

		case _LCD_REFRESH:
			fsm_lcd_state = _LCD_ON;
			reset_timer(TIMER_LCD);
			lcd_screen_refresh(0);
			break;

		case _LCD_CLEAR:
			fsm_lcd_state = _LCD_ON;
			reset_timer(TIMER_LCD);
			lcd_screen_refresh(1);
			break;

		default: break;
	}
}

// FSM sensor code
void fsm_sensor(void) {
	char fsm_message_state = UNDEFINED;
	if (get_message(MSG_SENSOR)) {
		fsm_message_state = get_message_param(MSG_SENSOR);
	}

	switch (fsm_sensor_state) {
		case _SENSOR_IDLE:
			if (fsm_message_state == _SENSOR_READ) {
				fsm_sensor_state = _SENSOR_READ;
				read_sensor_data();
			}
			break;

		case _SENSOR_READ:
			if (fsm_message_state == _SENSOR_IDLE) {
				fsm_sensor_state = _SENSOR_IDLE;
			} else if (get_timer(TIMER_SENSOR) >= SENSOR_READ_DELAY) {
				read_sensor_data();
				reset_timer(TIMER_SENSOR);
			}
			break;

		default: break;
	}
}

// FSM LED code
void fsm_led(void) {
	char fsm_message_state = UNDEFINED;
	if (get_message(MSG_LED)) {
		fsm_message_state = get_message_param(MSG_LED);
	}

	switch (fsm_led_state) {
		case _LED_OFF:
			if (fsm_message_state == _LED_ON) {
				fsm_led_state = _LED_ON;
				LED1_ON;
			} else {
				LED1_OFF;
			}
			break;

		case _LED_ON:
			if (fsm_message_state == _LED_OFF) {
				fsm_led_state = _LED_OFF;
				LED1_OFF;
			} else if (get_timer(TIMER_LED) >= LED_BLINK_DELAY) {
				LED1_TOGGLE;
				reset_timer(TIMER_LED);
			}
			break;

		default: break;
	}
}