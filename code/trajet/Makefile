include ../Makefile.common

CFLAGS += -DDELAY=20

OBJECTS=robotdiag.o \
        buzz.o      \
        obstacle.o  \
        state.o     \
        trajet.o    \
        main.o

all: $(TRAJET_TARGET)

$(TRAJET_NAME).out: $(OBJECTS)
	@(cd ../$(LIB_DIR) && $(MAKE))
	$(CC) $(LDFLAGS) -o $(TRAJET_NAME).out $^ $(LDLIBS)

main.o: $(INC)/memory.h \
        trajet.h        \
        robotdiag.h

buzz.o: $(INC)/buzzer.h \
        $(INC)/timer.h  \
        $(INC)/utils.h  \
        buzz.h

state.o: $(INC)/timer.h \
         $(INC)/utils.h \
         robot.h        \
         state.h

obstacle.o: $(INC)/sensor.h \
            robot.h         \
            buzz.h          \
            obstacle.h

install: $(TRAJET_TARGET)
	$(AVRDUDE) -c $(PROGRAMMERID) -p $(MCU) -P -e -U flash:w:$(TRAJET_TARGET)

clean:
	$(REM) $(REMOPTS) *.o
	$(REM) $(REMOPTS) $(TRAJET_NAME).out
	$(REM) $(REMOPTS) $(TRAJET_NAME).map

mrproper: clean
	$(REM) $(REMOPTS) $(TRAJET_TARGET)

.PHONY: clean mrproper

