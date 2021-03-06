# name of your main code file without the .c / .cpp suffix
NAME = the_name
# location of alibvr
ALIBVR = alibvr
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

CXXFLAGS_DEBUG   = -g3 $(CXXFLAGS_COMMON)
CXXFLAGS_RELEASE = -g -Os $(CXXFLAGS_COMMON)


# Flashing
# FLASH_COMMAND	= avrdude -pm328 -c usbasp
FLASH_COMMAND	= avrdude -pm328 -c avrisp2 -P usb
# if usbprog can't connect try this (http://stackoverflow.com/questions/15313269/avrispmkii-connection-status-unknown-status-0x00):
# FLASH_COMMAND	= avrdude -pm328 -c avrisp2 -P usb -B 22

FLASH_FLAGS           = -U flash:w:$(NAME).hex
FLASH_FAST_FLAGS      = -V -U flash:w:$(NAME).hex
FLASH_EEP_FLAGS       = -U eeprom:w:$(NAME).eep
FLASH_FUSE_8MHZ_FLAGS = -U lfuse:w:0xe2:m -U hfuse:w:0xd9:m -U efuse:w:0x07:m
FLASH_FUSE_1MHZ_FLAGS = -U lfuse:w:0x62:m -U hfuse:w:0xd9:m -U efuse:w:0x07:m

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings

# avrsim config:
# avr-gcc -mmcu=atmega328 -c simavr_conf.c -o simavr_conf.o
# avr-objcopy -O binary --only-section=.mmcu simavr_conf.o simavr_conf.mmcu
# avr-objcopy --add-section .mmcu=simavr_conf.mmcu main.obj main_sim.obj

# get the calibration byte
#avrdude -pm48 -D -c usbasp -U cal:r:cal.tmp:r

.PHONY: release debug .all clean clobber flash flashEep flashFast flashFuse8Mhz startSim


release : CFLAGS = $(CFLAGS_RELEASE)
release : CXXFLAGS = $(CXXFLAGS_RELEASE)
release : .all

debug : CFLAGS = $(CFLAGS_DEBUG)
debug : CXXFLAGS = $(CXXFLAGS_DEBUG)
debug : .all


.all : $(NAME).hex $(NAME).eep $(NAME).obj


%.obj : %.o
	$(CC) $(CFLAGS) $< -o $@

%.elf : %.o
	$(CC) $(CFLAGS) $< -o $@

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

%.eep : %.obj
	$(OBJ2HEX) $(HEX_EEPROM_FLAGS) -O ihex $< $@

%.s : %.obj
	$(OBJDUMP) -S --disassemble $< > $@

clean :
	rm -f $(NAME).o $(NAME).obj $(NAME).s *.sch~ gschem.log *.S~ *.hex *.map *.eep compile.flags

clobber : clean
	rm -f $(NAME).hex $(NAME).ps $(NAME).eep

flash : $(NAME).hex
	$(FLASH_COMMAND) $(FLASH_FLAGS)

flashFast : $(NAME).hex
	$(FLASH_COMMAND) $(FLASH_FAST_FLAGS)

flashEep : $(NAME).eep
	$(FLASH_COMMAND) $(FLASH_EEP_FLAGS)

flashFuse8MHz :
	$(FLASH_COMMAND) $(FLASH_FUSE_8MHZ_FLAGS)

startSim :
	./simavr.sh &
