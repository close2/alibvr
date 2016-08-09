# name of your main code file without the .c / .cpp suffix
CPP_FILES =$(wildcard src/*.cpp)
C_FILES =$(wildcard src/*.c)
CPP_TARGETS = $(patsubst src/%.cpp,%,$(CPP_FILES))
C_TARGETS = $(patsubst src/%.c,%,$(C_FILES))
TARGETS ?= $(CPP_TARGETS) $(C_TARGETS)
SOURCEDIR = src
BUILDDIR = build
TOOLSDIR = tools

# location of alibvr
ALIBVR = ../../src
# type of atmega chip
ATMEGA = atmega328


CC  = avr-gcc
CXX = avr-gcc
OBJDUMP = avr-objdump
OBJ2HEX = avr-objcopy 


INC = -I $(ALIBVR)

CFLAGS_COMMON  = $(INC)
CFLAGS_COMMON += -gdwarf-2
CFLAGS_COMMON += -Wall -Wno-unused-function
CFLAGS_COMMON += -mcall-prologues -mmcu=$(ATMEGA)

CFLAGS_DEBUG   = -g3 $(CFLAGS_COMMON)
CFLAGS_RELEASE = -g -Os $(CFLAGS_COMMON)

CXXFLAGS_COMMON  = -std=c++11
CXXFLAGS_COMMON += $(INC)
CXXFLAGS_COMMON += -fno-threadsafe-statics -fwhole-program
CXXFLAGS_COMMON += -gdwarf-2
CXXFLAGS_COMMON += -Wall -Wno-unused-function
CXXFLAGS_COMMON += -mcall-prologues -mmcu=$(ATMEGA)
CXXFLAGS_COMMON += -fno-threadsafe-statics -std=c++11 -fwhole-program
CXXFLAGS_COMMON += -Werror


CXXFLAGS_DEBUG   = -g3 $(CXXFLAGS_COMMON)
CXXFLAGS_RELEASE = -g -Os $(CXXFLAGS_COMMON)


# Flashing
# FLASH_COMMAND	= avrdude -pm328 -c usbasp
FLASH_COMMAND	= avrdude -pm328 -c avrisp2 -P usb
# if usbprog can't connect try this (http://stackoverflow.com/questions/15313269/avrispmkii-connection-status-unknown-status-0x00):
# FLASH_COMMAND	= avrdude -pm328 -c avrisp2 -P usb -B 22

# needs NAME.hex appended.  For instance flash:w:adc.hex
FLASH_FLAGS           = -U flash:w:
# needs NAME.hex appended.  For instance flash:w:adc.hex
FLASH_FAST_FLAGS      = -V -U flash:w:
# needs NAME.eep appended.  For instance eeprom:w:adc.eep
FLASH_EEP_FLAGS       = -U eeprom:w:
FLASH_FUSE_8MHZ_FLAGS = -U lfuse:w:0xe2:m -U hfuse:w:0xd9:m -U efuse:w:0x07:m
FLASH_FUSE_1MHZ_FLAGS = -U lfuse:w:0x62:m -U hfuse:w:0xd9:m -U efuse:w:0x07:m

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings

# avrsim config:
# avr-gcc -mmcu=atmega328 -c $(TOOLSDIR)/simavr_conf.c -o $(BUILDDIR)/simavr_conf.o
# avr-objcopy -O binary --only-section=.mmcu $(BUILDDIR)/simavr_conf.o $(BUILDDIR)/simavr_conf.mmcu
# avr-objcopy --add-section .mmcu=$(BUILDDIR)/simavr_conf.mmcu $(BUILDDIR)/main.obj $(BUILDDIR)/main_sim.obj

# get the calibration byte
#avrdude -pm48 -D -c usbasp -U cal:r:cal.tmp:r

.PHONY: release debug .all clean clobber flash flash-eep flash-fast flash-fuse-8-MHz start-sim check-flash-is-set


release : CFLAGS = $(CFLAGS_RELEASE)
release : CXXFLAGS = $(CXXFLAGS_RELEASE)
release : .all

debug : CFLAGS = $(CFLAGS_DEBUG)
debug : CXXFLAGS = $(CXXFLAGS_DEBUG)
debug : .all

.all : $(TARGETS:%=$(BUILDDIR)/%.hex) $(TARGETS:%=$(BUILDDIR)/%.eep) $(TARGETS:%=$(BUILDDIR)/%.o)

$(BUILDDIR)/%.o : $(SOURCEDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILDDIR)/%.elf : $(BUILDDIR)/%.o
	$(CC) $(CFLAGS) $< -o $@

$(BUILDDIR)/%.hex : $(BUILDDIR)/%.o
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

$(BUILDDIR)/%.eep : $(BUILDDIR)/%.o
	$(OBJ2HEX) $(HEX_EEPROM_FLAGS) -O ihex $< $@

$(BUILDDIR)/%.s : $(BUILDDIR)/%.o
	$(OBJDUMP) -S --disassemble $< > $@

clean :
	rm -f $(TARGETS:%=$(BUILDDIR)/%.o) $(TARGETS:%=$(BUILDDIR)/%.s) $(BUILDDIR)/*.sch~ $(BUILDDIR)/gschem.log $(BUILDDIR)/*.S~ $(BUILDDIR)/*.hex $(BUILDDIR)/*.map $(BUILDDIR)/*.eep

clobber : clean
	rm -f $(TARGETS:%=$(BUILDDIR)/%.hex) $(TARGETS:%=$(BUILDDIR)/%.ps) $(TARGETS:%=$(BUILDDIR)/%.eep)

flash : check-flash-is-set $(BUILDDIR)/$(FLASH).hex
	$(FLASH_COMMAND) "$(FLASH_FLAGS)$<"

flash-fast : check-flash-is-set $(BUILDDIR)/$(FLASH).hex
	$(FLASH_COMMAND) $(FLASH_FAST_FLAGS)$<

flash-eep : check-flash-is-set $(BUILDDIR)/$(FLASH).eep
	$(FLASH_COMMAND) $(FLASH_EEP_FLAGS)$<

flash-fuse-8-MHz :
	$(FLASH_COMMAND) $(FLASH_FUSE_8MHZ_FLAGS)

check-flash-is-set :
ifndef FLASH
	$(error You have to set the FLASH variable when flashing.  Example: make flash FLASH=adc_sync)
endif

start-sim :
	$(TOOLSDIR)/simavr.sh &

