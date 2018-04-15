#ifndef INCLUDE_OBSTACLE_H
#define INCLUDE_OBSTACLE_H

/**
 * Cette énumération définie un obstacle.
 */
enum obstacle {
    UNKNOWN,
    POTEAU,
    MUR,
};

/**
 * Cette fonction détermine si le robot détecte un obstacle ou non. Pour ce
 * faire, cette fonction s'attend d'être appelée à chaque #DELAI millisecondes.
 *
 * @param robot Un pointeur vers le robot.
 *
 * @return Le type d'obstacle détecté.
 */
enum obstacle detect(struct robot* robot);

#endif
