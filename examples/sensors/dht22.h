#ifndef DHT22_H
#define DHT22_H

#include <inttypes.h>

#define THERM_PIN PINB
#define THERM_DDR DDRB
#define THERM_PORT PORTB

#define THERM_DQ PB0
/* Utils */
#define THERM_INPUT_MODE() THERM_DDR&=~(1<<THERM_DQ)
#define THERM_OUTPUT_MODE() THERM_DDR|=(1<<THERM_DQ)
#define THERM_LOW() THERM_PORT&=~(1<<THERM_DQ)
#define THERM_HIGH() THERM_PORT|=(1<<THERM_DQ)
#define THERM_READ() ((THERM_PIN&(1<<THERM_DQ))? 1 : 0)

typedef enum
{
	DHT_ERROR_NONE = 0,
	DHT_BUS_HUNG = 1,
	DHT_ERROR_NOT_PRESENT = 2,
	DHT_ERROR_ACK_TOO_LONG = 3,
	DHT_ERROR_SYNC_TIMEOUT = 4,
	DHT_ERROR_DATA_TIMEOUT = 5,
	DHT_ERROR_CHECKSUM = 6,
	DHT_ERROR_TOOQUICK = 7
} DHT22_ERROR_t;


int dht22_read(float *temperature, float *humidity);
char get_error_name(int error_code);



#endif /*DHT22_H*/
