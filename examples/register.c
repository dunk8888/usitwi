#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay_basic.h>
#include "../src/slave.h"

#define I2C_SLAVE_ADDR  0x26
#define LED PB3

void adcInit() {

	// Enable ADC2/PB4 as ADC input
	ADMUX |= 1 << MUX1;

	// Set the ADC clock precaler to 16 (ie 500KHz if the main clock is 8MHz).
	ADCSRA |= 1 << ADPS2;

	// Left align the output (this lets us just use the 8 bits in the ADCH output register (we 
	// can ignore the extra 2 bits in the ADCL register).
	ADMUX |= 1 << ADLAR;

	// Enable ADC.
	ADCSRA |= 1 << ADEN;
}

uint8_t adcRead() {

	// Start a conversion	
	ADCSRA |= 1 << ADSC;

	// Wait for it to finish
	while (ADCSRA & (1 << ADSC));

	// Get the result.
	return ADCH;
}

// Somewhere to store the values the master writes to i2c register 2 and 3.
static volatile uint8_t i2cReg2 = 0;
static volatile uint8_t i2cReg3 = 0;

uint8_t currentRegister = 0xFF;

// A callback triggered when the i2c master attempts to read from a register.
uint8_t onTWIRead()
{
	switch(currentRegister) {
		case 0: 
			return 10;
		break;
		case 1:
			return adcRead();
		break;
		case 2:
			return i2cReg2;
		break;
		case 3:
			return i2cReg3;
		break;
		default:
			return 0xFF;
		break;
	}
}

// A callback triggered when the i2c master attempts to write to a register.
void onTWIWrite(uint8_t value)
{
	if (currentRegister == 0xFF) {
		currentRegister = value;
	} else {
		switch(currentRegister) {
			case 2:
				i2cReg2 = value;
			break;
			case 3:
				i2cReg3 = value;
			break;
		}
	}
}

void onTWIStart(uint8_t value)
{
	if (!value) {
		currentRegister = 0xFF;
	}
}

void onTWIStop()
{
	currentRegister = 0xFF;
}

int main()
{
	// Set the LED pin as output.
	DDRB |= (1 << LED);

	usiTwiSlaveInit(I2C_SLAVE_ADDR, onTWIStart, onTWIStop, onTWIRead, onTWIWrite);
	adcInit();
	sei();

	while (1)
	{
		// This is a pretty pointless example which allows me to test writing to two i2c registers: the
		// LED is only lit if both registers have the same value.
		if (i2cReg2 == i2cReg3)
			PORTB |= 1 << LED;
		else
			PORTB &= ~(1 << LED);
	}
}
 
