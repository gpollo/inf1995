include ../Makefile.common
include ../Makefile.clean

EXEC=libstatique
OBJECTS=moteur.o memoire.o

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(AR) $(ARFLAGS) $(EXEC).a $(OBJECTS)

main.o: $(INC)/moteur.h

memoire.o: $(INC)/memoire.h
