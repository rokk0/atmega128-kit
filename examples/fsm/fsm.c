#include "fsm.h"

char fsm_led_state;
char fsm_lcd_state;
char fsm_sensor_state;
char fsm_button_state;
char fsm_menu_state;

// Lock selected menu screen
char fsm_menu_lock;

// FSM LED init
void fsm_led_init(char message) {
	fsm_led_state = _LED_OFF;
	reset_timer(TIMER_LED);
	send_message_w_param(MSG_LED, message);
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

// FSM button init
void fsm_button_init(char message) {
	fsm_button_state = _BUTTON_OFF;
	reset_timer(TIMER_BUTTON);
	send_message_w_param(MSG_BUTTON, message);
}

// FSM menu init
void fsm_menu_init(char message) {
	fsm_menu_state = fsm_menu_lock = _MENU_MAIN;
	reset_timer(TIMER_MENU);
	send_message_w_param(MSG_MENU, message);
}

// FSM LED code
void fsm_led(void) {
	char fsm_message_state = 0xff;
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

		default:
			break;
	}
}

// FSM LCD code
void fsm_lcd(void) {
	char fsm_message_state = 0xff;
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

				default: {} break;
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

		default:
			break;
	}
}

// FSM sensor code
void fsm_sensor(void) {
	char fsm_message_state = 0xff;
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

		default:
			break;
	}
}

// FSM button code
void fsm_button(void) {
	char fsm_message_state = 0xff;
	if (get_message(MSG_BUTTON)) {
		fsm_message_state = get_message_param(MSG_BUTTON);
	}

	switch (fsm_button_state) {
		case _BUTTON_OFF:
			if (fsm_message_state == _BUTTON_1) {
				fsm_button_state = _BUTTON_1;
				// Toggle MAIN/SENSOR menu lock state
				fsm_menu_state = _MENU_MAIN;
				fsm_menu_lock = fsm_menu_lock ? _MENU_MAIN : _MENU_SENSOR;
				send_message_w_param(MSG_MENU, fsm_menu_lock);
				reset_timer(TIMER_MENU);
			} else if (fsm_message_state == _BUTTON_2) {
				fsm_button_state = _BUTTON_2;
				send_message_w_param(MSG_MENU, _MENU_SENSOR_MIN);
				reset_timer(TIMER_MENU);
			} else if (fsm_message_state == _BUTTON_3) {
				fsm_button_state = _BUTTON_3;
				send_message_w_param(MSG_MENU, _MENU_SENSOR_MAX);
				reset_timer(TIMER_MENU);
			} else if (fsm_message_state == _BUTTON_4) {
				fsm_button_state = _BUTTON_4;
				LED2_TOGGLE;
			} else { }
			break;

		case _BUTTON_1:
			break;

		case _BUTTON_2:
			break;

		case _BUTTON_3:
			break;

		case _BUTTON_4:
			break;

		default:
			break;
	}

	if (get_timer(TIMER_BUTTON) >= BUTTON_CHECK_DELAY) {
		fsm_button_state = _BUTTON_OFF;
		reset_timer(TIMER_BUTTON);
	}
}

// Interrupt current menu with the new one
void interrupt_menu(char new_menu) {
	if (new_menu != 0xff) {
		fsm_menu_state = fsm_menu_lock = _MENU_MAIN;
		reset_timer(TIMER_MENU);
		send_message_w_param(MSG_MENU, new_menu);
	}
}

// FSM menu code
void fsm_menu(void) {
	char fsm_message_state = 0xff;
	if (get_message(MSG_MENU)) {
		fsm_message_state = get_message_param(MSG_MENU);
	}
	switch (fsm_menu_state) {
		case _MENU_MAIN:
			if (fsm_message_state != 0xff) {
				// Print new menu
				fsm_menu_state = fsm_message_state;
				lcd_menu_print(fsm_menu_state);
			} else {
				// Refresh main menu
				lcd_menu_print(fsm_menu_state);
			}
			break;
	/*		
			if (fsm_message_state == _MENU_SENSOR) {
				fsm_menu_state = _MENU_SENSOR;
				lcd_menu_print(fsm_menu_state);
			} else if (fsm_message_state == _MENU_SENSOR_MIN) {
				fsm_menu_state = _MENU_SENSOR_MIN;
				lcd_menu_print(fsm_menu_state);
			} else if (fsm_message_state == _MENU_SENSOR_MAX) {
				fsm_menu_state = _MENU_SENSOR_MAX;
				lcd_menu_print(fsm_menu_state);
			} else {
				lcd_menu_print(fsm_menu_state);
			}
	*/


		case _MENU_SENSOR:
			if (fsm_message_state == _MENU_MAIN) {
				fsm_menu_state = _MENU_MAIN;
			} else if (fsm_message_state != fsm_menu_state) {
				interrupt_menu(fsm_message_state);
			}
			break;

		case _MENU_SENSOR_MIN:
			if (fsm_message_state == _MENU_MAIN) {
				fsm_menu_state = _MENU_MAIN;
			} else if (fsm_message_state != fsm_menu_state) {
				interrupt_menu(fsm_message_state);
			}
			break;

		case _MENU_SENSOR_MAX:
			if (fsm_message_state == _MENU_MAIN) {
				fsm_menu_state = _MENU_MAIN;
			} else if (fsm_message_state != fsm_menu_state) {
				interrupt_menu(fsm_message_state);
			}
			break;

		default:
			break;
	}

	if (fsm_menu_lock > 0) {
		// Refresh menu data every 1s.
		if ((get_timer(TIMER_MENU) >= MENU_REFRESH_DELAY)) {
			lcd_menu_print(fsm_menu_state);
			reset_timer(TIMER_MENU);
		}
	} else {
		// Return to main menu after 3s.
		if ((get_timer(TIMER_MENU) >= MENU_SHOW_DELAY)) {
			fsm_menu_state = _MENU_MAIN;
			reset_timer(TIMER_MENU);
		}
	}
	
}

