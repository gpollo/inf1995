include ../Makefile.common

OBJECTS=main.o
ROUTINE=routine.txt
BYTECODE=bytecode.o

all: $(FLASHER_TARGET)

$(FLASHER_NAME).out: $(OBJECTS)
	@(cd ../$(LIB_DIR) && $(MAKE))
	$(CC) $(LDFLAGS) -o $(FLASHER_NAME).out $^ $(LDLIBS)

main.o: $(INC)/uart.h $(INC)/memory.h

install: $(FLASHER_TARGET)
	$(AVRDUDE) -c $(PROGRAMMERID) -p $(MCU) -P -e -U flash:w:$(FLASHER_TARGET)

$(BYTECODE): $(ROUTINE)
	$(PROGMEM) -o $(BYTECODE) $(ROUTINE)

flash: $(BYTECODE)
	$(SERIEVIAUSB) -e -f $(BYTECODE)

clean:
	$(REM) $(REMOPTS) *.o
	$(REM) $(REMOPTS) $(INTERPRETER_NAME).out
	$(REM) $(REMOPTS) $(INTERPRETER_NAME).map

mrproper: clean
	$(REM) $(REMOPTS) $(INTERPRETER_TARGET)
