##PERIPHERALS

HD44780 - LCD (16x2)

DHT-22  - temperature and humidity sensor

##PROJECT CONFIGURATIONS

Set MCU frequency:

```
#define F_CPU 16000000UL // set 16MHz frequency
```

Or through project configuration:

```
Toolchain -> C Compiler -> Symbols -> Defined symbols (-D) -> F_CPU=16000000UL
```

To use sprintf with float/double type (? symbol instead of number):

```
Toolchain -> Linker -> General -> Check 'Use vprintf library(-Wl,-u,vfprintf)'
Toolchain -> Linker -> Libraries -> Add libm and libprintf_flt.a

sprintf(buffer, "Temp: %2.2f C", number);
```

##PORT CONFIGURATIONS

* DDRx = 0 // input (get data: buttons, sensors, etc.)
 * PORTx = 0 // hi-Z
 * PORTx = 1 // pull-up
* DDRx = 1 // output (set data: leds, motors, etc.)
 * PORTx = 0 // logic 0
 * PORTx = 1 // logic 1

##BINARY OPERATIONS

[AVRFreaks - Bit manipulation (AKA "Programming 101") ](http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=37871)

```
// ## - concatenation in macros
#define LOW 0 
#define HIGH 1

#define INPUT(port, pin) (DDR ## port &= ~(1<<pin))
#define OUTPUT(port, pin) (DDR ## port |= (1<<pin))
#define PIN_SET(port, pin) (PORT ## port |= (1<<pin))
#define PIN_CLEAR(port, pin) (PORT ## port &= ~(1<<pin))
#define PIN_TOGGLE(port, pin) (PORT ## port ^= (1<<pin))
#define PIN_CHECK(port, pin) (PORT ## port & (1<<pin))

#define INPUT_MASK(port, mask) (DDR ## port &= ~(mask))
#define OUTPUT_MASK(port, mask) (DDR ## port |= (mask))
#define PIN_MASK_SET(port, mask) (PORT ## port |= (mask))
#define PIN_MASK_CLEAR(port, mask) (PORT ## port &= ~(mask))
#define PIN_MASK_TOGGLE(port, mask) (PORT ## port ^= (mask))
#define PIN_MASK_CHECK(port, mask) (PORT ## port & (mask))

#define u08 unsigned char		// 0 to 255
#define s08 signed char			// -128 to 127

#define u16 unsigned int		// 0 to 65535
#define s16 signed int			// -32768 to 32767

#define u32 unsigned long int	// 0 to 4294967295
#define s32 signed long int		// -2147483648 to 2147483647

#define f32 float				// ±1.175e-38 to ±3.402e38
#define d32 double				// ±1.175e-38 to ±3.402e38

OUTPUT(D, 3); // port D, pin 3 as output
PIN_SET(D, 3); // set port D pin 3 to HIGH
if (PIN_CHECK(D, 3) == HIGH) {
}
```

```
PORTx = 0b10001000;
PORTx = 0x88;
PORTx = (1<<Px7)|(1<<Px3);
PORTx = _BV(Px7)|_BV(Px3);
```

```
PORTx &= ~(1<<Px7); // AND - set 0 ONLY bit 7
PORTx |= (1<<Px7);  // OR  - set 1 ONLY bit 7
PORTx ^= (1<<Px7);  // XOR - toggle ONLY bit 7

// To see if bit 7 is set in the variable PORTx
if (PORTx & (1<<7)) { 
}
```

| AND  |  OR  | XOR  |
|:----:|:----:|:----:|
| 0011 | 0011 | 0011 |
| 0101 | 0101 | 0101 |
| 0001 | 0111 | 0110 |

