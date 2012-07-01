MMCU=attiny85
CC=avr-gcc
OBJCPY=avr-objcopy
AVRDUDE=sudo avrdude
AVRSIZE=avr-size

compile:
	$(CC) -mmcu=$(MMCU) -Os -g i2ctest.c usiTwiSlave.c -o i2ctest.elf
	$(OBJCPY) -j .text -j .data -O ihex i2ctest.elf i2ctest.hex
	$(OBJCPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex i2ctest.elf i2ctest.eeprom.hex
	$(AVRSIZE) -A -t i2ctest.elf

program:
	$(AVRDUDE) -c usbasp -p $(MMCU) -v -e -U flash:w:i2ctest.hex

clean:
	rm -f *.o *.elf *.hex
