include ../Makefile.common

OBJECTS=main.o

all: $(TRAJET_TARGET)

$(TRAJET_NAME).out: $(OBJECTS)
	@(cd ../$(LIB_DIR) && $(MAKE))
	$(CC) $(LDFLAGS) -o $(TRAJET_NAME).out $^ $(LDLIBS)

main.o: $(INC)/uart.h $(INC)/sensor.h

install: $(TRAJET_TARGET)
	$(AVRDUDE) -c $(PROGRAMMERID) -p $(MCU) -P -e -U flash:w:$(TRAJET_TARGET)

clean:
	$(REM) $(REMOPTS) *.o
	$(REM) $(REMOPTS) $(TRAJET_NAME).out
	$(REM) $(REMOPTS) $(TRAJET_NAME).map

mrproper: clean
	$(REM) $(REMOPTS) $(TRAJET_TARGET)

.PHONY: clean mrproper