##INTERRUPTS
[Interrupts short and simple: Part 1 - Good programming practices](http://www.embedded.com/design/programming-languages-and-tools/4397803/Interrupts-short-and-simple--Part-1---Good-programming-practices)

[Interrupts short & simple: Part 2 - Variables, buffers & latencies](http://www.embedded.com/design/programming-languages-and-tools/4398340/Interrupts-short---simple--Variables--buffers---latencies)

[Interrupts short and simple: Part 3 - More interrupt handling tips](http://www.embedded.com/design/programming-languages-and-tools/4398710/Interrupts-short-and-simple--Part-3---More-interrupt-handling-tips)

[AVRFreaks - Newbie's Guide to AVR Interrupts](http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=89843)

EIMSK - External Interrupt Mask Register (enable INT0 - INT7)

EICRA - External Interrupt Control Register A (configure INT0 - INT3):

| ISCn1 | ISCn0 | Description |
|:-----:|:-----:| ----------- |
| 0     | 0     | The low level of INTn generates an interrupt request.
| 0     | 1     | Reserved
| 1     | 0     | The falling edge of INTn generates asynchronously an interrupt request.
| 1     | 1     | The rising edge of INTn generates asynchronously an interrupt request.

EICRB - External Interrupt Control Register B (configure INT4 - INT7):

| ISCn1 | ISCn0 | Description |
|:-----:|:-----:| ----------- |
|   0   |   0   | The low level of INTn generates an interrupt request.
|   0   |   1   | Any logical change on INTn generates an interrupt request.
|   1   |   0   | The falling edge between two samples of INTn generates an interrupt request.
|   1   |   1   | The rising edge between two samples of INTn generates an interrupt request.

INT4 example:

```
DDRE = ~(1<<PE4);	// read button 1
PORTE = (1<<PE4);	// enable pull-up (logic 1 by default)

EIMSK = (1<<INT4);	// enable interrupt INT4
EICRB = (1<<ISC41); // interrupt on button pressed down (logic 0 - falling edge)

ISR (INT4_vect) {
	// blink LED
}
```

##TIMERS

[AVR Timers full review](http://maxembedded.com/category/microcontrollers-2/atmel-avr/avr-timers-atmel-avr/)

[AVRFreaks - Newbie's Guide to AVR Timers](http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=50106)


```
TIMSK - Timer/Counter Interrupt Mask Register
TCCR0 - Timer/Counter Control Register
TCNT0 - Timer/Counter Register
OCR0 - Output Compare Register
ASSR - Asynchronous Status Register
```

TIMSK [bits OCIE0, TOIE0] - Timer interrupts:

```
When the OCIE0 bit is written to one, Compare Match interrupt is enabled
When the TOIE0 bit is written to one, Overflow interrupt is enabled
```

TCCR0 [bits WGM01:0] - Waveform Generation Mode:

| Mode | WGM01 (CTC0) | WGM00 (PWM0) | Timer/Counter Mode of Operation | TOP  | Update of OCR0 at | TOV0 Flag Set on |
|:----:|:------------:|:------------:| ------------------------------- |:----:|:-----------------:|:----------------:|
|  0   |      0       |      0       | Normal                          | 0xFF |    Immediate      |       MAX        |
|  1   |      0       |      1       | PWM, Phase Correct              | 0xFF |       TOP         |      BOTTOM      |
|  2   |      1       |      0       | CTC                             | OCR0 |    Immediate      |       MAX        |
|  3   |      1       |      1       | Fast PWM                        | 0xFF |      BOTTOM       |       MAX        |


> The 8-bit comparator continuously compares TCNT0 with the Output Compare Register (OCR0). Whenever TCNT0 equals OCR0, the comparator signals a match. A match will set the output compare flag (OCF0) at the next timer clock cycle. If enabled (OCIE0 = 1), the output compare flag generates an output compare interrupt. The OCF0 flag is automatically cleared when the interrupt is executed. Alternatively, the OCF0 flag can be cleared by software by writing a logical one to its I/O bit location. The waveform generator uses the match signal to generate an output according to operating mode set by the WGM01:0 bits and compare output mode (COM01:0) bits. OC0 pin will be updated if compare match had occurred (the COM01:0 bits settings define whether the OC0 pin is set, cleared or toggled).

###Nomal Mode:

> The simplest mode of operation is the normal mode (WGM01:0 = 0). In this mode the counting direction is always up (incrementing), and no counter clear is performed. The counter simply overruns when it passes its maximum 8-bit value (TOP = 0xFF) and then restarts from the bottom (0x00). In normal operation the Timer/Counter overflow flag (TOV0) will be set in the same timer clock cycle as the TCNT0 becomes zero. The TOV0 flag in this case behaves like a ninth bit, except that it is only set, not cleared. However, combined with the timer overflow interrupt that automatically clears the TOV0 flag, the timer resolution can be increased by software. There are no special cases to consider in the normal mode, a new counter value can be written anytime.

> The output compare unit can be used to generate interrupts at some given time. Using the output compare to generate waveforms (through OC0 pin) in normal mode is not recommended, since this will occupy too much of the CPU time.

###Clear Timer on Compare Match (CTC) Mode:

> In Clear Timer on Compare or CTC mode (WGM01:0 = 2), the OCR0 Register is used to manipulate the counter resolution. In CTC mode the counter is cleared to zero when the counter value (TCNT0) matches the OCR0. The OCR0 defines the top value for the counter, hence also its resolution. This mode allows greater control of the compare match output frequency. It also simplifies the operation of counting external events.

> An interrupt can be generated each time the counter value reaches the TOP value by using the OCF0 flag. If the interrupt is enabled, the interrupt handler routine can be used for updating the TOP value. However, changing the TOP to a value close to BOTTOM when the counter is running with none or a low prescaler value must be done with care since the CTC mode does not have the double buffering feature. If the new value written to OCR0 is lower than the current value of TCNT0, the counter will miss the compare match. The counter will then have to count to its maximum value (0xFF) and wrap around starting at 0x00 before the compare match can occur. 

###Fast PWM Mode:

> The fast Pulse Width Modulation or fast PWM mode (WGM01:0 = 3) provides a high frequency PWM waveform generation option. The fast PWM differs from the other PWM option by its single-slope operation. The counter counts from BOTTOM to MAX then restarts from BOTTOM. In non-inverting Compare Output mode, the output compare (OC0) is cleared on the compare match between TCNT0 and OCR0, and set at BOTTOM. In inverting Compare Output mode, the output is set on compare match and cleared at BOTTOM. Due to the single-slope operation, the operating frequency of the fast PWM mode can be twice as high as the phase correct PWM mode that uses dual-slope operation. This high frequency makes the fast PWM mode well suited for power regulation, rectification, and DAC applications. High frequency allows physically small sized external components (coils, capacitors), and therefore reduces total system cost.

###Phase Correct PWM Mode

> The phase correct PWM mode (WGM01:0 = 1) provides a high resolution phase correct PWM waveform generation option. The phase correct PWM mode is based on a dual-slope operation. The counter counts repeatedly from BOTTOM to MAX and then from MAX to BOTTOM. In noninverting Compare Output mode, the output compare (OC0) is cleared on the compare match between TCNT0 and OCR0 while counting up, and set on the compare match while downcounting. In inverting Output Compare mode, the operation is inverted. The dual-slope operation has lower maximum operation frequency than single slope operation. However, due to the symmetric feature of the dual-slope PWM modes, these modes are preferred for motor control applications.

TCCR0 [bits COM01:0] - Compare Match Output Mode (non-PWM Mode):

| COM01 | COM00 |  Description  |
|:-----:|:-----:|:-------------:|
|   0   |   0   | Normal port operation, OC0 disconnected.
|   0   |   1   | Toggle OC0 on compare match
|   1   |   0   | Clear OC0 on compare match
|   1   |   1   | Set OC0 on compare match

TCCR0 [bits COM01:0] - Compare Match Output Mode (Fast PWM Mode):

| COM01 | COM00 |  Description  |
|:-----:|:-----:|:-------------:|
|   0   |   0   | Normal port operation, OC0 disconnected.
|   0   |   1   | Reserved
|   1   |   0   | Clear OC0 on compare match, set OC0 at BOTTOM, (non-inverting mode)
|   1   |   1   | Set OC0 on compare match, clear OC0 at BOTTOM, (inverting mode)

TCCR0 [bits COM01:0] - Compare Match Output Mode (Phase Correct PWM Mode):

| COM01 | COM00 |  Description  |
|:-----:|:-----:|:-------------:|
|   0   |   0   | Normal port operation, OC0 disconnected.
|   0   |   1   | Reserved
|   1   |   0   | Clear OC0 on compare match when up-counting. Set OC0 on compare match when downcounting.
|   1   |   1   | Set OC0 on compare match when up-counting. Clear OC0 on compare match when downcounting.

TCCR0 [bits CS02:0] - Clock Select:	

| CS02 | CS01 | CS00 | Description |
|:----:|:----:|:----:| ----------- |
|  0   |  0   |  0   | No clock source (Timer/Counter stopped)
|  0   |  0   |  1   | clkT0S/1 (No prescaling)
|  0   |  1   |  0   | clkT0S/8 (From prescaler)
|  0   |  1   |  1   | clkT0S/32 (From prescaler)
|  1   |  0   |  0   | clkT0S/64 (From prescaler)
|  1   |  0   |  1   | clkT0S/128 (From prescaler)
|  1   |  1   |  0   | clkT0S/256 (From prescaler)
|  1   |  1   |  1   | clkT0S/1024 (From prescaler)

Clocks per second / Milliseconds per second / Pre-scaler = Ticks per second

(20000000 / 1000) / 256 = 78.125
(32768 / 1000) / 128 = 256

16-bit timers:

> The TOP value, or maximum Timer/Counter value, can in some modes of operation be defined by either the OCRnA Register, the ICRn Register, or by a set of fixed values. When using OCRnA as TOP value in a PWM mode, the OCRnA Register can not be used for generating a PWM output. However, the TOP value will in this case be double buffered allowing the TOP value to be changed in run time. If a fixed TOP value is required, the ICRn Register can be used as an alternative, freeing the OCRnA to be used as PWM output.

> OCRnB must be less than or equal to OCRnA, otherwise TIMERx_COMPA_vect will reset TCNT1 counter before OCRnB interrupt!!!

```
//TCNT0 = 0;								// timer 0 start value of counter (from 0 to 255) - 1 second (when reaches 255)
ASSR = (1<<AS0);							// enable asynchronous clock from TOSC1 (32,768kHz)
TIMSK = (1<<TOIE0);							// enable overflow interrupt (TIMER0_OVF_vect)
TCCR0 = (0<<WGM01)|(0<<WGM00);				// timer 0 mode - Normal
TCCR0 |= (1<<CS02)|(0<<CS01)|(1<<CS00);		// timer 0 pre-scaler (32.768 / 128 = 256 ticks per second)
	
//TCNT1 = 0;								// timer 1 start value of counter (from 0 to 65535) - 1 second (when reaches 15.624)
OCR1A = 15;									// timer 1 top value A for compare interrupt calling - 1 millisecond (15.625 / 1000 = 15)
TIMSK |= (1<<OCIE1A);						// enable compare interrupt (TIMER1_COMPA_vect)
TCCR1B = (0<<WGM13)|(1<<WGM12);				// timer 1 mode - CTC (OCRnA TOP)
TCCR1B |= (1<<CS12)|(0<<CS11)|(1<<CS10);	// timer 1 (another mask <!>) pre-scaler (16.000.000 / 1024 = 15.625 ticks per second)

ISR (TIMER0_OVF_vect) {
	// every 1 s
}
ISR (TIMER1_COMPA_vect) {
	// every 1 ms
}
```

##WATCHDOG

> The watchdog timer starts when the WDE bit is enabled and prescaler bits are configured for time-out condition. As watchdog timer reaches time-out condition, watchdog timer is reset and generates a pulse of one clock cycle which resets the program counter. When watch dog timer resets the timer, the WDRF (Watch Dog Reset Flag) bit in MCUCSR register is set by the hardware. To disable the watchdog timer following steps must be followed:

1. Set the WDE and WDTOE bits in same clock cycle WDTCR register. The logic one must be written to WDE bit even though it is set to one already.

2. After four clock pulses, write logic 0 to the WDE bit. Otherwise watchdog timer will not be disabled.

```
#include <avr/wdt.h>

ISR (TIMER0_OVF_vect) {
	s_timer++;
	wdt_reset();
}

int main(void) {
	wdt_enable(WDTO_2S); // restart MCU after 2s without wdt_reset

	while(1) {
		_delay_ms(3000); // MCU will be restarted, if delay > 2s
		// wdt_disable();
	}
}
```

##MEMORY

PROGMEM allows to store and read data directly from FLASH (128 KB) instead of loading it to SRAM (4 KB)

The AVR internal FLASH memory has a limited lifespan of 10,000 writes - reads are unlimited.

[AVRFreaks - GCC and the PROGMEM Attribute](http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=38003)

PSTR allows to declare string inline into the Flash memory without using variables.

The downside to using the PSTR macro rather than a PROGMEM modified string variable is that you can only use the PSTR string once.

```
#include <avr/pgmspace.h>

const char temp_str_P[] PROGMEM = "Temp: %2.2f C";

sprintf_P(buffer, temp_str_P, (double)float_number); // conversion float to double removes warning

lcd_puts_p(PSTR("Loading..."));
```

EEPROM (4 KB), short for Electronically Erasable Read-Only memory, is a form of non-volatile memory with a reasonably long lifespan. Because it is non-volatile, it will retain its information during periods of no AVR power and thus is a great place for storing sparingly changing data such as device parameters. 

The AVR internal EEPROM memory has a limited lifespan of 100,000 writes - reads are unlimited.

[AVRFreaks - Using the EEPROM memory in AVR-GCC](http://www.avrfreaks.net/?name=PNphpBB2&file=viewtopic&t=38417)


```
#include <avr/eeprom.h>

uint8_t eeprom_read_byte (const uint8_t * addr )
void eeprom_write_byte ( uint8_t * addr , uint8_t value )
void eeprom_update_byte ( uint8_t * addr , uint8_t value )

uint16_t eeprom_read_word (const uint16_t * addr )
void eeprom_write_word ( uint16_t * addr , uint16_t value )
void eeprom_update_word ( uint16_t * addr , uint16_t value )

uint16_t eeprom_read_dword (const uint32_t * addr )
void eeprom_write_dword ( uint32_t * addr , uint32_t value )
void eeprom_update_dword ( uint32_t * addr , uint32_t value )

uint16_t eeprom_read_float (const float * addr )
void eeprom_write_float (float * addr , float value )
void eeprom_update_float (float * addr , float value )

void eeprom_read_block (void * pointer_ram , const void * pointer_eeprom , size_t n)
void eeprom_write_block (const void * pointer_ram , void * pointer_eeprom , size_t n)
void eeprom_update_block (const void * pointer_ram , void * pointer_eeprom , size_t n)
```

##NOTES

The static keyword tells the compiler to allocate the variable as if it were file scope (like the global variables), but it won't be globally visible now because it's hidden in a function. The variable has an eternal life, but can't be accessed by just any function.  This also has the effect of creating a variable that won't change from function call to function call. Static functions are functions that are only visible to other functions in the same file.

###volatile

```
/*
Without volatile the optimizer is seeing that s_timer is never changed anywhere inside main. The optimizer does not realize 
that s_timer can change inside main even if there are no instructions there which would cause s_timer value to change. 
Therefore, it takes a copy of the s_timer global variable as the function begins, and it bases its comparisons on that copy. 

The interrupt, in the meantime, is updating the original value of s_timer and main's copy of s_timer is left un-modified. 
*/

// this tells the two 'threads' (main execution loop and ISR code) to not 'cache' the value in a register, but always retrieve it from memory.
volatile int s_timer = 0;

ISR (TIMER0_OVF_vect) {
	s_timer++;
}

void wait_for_done (void) {
	// s_timer should to be volatile, to exit out of cycle
	while (s_timer < 200) {
		// do something
	}
}

// For example, this code is wrong:
int *volatile REGISTER = 0xfeed;
*REGISTER = new_val;

// To write clear, maintainable code using volatile, a reasonable idea is to build up more complex types using typedefs (of course this is a good idea anyway). 
// We could first make a new type “vint” which is a volatile int:
typedef volatile int vint;

// Next, we create a pointer-to-vint:
vint *REGISTER = 0xfeed;

// We might ask, does it make sense to declare an object as both const and volatile?
const volatile int *p;

// Although this initially looks like a contradiction, it is not.  The semantics of const in C are “I agree not to try to store to it” rather than “it does not change”
// So in fact this qualification is perfectly meaningful and would even be useful, for example, to declare a timer register that spontaneously changes value,
// but that should not be stored to (this example is specifically pointed out in the C standard).
```

###structures

```
struct { double x,y; } s1, s2, sm[9];
struct { 
	int   year;
    char  moth, day;
} date1, date2;

// with tag
struct student {
	char name[25];
    int  id, age;
    char prp; 
};

struct student st1,st2;

// recursion
struct node {
	int data;
    struct node * next;
} st1_node;

st1.name = "John";
st2.id = st1.id;
st1_node.data = st1.age;
```

###bit field

```
// (variable : <bit length>)
struct {
	unsigned active : 1;
	unsigned ready : 1;
	unsigned : 5;
	unsigned error : 1;
} flags; // x00000xx - 0 are not used bits

flags.active |= 1; // x00000x1
```

###pointers

```
int  var = 20; // actual variable declaration
int  *ip; // pointer variable declaration

ip = &var; // store address of var in pointer variable

printf("Address of var variable: %x\n", &var  );

// address stored in pointer variable
printf("Address stored in ip variable: %x\n", ip );

// access the value using the pointer
printf("Value of *ip variable: %d\n", *ip );

/*
Address of var variable: bffd8b3c
Address stored in ip variable: bffd8b3c
Value of *ip variable: 20
*/

double (*fun1)(int x, int y); // not the same as: int *fun (int x, int y);
double fun2(int k, int l);

fun1 = fun2; // initialize function pointer (required for calling)
(*fun1)(2,7); // call function by pointer
```

###other

```
char const * a = "a"; // const data value
char * const a = "a"; // const pointer value
const char * const a = "a";
```

```
int y, x = 3;

y = ++x; // y==4, x==4
y = x++; // y==3, x==4
```

```
int    * ( * comp [10]) ();
 6     5   3   1    2    4

char  * ( * ( * var ) () ) [10];
  7   6   4   2  1     3     5
```