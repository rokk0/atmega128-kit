#include "messages.h"

typedef struct {
	char msg;
	char param;
} MSG_DATA;

MSG_DATA messages[MAX_MESSAGES];

#ifdef USE_BROADCAST_MESSAGES
char broadcast_messages[MAX_BROADCAST_MESSAGES];
#endif

void init_messages(void) {
	char i;
	for (i = 0; i < MAX_MESSAGES; i++) {
		messages[i].msg = 0;
	}
#ifdef USE_BROADCAST_MESSAGES
	for (i = 0; i < MAX_BROADCAST_MESSAGES; i++) {
		broadcast_messages[i] = 0;
	}
#endif
}

void send_message_w_param(char msg, char param) {
	if (messages[msg].msg == 0) {
		messages[msg].msg = 1;
		messages[msg].param = param;
	}
}

void send_message_wo_param(char msg) {
	if (messages[msg].msg == 0) {
		messages[msg].msg = 1;
	}
}

char get_message(char msg) {
	if (messages[msg].msg == 2) {
		messages[msg].msg = 0;
		return 1;
	}
	return 0;
}

char get_message_param(char msg) {
	return messages[msg].param;
}

#ifdef USE_BROADCAST_MESSAGES
void send_broadcast_message(char msg) {
	if (broadcast_messages[msg] == 0) {
		broadcast_messages[msg] = 1;
	}
}

char get_broadcast_message(char msg) {
	if (broadcast_messages[msg] == 2) {
		return 1;
	}
	return 0;
}
#endif

void process_messages(void) {
	char i;
	for (i = 0; i < MAX_MESSAGES; i++) {
		if (messages[i].msg == 2) messages[i].msg = 0;
		if (messages[i].msg == 1) messages[i].msg = 2;
	}
#ifdef USE_BROADCAST_MESSAGES
	for (i = 0; i < MAX_BROADCAST_MESSAGES; i++) {
		if (broadcast_messages[i] == 2) broadcast_messages[i] = 0;
		if (broadcast_messages[i] == 1) broadcast_messages[i] = 2;
	}
#endif
}
