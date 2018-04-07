include ../Makefile.common

OBJECTS=main.o   \
        trajet.o \
        robotdiag.o

all: $(TRAJET_TARGET)

$(TRAJET_NAME).out: $(OBJECTS)
	@(cd ../$(LIB_DIR) && $(MAKE))
	$(CC) $(LDFLAGS) -o $(TRAJET_NAME).out $^ $(LDLIBS)

main.o: $(INC)/memory.h \
        trajet.h        \
	robotdiag.h

trajet.o: $(INC)/utils.h  \
          $(INC)/sensor.h \
          $(INC)/moteur.h \
          $(INC)/adc.h    \
          $(INC)/uart.h   \
          $(INC)/timer.h  \
          $(INC)/del.h    \
          $(INC)/buzzer.h
          
robotdiag.o: $(INC)/uart.h   \
             $(INC)/timer.h  \
             $(INC)/moteur.h \
             $(INC)/adc.h    \
             $(INC)/del.h    \
             $(INC)/sensor.h \
	     message.h
		  
install: $(TRAJET_TARGET)
	$(AVRDUDE) -c $(PROGRAMMERID) -p $(MCU) -P -e -U flash:w:$(TRAJET_TARGET)

clean:
	$(REM) $(REMOPTS) *.o
	$(REM) $(REMOPTS) $(TRAJET_NAME).out
	$(REM) $(REMOPTS) $(TRAJET_NAME).map

mrproper: clean
	$(REM) $(REMOPTS) $(TRAJET_TARGET)

.PHONY: clean mrproper

