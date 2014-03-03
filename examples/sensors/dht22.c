#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>
#include "dht22.h"

// This should be 40, but the sensor is adding an extra bit at the start
#define DHT22_DATA_BIT_COUNT 41


//
// Read the 40 bit data stream from the DHT 22
// Store the results in private member data to be read by public member functions
//
int dht22_read(float *temperature, float *humidity)
{
 // uint8_t bitmask = _bitmask;
  //volatile uint8_t *reg asm("r30") = _baseReg;
	uint8_t retryCount;
	uint8_t bitTimes[DHT22_DATA_BIT_COUNT];
	int currentHumidity;
	int currentTemperature;
	uint8_t checkSum, csPart1, csPart2, csPart3, csPart4;
	
	int i;

	currentHumidity = 0;
	currentTemperature = 0;
	checkSum = 0;
	
	//return currentTime;
	for(i = 0; i < DHT22_DATA_BIT_COUNT; i++)
	{
		bitTimes[i] = 0;
	}

	 // Pin needs to start HIGH, wait until it is HIGH with a timeout
	cli();
	THERM_INPUT_MODE();
	sei();
	retryCount = 0;
	do
	{
		if (retryCount > 125)
		{
			return DHT_BUS_HUNG;
		}
		retryCount++;
		_delay_us(2);
	} while(!THERM_READ());
	// Send the activate pulse
	cli();
	THERM_LOW();
	THERM_OUTPUT_MODE(); // Output Low
	sei();
	_delay_us(1100); // 1.1 ms
	cli();
	THERM_HIGH();
	THERM_INPUT_MODE();	// Switch back to input so pin can float
	sei();
	// Find the start of the ACK Pulse
	retryCount = 0;
	do
	{
		if (retryCount > 25) //(Spec is 20 to 40 us, 25*2 == 50 us)
		{
			return DHT_ERROR_NOT_PRESENT;
		}
		retryCount++;
		_delay_us(2);
	} while(!THERM_READ());
	// Find the end of the ACK Pulse
	retryCount = 0;
	do
	{
		if (retryCount > 50) //(Spec is 80 us, 50*2 == 100 us)
		{
			return DHT_ERROR_ACK_TOO_LONG;
		}
		retryCount++;
		_delay_us(2);
	} while(THERM_READ());
	// Read the 40 bit data stream
	for(i = 0; i < DHT22_DATA_BIT_COUNT; i++)
	{
		// Find the start of the sync pulse
		retryCount = 0;
		do
		{
			if (retryCount > 35) //(Spec is 50 us, 35*2 == 70 us)
			{
				return DHT_ERROR_SYNC_TIMEOUT;
			}
			retryCount++;
			_delay_us(2);
		} while(!THERM_READ());
		// Measure the width of the data pulse
		retryCount = 0;
		do
		{
			if (retryCount > 50) //(Spec is 80 us, 50*2 == 100 us)
			{
				return DHT_ERROR_DATA_TIMEOUT;
			}
			retryCount++;
			_delay_us(2);
		} while(THERM_READ());
		bitTimes[i] = retryCount;
	}
	// Now bitTimes have the number of retries (us *2)
	// that were needed to find the end of each data bit
	// Spec: 0 is 26 to 28 us
	// Spec: 1 is 70 us
	// bitTimes[x] <= 11 is a 0
	// bitTimes[x] >  11 is a 1
	// Note: the bits are offset by one from the data sheet, not sure why
	for(i = 0; i < 16; i++)
	{
	    if(bitTimes[i + 1] > 11)
		{
			currentHumidity |= (1 << (15 - i));
		}
	}
	for(i = 0; i < 16; i++)
	{
		if(bitTimes[i + 17] > 11)
		{
			currentTemperature |= (1 << (15 - i));
		}
	}
	for(i = 0; i < 8; i++)
	{
		if(bitTimes[i + 33] > 11)
		{
			checkSum |= (1 << (7 - i));
		}
	}

	*humidity = ((float)(currentHumidity & 0x7FFF)) / 10.0;
	
	if(currentTemperature & 0x8000)
	{
		// Below zero, non standard way of encoding negative numbers!
		currentTemperature &= 0x7FFF;
		*temperature = ((float)currentTemperature / 10.0) * -1.0;
	}
	else
	{
		*temperature = (float)currentTemperature / 10.0;
	}

	csPart1 = currentHumidity >> 8;
	csPart2 = currentHumidity & 0xFF;
	csPart3 = currentTemperature >> 8;
	csPart4 = currentTemperature & 0xFF;
	
	if(checkSum == ((csPart1 + csPart2 + csPart3 + csPart4) & 0xFF))
	{
		//*humidity = 10;
		return DHT_ERROR_NONE;
	}
	//*humidity = 20;
	return DHT_ERROR_CHECKSUM;
}

