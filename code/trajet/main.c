#include <memory.h>
#include "trajet.h"


int main(void) {
    /* on a besoins de la mémoire */
    memory_init();

    /* on lit le premier octet */
    uint8_t byte = memory_read_byte(0, 0x0);

    /* on flip l'octet */
    byte = (byte == 0) ? 0x1 : 0x0;

    /* on reécrit l'octet en mémoire */
    memory_write_byte(0, 0x0, byte);

    /* on exécute le bon programme selon le bon octet */
    if(byte) {
        trajet_main();
    } else {
        /* TODO: exécuter le robodiag*/
    }
}
