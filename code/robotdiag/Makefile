include ../Makefile.common

OBJECTS=main.o

all: $(ROBOTDIAG_TARGET)

$(ROBOTDIAG_NAME).out: $(OBJECTS)
	@(cd ../$(LIB_DIR) && $(MAKE))
	$(CC) $(LDFLAGS) -o $(ROBOTDIAG_NAME).out $^ $(LDLIBS)

main.o: $(INC)/moteur.h

install: $(ROBOTDIAG_TARGET)
	$(AVRDUDE) -c $(PROGRAMMERID) -p $(MCU) -P -e -U flash:w:$(ROBOTDIAG_TARGET)

clean:
	$(REM) $(REMOPTS) *.o
	$(REM) $(REMOPTS) $(ROBOTDIAG_NAME).out
	$(REM) $(REMOPTS) $(ROBOTDIAG_NAME).map

mrproper: clean
	$(REM) $(REMOPTS) $(ROBOTDIAG_TARGET)

.PHONY: clean mrproper

