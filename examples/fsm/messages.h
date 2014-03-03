#ifndef MESSAGES_H_
#define MESSAGES_H_

/* FSM MESSAGES */
#define MSG_BUTTON				0
#define MSG_MENU				1
#define MSG_LCD					2
#define MSG_SENSOR				3
#define MSG_LED					4

/* CONFIG */
#define MAX_MESSAGES 			5

//#define USE_BROADCAST_MESSAGES
#ifdef USE_BROADCAST_MESSAGES
	#define MAX_BROADCAST_MESSAGES 	4

	#define B_MSG_1					0
	#define B_MSG_2					1
	#define B_MSG_3					2
	#define B_MSG_4					3
#endif

void init_messages(void);
void process_messages(void);
void send_message_w_param(char msg, char param);
void send_message_wo_param(char msg);
char get_message(char msg);
char get_message_param(char msg);

#ifdef USE_BROADCAST_MESSAGES
	void send_broadcast_message(char msg);
	char get_broadcast_message(char msg);
#endif

#endif /* MESSAGES_H_ */
