include ../Makefile.common
include ../Makefile.clean

EXEC=libstatique
OBJECTS=moteur.o uart.o memoire.o adc.o

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(AR) $(ARFLAGS) $(EXEC).a $(OBJECTS)

moteur.o: $(INC)/moteur.h

uart.o: $(INC)/uart.h

memoire.o: $(INC)/memoire.h

adc.o: $(INC)/adc.h
