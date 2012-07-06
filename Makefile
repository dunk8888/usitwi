MMCU=attiny85
CC=avr-gcc
OBJCPY=avr-objcopy
AVRDUDE=avrdude
AVRSIZE=avr-size

compile:
	$(CC) -mmcu=$(MMCU) -Os -g examples/register.c src/slave.c -o out/register.elf
	$(OBJCPY) -j .text -j .data -O ihex out/register.elf out/register.hex
	$(OBJCPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex out/register.elf out/register.eeprom.hex
	$(AVRSIZE) -A -t out/register.elf

program:
	$(AVRDUDE) -p $(MMCU) -v -e -U flash:w:out/register.hex

clean:
	rm -f out/*.o out/*.elf out/*.hex
