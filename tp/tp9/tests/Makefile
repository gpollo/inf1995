include ../Makefile.common

OBJECTS=main.o

all: $(TEST_TARGET)

$(TEST_NAME).out: $(OBJECTS)
	@(cd ../$(LIB_DIR) && $(MAKE))
	$(CC) $(LDFLAGS) -o $(TEST_NAME).out $^ $(LDLIBS)

main.o: $(INC)/uart.h $(INC)/memory.h $(INC)/timer.h $(INC)/adc.h

install: $(TEST_TARGET)
	$(AVRDUDE) -c $(PROGRAMMERID) -p $(MCU) -P -e -U flash:w:$(TEST_TARGET)

clean:
	$(REM) $(REMOPTS) *.o
	$(REM) $(REMOPTS) $(TEST_NAME).out
	$(REM) $(REMOPTS) $(TEST_NAME).map

mrproper: clean
	$(REM) $(REMOPTS) $(TEST_TARGET)

.PHONY: clean mrproper

