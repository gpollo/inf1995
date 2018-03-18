include ../Makefile.common

OBJECTS=main.o

all: $(FLASHER_TARGET)

$(FLASHER_NAME).out: $(OBJECTS)
	@(cd ../$(LIB_DIR) && $(MAKE))
	$(CC) $(LDFLAGS) -o $(FLASHER_NAME).out $^ $(LDLIBS)

main.o: $(INC)/uart.h $(INC)/memory.h

install: $(FLASHER_TARGET)
	$(AVRDUDE) -c $(PROGRAMMERID) -p $(MCU) -P -e -U flash:w:$(FLASHER_TARGET)

clean:
	$(REM) $(REMOPTS) *.o
	$(REM) $(REMOPTS) $(FLASHER_NAME).out
	$(REM) $(REMOPTS) $(FLASHER_NAME).map

mrproper: clean
	$(REM) $(REMOPTS) $(FLASHER_TARGET)

.PHONY: clean mrproper

