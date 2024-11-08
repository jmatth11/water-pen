
# the baud rate for the programmer
BAUD=115200
# the programmer to flash the device
PROGRAMMER=usbtiny
# the clocks frequency
CLOCK_FREQ=1000000UL
# memory operation to perform on target (flash operation to write the hex file to)
MEM_OP=flash:w:main.hex
MCU=attiny85
# MCU=atmega328p
# for atmega328p
# TARGET_PLATFORM=m328p
# for attiny85
TARGET_PLATFORM=t85
USB_PORT=usb:001:007

# Remove section when generating file
SECTION=.eeprom
EXE=main

.PHONY: all
all:
	avr-gcc -Os -DF_CPU=$(CLOCK_FREQ) -mmcu=$(MCU) -c -o main.o main.c
	avr-gcc -mmcu=$(MCU) main.o -o $(EXE)

.PHONY: upload
upload: all
	avr-objcopy -O ihex -R $(SECTION) $(EXE) $(EXE).hex
	avrdude -F -V -c $(PROGRAMMER) -p $(TARGET_PLATFORM) -P $(USB_PORT) -b $(BAUD) -U $(MEM_OP)

.PHONY: clean
clean:
	rm *.o *.hex $(EXE)
