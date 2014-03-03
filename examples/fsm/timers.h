#ifndef TIMERS_H_
#define TIMERS_H_

/* CONSTANTS */
#define SEC 			1000 // Timer1 tick is 1ms
#define MIN 			60 * sec
#define HOUR 			60 * min

/* FSM TIMERS */
#define TIMER_BUTTON			0
#define TIMER_BUTTON_DEBOUNCE 	1
#define TIMER_BUTTON_DOUBLE		2
#define TIMER_BUTTON_LONG		3
#define TIMER_MENU	 			4
#define TIMER_LCD 				5
#define TIMER_SENSOR 			6
#define TIMER_LED 				7

/* CONFIG */
#define MAX_TIMERS 				8

//#define USE_GLOBAL_TIMERS
#ifdef USE_GLOBAL_TIMERS
	#define MAX_GTIMERS 	4

	#define GTIMER_1		0
	#define GTIMER_2		1
	#define GTIMER_3		2
	#define GTIMER_4		3
#endif

void process_timers(unsigned long int *tick);
void init_timers(void);
int  get_timer(unsigned char timer);
void reset_timer(unsigned char timer);

#ifdef USE_GLOBAL_TIMERS
	int  get_g_timer(unsigned char timer);
	void stop_g_timer(unsigned char timer);
	void start_g_timer(unsigned char timer);
	void pause_g_timer(unsigned char timer);
	void continue_g_timer(unsigned char timer);
#endif

#endif /* TIMERS_H_ */
