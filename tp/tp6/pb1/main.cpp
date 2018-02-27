/*
 * +---------+----------+---------+--------+
 * |  État   |  Bouton  |  État   | Sortie |
 * | Présent | Poussoir | Suivant |  LED   |
 * +---------+----------+---------+--------+
 * | INIT    | relaché  |  INIT   | rouge  |
 * | INIT    | appuyer  |  E1     | ambre  |
 * | E1      | appuyer  |  E1     | ambre  |
 * | E1      | relaché  |  E2     | vert   |
 * | E2      | relaché  |  E2     | vert   |
 * | E2      | appuyer  |  E3     | rouge  |
 * | E3      | appuyer  |  E3     | rouge  |
 * | E3      | relaché  |  E4     | eteint |
 * | E4      | relaché  |  E4     | eteint |
 * | E4      | appuyer  |  E5     | vert   |
 * | E5      | appuyer  |  E5     | vert   |
 * | E5      | relaché  |  INIT   | rouge  |
 * +---------+----------+---------+--------+
 */
#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "memoire_24.h"

#define LED_OFF   0x0
#define LED_RED   0x1
#define LED_GREEN 0x2

#define BUFFER_SIZE 127

using namespace std;

int main() {
    char* msg = "*E*C*O*L*E* *P*O*L*Y*T*E*C*H*N*I*Q*U*E*";

    Memoire24CXXX flash;
    flash.ecriture(0x0, (uint8_t*) msg, sizeof(msg));

    char buf[BUFFER_SIZE];
    flash.lecture(0x0, (uint8_t*) buf, sizeof(msg));
    
    DDRB = 0xff;
    PORTB = LED_GREEN;
    
    for(uint8_t i = 0; i < sizeof(msg); ++i){
        if(msg[i] != buf[i]){
            PORTB = LED_RED; 
            break;
        }
    }

    return 0;
}
