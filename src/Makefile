CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude -p attiny85 -c usbtiny -v

CFLAGS = -Wall -Wextra -Wno-unused-parameter -Os -Iusbdrv -I. -mmcu=attiny85 -DF_CPU=16500000

# Object files for the firmware (usbdrv/oddebug.o not strictly needed I think)
OBJECTS = trinketusb.o vusbmidi.c usbdrv/usbdrv.o usbdrv/usbdrvasm.o SoftwareSerial.cpp.o main.cpp.o

all: main.hex

flash: all
	$(DUDE) $(DUDEFLAGS) -U flash:w:main.hex

# Housekeeping if you want it
clean:
	$(RM) *.o *.hex *.elf usbdrv/*.o

main.hex: main.elf
	$(OBJCOPY) -j .text -j .data -O ihex main.elf main.hex

main.elf: $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

# Without this dependence, .o files will not be recompiled if you change 
# the config! I spent a few hours debugging because of this...
$(OBJECTS) $(DEMOS): usbconfig.h

# From C source to .o object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# From C++ source to .o object file
%.cpp.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# From assembler source to .o object file
%.o: %.S
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@
