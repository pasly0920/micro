## Project Name
PROJECT = jkit-128-template
TARGET = $(PROJECT).elf

## General Flags
MCU = atmega128
CC = avr-gcc
CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
## -Wall: Enable all warnings
## -std=gnu99: Use the GNU C99 extensions
## -Os: Optimize for size
## -fpack-struct: Pack structure members together without holes(data structure alignment)
CFLAGS = $(COMMON)
CFLAGS += -Wall -std=gnu99 -Os -fpack-struct
# CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d

## Assembly specific flags
## -x assembler-with-cpp: This option tells the compiler to run the C preprocessor on all assembly source files
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp

## Linker flags
## -Wl, -Map: Create a link map
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=$(PROJECT).map

## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings

## Libraries
LIBS = -lm -lprintf_flt

## Objects that must be built in order to link
OBJECTS = main.o

## Objects explicitly added by the user
LINKONLYOBJECTS =

## Build
all: $(TARGET) $(PROJECT).hex $(PROJECT).eep $(PROJECT).lss

## Compile
main: main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

## Link
$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(PROJECT).elf dep/* $(PROJECT).hex $(PROJECT).eep $(PROJECT).lss $(PROJECT).map

## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)
