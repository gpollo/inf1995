include ../Makefile.common

OBJECTS=moteur.o uart.o memory.o adc.o timer.o son.o

all: $(LIB_TARGET)

$(LIB_TARGET): $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIB_TARGET) $(OBJECTS)

moteur.o: $(INC)/moteur.h

uart.o: $(INC)/uart.h

memory.o: $(INC)/memory.h

adc.o: $(INC)/adc.h

timer.o: $(INC)/timer.h

son.o: $(INC)/son.h

mpu6050.o: $(INC)/mpu6050.h

clean:
	$(REM) $(REMOPTS) *.o

mrproper: clean
	$(REM) $(REMOPTS) -f $(LIB_TARGET)

.PHONY: clean mrproper
