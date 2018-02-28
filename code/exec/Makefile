include ../Makefile.common
include ../Makefile.clean

EXEC=robot
OBJECTS=main.o

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(AR) $(ARFLAGS) $(EXEC).a $(OBJECTS)

main.o: $(INC)/motor.h



