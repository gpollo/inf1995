#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <son.h>
#include <memory.h>
#include <moteur.h>
#include <del.h>
#include <uart.h>

#ifdef DEBUG
    #define uart_debug(format,...) uart_printf(format,##__VA_ARGS__)
#else
    #define uart_debug(format,...)
#endif

#define PAGE_SIZE 128
uint8_t code[PAGE_SIZE];

struct loop {
    uint16_t addr;
    uint8_t page;
    uint8_t cntr;
};

int main() {
	/* on initialise le uart */
    uart_init();
    uart_debug("\n\r\n\r");
	
    /* on initialise le son */
    son_init();

    /* on initialise la mémoire */
    memory_init();

    /* on initialise les moteurs */
    moteur_init();
	
	/* on initialise les dels */
    del_init();
	
	/* on joue le son de départ */
	son_depart();

    /* ce flag dit si le programme roule ou non */
    uint8_t running = 0;

    /* structure de données pour la boucle */
    struct loop loop = {
        .addr = 0,
        .page = 0,
        .cntr = 0,
    };
        
	/* la table de correspondance des instructions */
	void** instructions[256];
    
	/* les instructions inconnues sont considérées comme des NOP */
	for(uint16_t i = 0; i < sizeof(uint8_t); i++) instructions[i] = &&CMD_NOP;
	
	/* on initialise la table d'instructions */
	instructions[0b00000001] = &&CMD_DBT;
	instructions[0b00000010] = &&CMD_ATT;
	instructions[0b01000100] = &&CMD_DAL;
	instructions[0b01000101] = &&CMD_DET;
	instructions[0b01001000] = &&CMD_SGO;
	instructions[0b00001001] = &&CMD_SAR;
	instructions[0b01100000] = &&CMD_MAR;
	instructions[0b01100001] = &&CMD_MAR;
	instructions[0b01100010] = &&CMD_MAV;
	instructions[0b01100011] = &&CMD_MRE;
	instructions[0b01100100] = &&CMD_TRD;
	instructions[0b01100101] = &&CMD_TRG;
	instructions[0b11000000] = &&CMD_DBC;
	instructions[0b11000001] = &&CMD_FBC;
	instructions[0b11111111] = &&CMD_FIN;

    /* la page actuellement lue de la mémoire  */
    uint8_t page = 0;

    /* on lit une page de la mémoire */
    memory_read_block(0, (page++)*PAGE_SIZE, PAGE_SIZE, code);

    /* l'adresse d'exécution dans la mémoire */
    uint16_t addr = 0;
    
    /* on boucle jusqu'à la fin du programme */
    while(1) {
        /* on lit une autre page si nécessaire */
        if(addr >= PAGE_SIZE) {
            memory_read_block(0, (page++)*PAGE_SIZE, PAGE_SIZE, code);
            addr = 0;
        }

        /* on obtient la prochaine instruction */
        uint8_t cmd = code[addr++];

        /* on lit une autre page si nécessaire */
        if(addr >= PAGE_SIZE) {
            memory_read_block(0, (page++)*PAGE_SIZE, PAGE_SIZE, code);
            addr = 0;
        }

        /* on obtient l'adresse du jump  */
        void* jump = instructions[cmd];

        /* on jump seulement le programme a débuté */
        if(running || (jump == (&&CMD_DBT))) goto *jump;

        /* sinon, on avance dans la mémoire */
        continue;

CMD_NOP:
        /* on ne fait rien */
        uart_debug("nop\n\r");
        continue;

CMD_DBT:
        /* on active le flag qui dit que le programme s'exécute */
        running = 1;
        uart_debug("début\n\r");
        addr++;
        continue;

CMD_ATT:
        /* on attend 25 millisecondes fois le nombre en paramètre */
        uart_debug("attendre param=%d\n\r", code[addr]);
        for(uint8_t i = code[addr++]; i > 0; i--) _delay_ms(25);
        continue;

CMD_DAL:
        /* on allume la del*/
		uart_debug("allume DEL param=%d\n\r", code[addr]);
		del_on(code[addr++]);
        continue;

CMD_DET:
		/* on éteint la DEL */
		uart_debug("éteindre DEL param=%d\n\r", code[addr]);
		del_off(code[addr++]);
        continue;

CMD_SGO:
        /* on joue le son */
        uart_debug("jouer son param=%d\n\r", code[addr]);
        son_jouer(code[addr++]);
        continue;

CMD_SAR:
		/* on arrête le son */
		uart_debug("arrêt son\n\r");
		son_arreter();
		addr++;
        continue;

CMD_MAR:
        /* on arrête les moteurs */
        uart_debug("arrêt moteur\n\r");
        moteur_arreter();
        addr++;
        continue;

CMD_MAV:
        /* on active les roues vers l'avant */
        uart_debug("avance param=%d\n\r", code[addr]);
        moteur_avancer(code[addr++]);
        continue;

CMD_MRE:
        /* on active les roues vers l'arrière */
        uart_debug("reculer param=%d\n\r", code[addr]);
        moteur_reculer(code[addr++]);
        continue;

CMD_TRD:
        /* on tourne le robot vers la droite */
        uart_debug("tourner à droite\n\r");
        moteur_tourner_droite();
        addr++;
        continue;

CMD_TRG:
        /* on tourne le robot vers la gauche */
        uart_debug("tourner à gauche\n\r");
        moteur_tourner_gauche();
        addr++;
        continue;

CMD_DBC:
        /* on sauvegarde l'adresse de début de la boucle et son compteur */
        uart_debug("début boucle param=%d\n\r", code[addr]);
        loop.cntr = code[addr++];
        loop.addr = addr;
        loop.page = page;
        continue;

CMD_FBC:
        /* on ne recommence pas la boucle lorsque le compteur est à zéro */
        uart_debug("fin boucle\n\r");
        if(loop.cntr == 0) {
				addr++;
				continue;
		}

        /* on décrémente la boucle */
        loop.cntr--;

        /* on lit à nouveau la page si nécessaire */
        if(loop.page != page) {
            page = loop.page;
            memory_read_block(0, (page++)*PAGE_SIZE, PAGE_SIZE, code);
        }
        
        /* on retourne à l'adresse de départ de la boucle */
        addr = loop.addr;
        //addr++;
        continue;

CMD_FIN:
        /* on quitte la boucle */
        uart_debug("fin programme\n\r");
        running = 0;
        break;
    }

    while(1);
}
