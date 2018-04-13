include ../Makefile.common

OBJECTS=moteur.o uart.o memory.o adc.o timer.o son.o del.o sensor.o buzzer.o interrupt.o

all: $(LIB_TARGET)

$(LIB_TARGET): $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIB_TARGET) $(OBJECTS)

moteur.o: $(INC)/moteur.h

uart.o: $(INC)/uart.h

memory.o: $(INC)/memory.h

adc.o: $(INC)/adc.h

timer.o: $(INC)/timer.h

son.o: $(INC)/son.h

del.o: $(INC)/del.h

sensor.o : $(INC)/sensor.h

buzzer.o : $(INC)/buzzer.h

interrupt.o : $(INC)/interrupt.h

clean:
	$(REM) $(REMOPTS) *.o

mrproper: clean
	$(REM) $(REMOPTS) -f $(LIB_TARGET)

.PHONY: clean mrproper
