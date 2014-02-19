#include "timers.h"

long int timers[MAX_TIMERS];

#ifdef USE_GLOBAL_TIMERS
	#define TIMER_STOPPED		0
	#define TIMER_RUNNING		1
	#define TIMER_PAUSED		2

	long int g_timers[MAX_GTIMERS];
	char g_timer_states[MAX_GTIMERS];
#endif

/*
 * Even if main program cycle is longer than system timer ticks period all SW timers will
 * be updated correctly by adding value of ticks from HW system timer accessed by *tick
 * pointer. After all SW timers incremented system timer ticks are cleared.
 * It is easy to change clock division ratio for SW timers from HW timer (default is 1:1).
 */
void process_timers(unsigned long int *tick) {
	unsigned char i = 0;
	unsigned long int x = *tick;

	if (x > 0) {
		for (i = 0; i < MAX_TIMERS; i++) {
			timers[i] += x;
#ifdef USE_GLOBAL_TIMERS
			if (g_timer_states[i] == TIMER_RUNNING) {
				g_timers[i] += x;
			}
#endif
		}
		*tick = 0;
	}
}

void init_timers(void) {
	unsigned char i;
	for (i = 0; i < MAX_TIMERS; i++) {
		timers[i] = 0;
	}
}

int get_timer(unsigned char timer) {
	return timers[timer];
}

void reset_timer(unsigned char timer) {
	timers[timer] = 0;
}

#ifdef USE_GLOBAL_TIMERS
/// Global timers functions
int  get_g_timer(unsigned char timer){
	return g_timers[timer];
}

void stop_g_timer(unsigned char timer){
	g_timer_states[timer] = TIMER_STOPPED;
}
void start_g_timer(unsigned char timer){
	if (g_timer_states[timer] == TIMER_STOPPED) {
		g_timers[timer] = 0;
		g_timer_states[timer] = TIMER_RUNNING;
	}
}
void pause_g_timer(unsigned char timer) {
	if (g_timer_states[timer] == TIMER_RUNNING) {
		g_timer_states[timer] = TIMER_PAUSED;
	}
}
void continue_g_timer(unsigned char timer) {
	if (g_timer_states[timer] == TIMER_PAUSED) {
		g_timer_states[timer] = TIMER_RUNNING;
	}
}
#endif
