#ifndef FSM_H_
#define FSM_H_

#include "../main.h"

/* FSM STATES */
#define STATE_DEFAULT		0

// LED
#define _LED_OFF			0
#define _LED_ON				1

// LCD
#define _LCD_OFF			0
#define _LCD_ON				1
#define _LCD_REFRESH		2
#define _LCD_CLEAR			3

// SENSOR
#define _SENSOR_IDLE		0
#define _SENSOR_READ		1

// BUTTON
#define _BUTTON_OFF			0
#define _BUTTON_1			1
#define _BUTTON_2			2
#define _BUTTON_3			3
#define _BUTTON_4			4

// MENU
#define _MENU_MAIN			0
#define _MENU_SENSOR		1
#define _MENU_SENSOR_MIN	2
#define _MENU_SENSOR_MAX	3

/* FSM STATE VARIABLES */
extern char fsm_led_state;
extern char fsm_lcd_state;
extern char fsm_sensor_state;
extern char fsm_button_state;
extern char fsm_menu_state;

/* FSM STATE FUNCTIONS */
// init
void fsm_led_init(char message);
void fsm_lcd_init(char message);
void fsm_sensor_init(char message);
void fsm_button_init(char message);
void fsm_menu_init(char message);

// process
void fsm_led(void);
void fsm_lcd(void);
void fsm_sensor(void);
void fsm_button(void);
void fsm_menu(void);

#endif /* FSM_H_ */
