#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../src/slave.h"

#define LED PB1

#define NULL_REGISTER 0xFF

static volatile uint8_t register1 = 0;
static volatile uint8_t register2 = 0;

uint8_t currentRegister = NULL_REGISTER;

uint8_t onRead() {
	switch(currentRegister) {
		case 0:
			return register1;
		break;
		case 1:
			return register2;
		break;
		default:
			return 0xFF;
		break;
	}
}

void onWrite(uint8_t value) {
	if (currentRegister == NULL_REGISTER) {
		currentRegister = value;
	} else {
		switch(currentRegister) {
			case 0:
				register1 = value;
			break;
			case 1:
				register2 = value;
			break;
		}
	}
}

void onStart(uint8_t read) {
	if (!read) {
		currentRegister = NULL_REGISTER;
	}
}

void onStop() {
	currentRegister = NULL_REGISTER;
}

int main() {
	DDRB |= (1 << LED);

	usiTwiSlaveInit(0x42, onStart, onStop, onRead, onWrite);

	sei();

	while (1) {
		if (register1 == register2)
			PORTB |= 1 << LED;
		else
			PORTB &= ~(1 << LED);
	}
}
