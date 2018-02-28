include ../Makefile.common
include ../Makefile.clean

EXEC=libstatique
OBJECTS=moteur.o

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(AR) $(ARFLAGS) $(EXEC).a $(OBJECTS)

main.o: $(INC)/moteur.h
