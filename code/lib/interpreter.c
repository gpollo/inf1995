#include <stdlib.h>
#include <interpreter.h>

void** instructions[256];

struct loop {
    uint16_t addr;
    uint8_t cntr;
};

void execute(uint8_t* code, uint16_t len) {
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

    /* l'adresse d'exécution dans la mémoire */
    uint16_t addr = 0;

    /* ce flag dit si le programme roule ou non */
    uint8_t running = 0;

    /* structure de données pour une boucle */
    struct loop loop = {
        .addr = 0,
        .cntr = 0,
    };

    /* on boucle jusqu'à la fin du programme */
    while(1) {
        if(addr >= len) break;

        /* on obtient la prochaine instruction */
        uint8_t cmd = code[addr++];

        /* on obtient l'adresse du jump  */
        void* jump = instructions[cmd];

        /* on jump seulement le programme a débuté */
        if(running || jump == &&CMD_DBT) goto *jump;

        /* sinon, on avance dans la mémoire */
        continue;

CMD_DBT:
        /* on active le flag qui dit que le programme s'exécute */
        running = 1;
        continue;

CMD_ATT:

        continue;

CMD_DAL:

        continue;

CMD_DET:

        continue;

CMD_SGO:

        continue;

CMD_SAR:

        continue;

CMD_MAR:

        continue;

CMD_MAV:

        continue;

CMD_MRE:

        continue;

CMD_TRD:

        continue;

CMD_TRG:

        continue;

CMD_DBC:
        /* on sauvegarde l'adresse de début de la boucle et son compteur */
        loop.cntr = addr++;
        loop.addr = addr;
        continue;

CMD_FBC:
        /* on quitte la boucle ou on décremente son compteur */
        if(loop.cntr > 0)
            addr = loop.addr;
        else
            loop.cntr--;
        continue;

CMD_FIN:
        running = 0;
        break;
    }

}
