CC=avr-gcc
SOURCES=$(shell find . -name '*.c')
OBJ=obj
BIN=bin
OBJECTS=$(addprefix $(OBJ)/,$(SOURCES:%.c=%.o))

CFLAGS=-Os
# the baud rate for the programmer
BAUD=115200
# the programmer to flash the device
PROGRAMMER=usbtiny
# the clocks frequency
CLOCK_FREQ=1000000UL
# memory operation to perform on target (flash operation to write the hex file to)
MEM_OP=flash:w:bin/main.hex
MCU=attiny85
# MCU=atmega328p
# for atmega328p
# TARGET_PLATFORM=m328p
# for attiny85
TARGET_PLATFORM=t85
USB_PORT=usb:001:007

# Remove section when generating file
SECTION=.eeprom
EXE=$(BIN)/main

.PHONY: all
all: upload

.PHONY: build
build: $(OBJECTS)
	@mkdir -p $(BIN)
	$(CC) -mmcu=$(MCU) $^ -o $(EXE)

$(OBJ)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS) -DF_CPU=$(CLOCK_FREQ) -mmcu=$(MCU)

.PHONY: upload
upload: build
	avr-objcopy -O ihex -R $(SECTION) $(EXE) $(EXE).hex
	avrdude -F -V -c $(PROGRAMMER) -p $(TARGET_PLATFORM) -P $(USB_PORT) -b $(BAUD) -U $(MEM_OP)

.PHONY: clean
clean:
	@rm -rf $(OBJ)/* 2> /dev/null
	@rm -f $(BIN)/* 2> /dev/null
