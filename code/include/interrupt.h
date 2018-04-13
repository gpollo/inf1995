#ifndef INCLUDE_INTERRUPT_H
#define INCLUDE_INTERRUPT_H

#include <stdint.h>

/**
 * Cette méthode initialise les interruptions par bouton
 */
void interruption_init(void (*cb)(uint8_t, void*), void* cbdata);

#endif
