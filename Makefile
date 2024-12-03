CC=avr-gcc
SOURCES=$(shell find . -name '*.c')
INCLUDES=-I./src/
OBJ=obj
BIN=bin
OBJECTS=$(addprefix $(OBJ)/,$(SOURCES:%.c=%.o))

CFLAGS=-Os -Wall -std=c11
# the baud rate for the programmer
BAUD=115200
# the programmer to flash the device
PROGRAMMER=usbtiny
# the clocks frequency
CLOCK_FREQ=1000000UL
# memory operation to perform on target (flash operation to write the hex file to)
MEM_OP=flash:w:bin/main.hex:i
MCU=attiny85
# MCU=atmega328p
# for atmega328p
# TARGET_PLATFORM=m328p
# for attiny85
TARGET_PLATFORM=t85
USB_PORT=usb:001:007
LD_FLAGS=-DF_CPU=$(CLOCK_FREQ)

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
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDES) $(LD_FLAGS) -mmcu=$(MCU)

.PHONY: upload
upload: build
	avr-objcopy -j .text -j .data -O ihex -R $(SECTION) $(EXE) $(EXE).hex
	avrdude -F -V -c $(PROGRAMMER) -p $(TARGET_PLATFORM) -P $(USB_PORT) -b $(BAUD) -U $(MEM_OP)

.PHONY: fuse
fuse:
	avrdude -F -V -c $(PROGRAMMER) -p $(TARGET_PLATFORM) -U hfuse:w:0xDD:m -U lfuse:w:0xF1:m

.PHONY: clean
clean:
	@rm -rf $(OBJ)/* 2> /dev/null
	@rm -f $(BIN)/* 2> /dev/null
