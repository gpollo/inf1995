include ../Makefile.common

CFLAGS += -DDELAY=20

OBJECTS=robotdiag.o \
        buzz.o      \
        obstacle.o  \
        state.o     \
        trajet.o    \
        robot.o     \
        main.o

all: $(TRAJET_TARGET)

$(TRAJET_NAME).out: $(OBJECTS)
	@(cd ../$(LIB_DIR) && $(MAKE))
	$(CC) $(LDFLAGS) -o $(TRAJET_NAME).out $^ $(LDLIBS)

robotdiag.o: $(INC)/uart.h      \
             $(INC)/timer.h     \
             $(INC)/adc.h       \
             $(INC)/interrupt.h \
             $(INC)/del.h       \
             $(INC)/moteur.h    \
             $(INC)/sensor.h    \
             message.h          \
             robotdiag.h

buzz.o: $(INC)/buzzer.h \
        $(INC)/timer.h  \
        $(INC)/utils.h  \
        buzz.h

state.o: $(INC)/timer.h  \
         $(INC)/sensor.h \
         $(INC)/uart.h   \
         $(INC)/moteur.h \
         $(INC)/utils.h  \
         robot.h         \
         obstacle.h      \
         state.h

trajet.o: $(INC)/uart.h      \
          $(INC)/utils.h     \
          $(INC)/interrupt.h \
          $(INC)/sensor.h    \
          $(INC)/moteur.h    \
          $(INC)/timer.h     \
          $(INC)/del.h       \
          $(INC)/buzzer.h    \
          robot.h            \
          buzz.h             \
          obstacle.h         \
          state.h            \
          trajet.h

obstacle.o: $(INC)/sensor.h \
            robot.h         \
            buzz.h          \
            obstacle.h

robot.o: $(INC)/moteur.h \
         $(INC)/sensor.h \
         $(INC)/utils.h  \
         robot.h

main.o: $(INC)/memory.h \
        trajet.h        \
        robotdiag.h

install: $(TRAJET_TARGET)
	$(AVRDUDE) -c $(PROGRAMMERID) -p $(MCU) -P -e -U flash:w:$(TRAJET_TARGET)

clean:
	$(REM) $(REMOPTS) *.o
	$(REM) $(REMOPTS) $(TRAJET_NAME).out
	$(REM) $(REMOPTS) $(TRAJET_NAME).map

mrproper: clean
	$(REM) $(REMOPTS) $(TRAJET_TARGET)

.PHONY: clean mrproper

