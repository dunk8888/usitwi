#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t cmd_buffer[7];
volatile int8_t  cmd_args = 0;
volatile int8_t  cmd_rets = 0;
void           (*cmd_function)();

void ledOn() {
	PORTB ^= (1 << PB1);
}

void get42() {
	cmd_buffer[0] = 0x42;
}

inline void cmd_lookup(uint8_t cmd) {
	if        ( cmd == 0x01 ) {
		cmd_args     = -1;
		cmd_rets     = -1;
		cmd_function = ledOn;
	} else if ( cmd == 0x02 ) {
		cmd_args     = -1;
		cmd_rets     = 0;
		cmd_function = get42;
	} else {
		cmd_function = 0;
	}
}

uint8_t usitwi_address = 0x42;

int main() {
	DDRB = (1 << PB1);
	usitwi_init();
	sei();
	while (1) {
	}
}


