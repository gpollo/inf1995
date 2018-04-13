#ifndef INCLUDE_UTILS_H
#define INCLUDE_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Cette macro ajuste un word big-endian en un word little-endian.
 *
 * @param n L'octet à convertir.
 */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define FLIP_WORD(n) ((((n) & 0x00FF) << 8) | (((n) & 0xFF00) >> 8));
#endif

/**
 * Cette macro limite la valeur d'une variable.
 *
 * @param value La variable à limitée.
 * @param limit La valeur maximale de la variable.
 *
 * @return La valeur limitée.
 */
#define LIMIT(value, limit) (               \
    ((value) > (limit)) ? (limit) : (value) \
)

/* on définie la valeur FALSE si nécessaire */
#ifndef FALSE
    #define FALSE 0
#endif

/* on définie la valeur TRUE si nécessaire */
#ifndef TRUE
    #define TRUE 1
#endif

/* on définie la valeur OK si nécessaire */
#ifndef OK
    #define OK 1
#endif

/* on définie la valeur FAIL si nécessaire */
#ifndef FAIL
    #define FAIL 0
#endif

/** Cette macro définit une direction possible. */
enum direction {
    /** Aucune direction. */
    AUCUNE,
    /** La direction vers la droite. */
    DROITE,
    /** La direction vers la gauche. */ 
    GAUCHE,
};

/** Cette macro peut être utilisée pour enlever un warning. */
#define UNUSED(x) (void)(x)

#ifdef __cplusplus
}
#endif

#endif
