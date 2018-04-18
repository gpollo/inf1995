#!/usr/bin/env python3

import numpy as np

""" Cette fonction calcule le voltage selon la courbe de régression.

    :param invdistance: L'inverse de la distance [1/cm].
    
    :return: Le voltage selon l'inverse de la distance [V].
"""
def get_voltage(invdistance):
    return -1814.9*invdistance**3+259.56*invdistance**2+14.147*invdistance+0.2954

""" Cette fonction convertit un voltage sur un nombre 10 bits.
    
    :param voltage:   Le voltage à convertir [V].
    :param reference: La référence du voltage [V].
    
    :return: La valeur du voltage sur 10 bits.
"""
def get_value(voltage, reference):
    return int((voltage*1024)/reference)

""" Cette fonction convertit l'inverse d'une distance à une distance.

    :param invdistance: L'inverse de la distance [1/cm].
    :param correction:  Un facteur de correction optionel [mm].
    
    :return: La distance calculée [mm].
"""
def get_distance(invdistance, correction):
    return int(10*(1/invdistance-0.42)+correction)

""" Cette fonction génère les valeurs pour le senseur.

    :return: Les valeurs du senseur.
"""
def get_maps():
    mapping = dict()

    # on génère des valeurs pour l'inverse de la distance
    for invdistance in np.linspace(0.01, 0.12, 2048):
        # on calcule le voltage à partir de l'inverse de la distance
        voltage = get_voltage(invdistance)
        
        # on calcule la distance à partir de l'inverse de la distance
        distance = get_distance(invdistance, 0)
        
        # on calcule la valeur 10 bits à partir du voltage
        value = get_value(voltage, 5.0)

        # on ignore les valeurs au-dessous 90 mm
        if distance < 90:
            continue

        # on ignore les valeurs au-dessus de 600mm
        if distance > 700:
            continue

        # le voltage ne devrait pas dépasser cette valeur
        if voltage > 2.8:
            continue

        # on veut le nombre sur 10 bits
        if value < 0 or value > 1023:
            continue

        # on ne reécrit par sur une ancienne valeur
        if value in mapping:
            continue

        # on ajoute la valeur à la list
        mapping[value] = distance
    
    return mapping

""" Cette fonction génère les valeurs textuels du senseur.

    :param mapping: Les valeurs du senseur.
    
    :return: Les valeurs textuels du senseur.
"""
def get_data(mapping):
    # on commence et on n'écrit rien
    printing = False
    
    # on initialise ces variables
    (minimum, maximum) = (None, None)
    
    # on écrit les valeurs
    data = ""
    for i in range(1024):
        if not i  in mapping:
            if printing:
                # à la première valeurinvalid, on arrête d'écrire
                maximum = i-1
                break
            else:
                # on ignore les premières valeurs invalide
                continue
        else:
            if not printing:
                # on commence à écrire des valeurs
                printing = True
                minimum = i
        
        # on ajoute la valeur au données
        data += "    {:6d}, /* {:4d} */ \\\n".format(mapping[i], i)
        
    return (data, minimum, maximum)

if __name__ == '__main__':
    # on génère les valeurs
    mapping = get_maps()
    
    # on obtient les valeurs textuelles
    (data, minimum, maximum) = get_data(mapping)

    # on génère le header
    output = ""
    output += "#ifndef INCLUDE_SENSOR_DATA_H\n"
    output += "#define INCLUDE_SENSOR_DATA_H\n"
    output += "\n"
    output += "#define SENSOR_MIN {}\n".format(minimum)
    output += "#define SENSOR_MAX {}\n".format(maximum)
    output += "\n"
    output += "#define SENSOR_DATA {  \\\n"
    output += data
    output += "}\n"
    output += "\n"
    output += "#endif"

    # on écrit le header
    print(output)
