

#ifndef MEMOIRE_24_H
#define MEMOIRE_24_H

#include <avr/io.h>
#include <util/twi.h>
class Memoire24CXXX

{

public:


// le constructeur appelle init() decrit plus bas

Memoire24CXXX();

~Memoire24CXXX();


// procedure d'initialisation appelee par le constructeur

// a appeler avant des lectures ou des ecritures

void init();

// la procedure init() initialize a zero le "memory bank".

// appeler cette methode uniquement si l'adresse doit changer

static uint8_t choisir_banc(const uint8_t banc);


// deux variantes pour la lecture:

// une donnee a la fois

uint8_t lecture(const uint16_t adresse, uint8_t *donnee);

// bloc de donnees : longueur doit etre de 127 et moins

uint8_t lecture(const uint16_t adresse, uint8_t *donnee,

const uint8_t longueur);


// deux variantes pour l'ecriture:

// une donnee a la fois

uint8_t ecriture(const uint16_t adresse, const uint8_t donnee);

// bloc de donnees : longueur doit etre de 127 et moins

uint8_t ecriture(const uint16_t adresse, uint8_t *donnee,

const uint8_t longueur);


private:

// pour l'ecriture

uint8_t ecrire_page(const uint16_t adresse, uint8_t *donnee,

const uint8_t longueur);


private:

// donnees membres

static uint8_t m_adresse_peripherique;

const uint8_t PAGE_SIZE;

};
#endif
