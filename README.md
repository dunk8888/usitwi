
# Usitwi
A [TWI](http://en.wikipedia.org/wiki/I%C2%B2C)
slave driver for Atmel microcontrollers with USI hardware.

This project is a fork from 
[usi-i2c-slave](https://code.google.com/p/usi-i2c-slave/).

## Usage
Usitwi has events for **TWI START**, **TWI STOP**, **TWI READ** and
**TWI WRITE**.

To use Usitwi you have to provide a `TWI_slaveAddress` variable and implement
the functions `TWI_onStart`, `TWI_onStop`, `TWI_onRead` and `TWI_onWrite`.

### Usitwi Interface

#### TWI_slaveAddress

This variable stores the slave TWI address.

```c
uint8_t TWI_slaveAddress = 0x42;
```

#### TWI_slaveInit

This is the initialization of the TWI stack. It will initialize the necessary
port pins and interrupts.

```c
TWI_slaveInit();
```

#### TWI_onStart

The `TWI_onStart` function is called, when a TWI start condition is detected.
The first argument is `1` if the master wants to read and `0` otherwise.

```c
void TWI_onStart(uint8_t read) {

}
```

#### TWI_onStop

The `TWI_onStop` function is called, when a TWI stop condition is detected.

```c
void TWI_onStop() {

}
```

#### TWI_onRead

The `TWI_onRead` function is called, when the master wants to read a byte from
the slave. The return value is the byte that will be sent to the master.

```c
uint8_t TWI_onRead() {
	return 0x42;
}
```

#### TWI_onWrite

The `TWI_onWrite` function is called, when the master wants to write a byte
to the slave. The first argument is the byte that the master transmitted.

```c
void TWI_onWrite(uint8_t value) {

}
```

### Basic Example

To use Usitwi you just have to include the `src/slave.h` header file and
add `src/slave.c` to your build.

This example implements a basic TWI register based access. The slave has
two registers which have the addresses `0x00` and `0x01`. It will light
up a LED connected to port `PB2` if the values stored in these two
registers are equal.

The microcontroller runs at 8MHz and the TWI slave address is `0x42`.

```c
#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../src/slave.h"

#define LED PB1

#define NULL_REGISTER 0xFF

static volatile uint8_t register1 = 0;
static volatile uint8_t register2 = 0;

uint8_t currentRegister = NULL_REGISTER;

uint8_t TWI_onRead() {
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

void TWI_onWrite(uint8_t value) {
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

void TWI_onStart(uint8_t read) {
	if (!read) {
		currentRegister = NULL_REGISTER;
	}
}

void TWI_onStop() {
	currentRegister = NULL_REGISTER;
}

uint8_t TWI_slaveAddress = 0x42;

int main() {
	DDRB |= (1 << LED);

	TWI_slaveInit();

	sei();

	while (1) {
		if (register1 == register2)
			PORTB |= 1 << LED;
		else
			PORTB &= ~(1 << LED);
	}
}
```

## Supported Devices

 * ATtiny
   * 2313
   * 25, 45, 85
   * 26
   * 261, 461, 861
 * ATmega
   * 165, 325, 3250, 645, 6450, 329, 3290
   * 169

## Implementation
See [Application Note AVR312](http://www.atmel.com/Images/doc2560.pdf) for a detailed description of the implementation idea.

## Licence

[GPLv3](http://www.gnu.org/licenses/gpl.html)

