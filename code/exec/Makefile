include ../Makefile.common

OBJECTS=main.o

all: $(MAIN_TARGET)

$(MAIN_NAME).out: $(OBJECTS)
	@(cd ../$(LIB_DIR) && $(MAKE))
	$(CC) $(LDFLAGS) -o $(MAIN_NAME).out $^ $(LDLIBS)

main.o: $(INC)/moteur.h

install: $(MAIN_TARGET)
	$(AVRDUDE) -c $(PROGRAMMERID) -p $(MCU) -P -e -U flash:w:$(MAIN_TARGET)

clean:
	$(REM) $(REMOPTS) *.o
	$(REM) $(REMOPTS) $(MAIN_NAME).out
	$(REM) $(REMOPTS) $(MAIN_NAME).map

mrproper: clean
	$(REM) $(REMOPTS) $(MAIN_TARGET)

.PHONY: clean mrproper

