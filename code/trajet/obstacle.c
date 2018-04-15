#include <stdint.h>
#include <sensor.h>

#include "robot.h"
#include "buzz.h"
#include "obstacle.h"

/* le temps minimum qu'un capteur devrait capter un poteau */
#ifndef TIME_POTEAU_MIN
    #define TIME_POTEAU_MIN 300
#endif

/* le temps maximum qu'un capteur devrait capter un poteau */
#ifndef TIME_POTEAU_MAX
    #define TIME_POTEAU_MAX 600
#endif

/* le temps minimum que devrait durer un mur */
#ifndef TIME_MUR_MIN
    #define TIME_MUR_MIN 1000
#endif

/* le temps limit pour empêcher un overflow au compteur*/
#ifndef TIME_LIMIT
    #define TIME_LIMIT 5000
#endif

#define IS_POTEAU(other, time, obstacle) {                              \
    if((other).capting) {                                               \
        /* si l'autre capteur capte, on augmente le temps de captage */ \
        (time) = LIMIT((time)+DELAY, TIME_LIMIT);                       \
                                                                        \
        /* si le temps de captage est élevé, c'est un mur */            \
        if((time) > TIME_MUR_MIN) (obstacle) = MUR;                     \
    } else {                                                            \
        /* si on ne capte plus, on regarde si c'est un poteau */        \
        if((TIME_POTEAU_MIN < (time)) && ((time) < TIME_POTEAU_MAX)) {  \
            (obstacle) = POTEAU;                                        \
            buzz();                                                     \
        }                                                               \
                                                                        \
        /* on reset le temps de captage lorsqu'on ne capte rien */      \
        (time) = 0;                                                     \
    }                                                                   \
}

enum obstacle detect(struct robot* robot) {
    /* on obtient les capteurs */
    struct capteurs* capteurs = &(robot->capteurs);

    /* par défaut, on assume que ne sait pas c'est quoi l'obstacle */
    enum obstacle obstacle = UNKNOWN;

    switch(robot->mur) {
    /* on regarde s'il y a un poteau à gauche */
    case DROITE:
        IS_POTEAU(capteurs->gauche, robot->time, obstacle);
        break;

    /* on regarde s'il y a un poteau à droite */
    case GAUCHE:
        IS_POTEAU(capteurs->droit, robot->time, obstacle);
        break;

    /* ce case ne devrait pas être évoqué */
    default:
        break;
    }

    return obstacle;
}