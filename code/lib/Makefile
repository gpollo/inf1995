include ../Makefile.common
include ../Makefile.clean

EXEC=libstatique
OBJECTS=moteur.o memoire.o can.o

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(AR) $(ARFLAGS) $(EXEC).a $(OBJECTS)

moteur.o: $(INC)/moteur.h

memoire.o: $(INC)/memoire.h

can.o: $(INC)/can.h