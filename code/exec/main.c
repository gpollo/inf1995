#include <buzzer.h>
#include <util/delay.h>

int main(void) {
	buzzer_init();
	buzzer_jouer();
	_delay_ms(2000);
	buzzer_arreter();
    while(1);
}
