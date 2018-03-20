#!/usr/bin/env python
# -*- coding: utf-8 -*-

# début du programme
print("dbt;")

# on ferme toute les DELs
print("det 255;")

# première pattern de DELs
print("dbc 10;")
for i in range(0, 7):
    print("dal {};".format(1 << i))
    print("att 1;")
    print("det {};".format(1 << i))
for i in range(0, 7):
    print("dal {};".format(128 >> i))
    print("att 1;")
    print("det {};".format(128 >> i))
print("fbc;")

# deuxième pattern de DELs (le compteur)
for i in range(0,255):
    print("dal {};".format(i))
    print("att 1;")
    print("det {};".format(i))

# troisième pattern de DELs
print("dbc 10;")
for i in range(0,3):
    print("dal {};".format((1 << i) | (128 >> i)))
    print("att 1;")
    print("det {};".format((1 << i) | (128 >> i)))
for i in range(0,3):
    print("dal {};".format((8 >> i) | (16 << i)))
    print("att 2;")
    print("det {};".format((8 >> i) | (16 << i)))
print("fbc;")

# quatrième pattern de DELs
print("dbc 10;")
for i in range(0,3):
    print("dal {};".format(0b0101101))
    print("att 3;")
    print("det {};".format(0b1010010))
print("fbc;")

# on ferme toute les DELs
print("det 255;")

# test des moteur
print("dbc 1;")

# tourne à gauche
print("trg;")

# un beep
print("sgo 50;")
print("att 20;")
print("sar;")

# tourne à droite
print("trd;")

# un beep
print("sgo 50;")
print("att 20;")
print("sar;")

# on avance
print("mav 200;")
print("att 100;")
print("mar;")

# un beep
print("sgo 50;")
print("att 20;")
print("sar;")

# on recule
print("mre 200;")
print("att 20;")
print("mar;")

# un beep
print("sgo 50;")
print("att 20;")
print("sar;")

# fin du test des moteurs
print("fbc;")

# fin du programme 
print("fin;")

