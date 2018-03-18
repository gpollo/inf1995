include ../Makefile.common

OBJECTS=main.o

all: $(INTERPRETER_TARGET)

$(INTERPRETER_NAME).out: $(OBJECTS)
	@(cd ../$(LIB_DIR) && $(MAKE))
	$(CC) $(LDFLAGS) -o $(INTERPRETER_NAME).out $^ $(LDLIBS)

main.o: $(INC)/uart.h $(INC)/memory.h $(INC)/moteur.h

install: $(INTERPRETER_TARGET)
	$(AVRDUDE) -c $(PROGRAMMERID) -p $(MCU) -P -e -U flash:w:$(INTERPRETER_TARGET)

clean:
	$(REM) $(REMOPTS) *.o
	$(REM) $(REMOPTS) $(INTERPRETER_NAME).out
	$(REM) $(REMOPTS) $(INTERPRETER_NAME).map

mrproper: clean
	$(REM) $(REMOPTS) $(INTERPRETER_TARGET)

.PHONY: clean mrproper

