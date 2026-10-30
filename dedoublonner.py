# -*- coding: utf-8 -*-
# Renomme les widgets qui portent le meme nom DANS LA MEME fenetre
# (le 1er garde son nom, les suivants recoivent _2, _3...).
# Usage : python3 dedoublonner.py modules/avions.glade
import sys
import xml.etree.ElementTree as ET

if len(sys.argv) != 2:
    sys.exit("Usage : python3 dedoublonner.py modules/MODULE.glade")
f = sys.argv[1]
arbre = ET.parse(f)
racine = arbre.getroot()
tous = set(w.get("id") for w in racine.iter("widget"))
for fen in racine.findall("widget"):
    vus = set()
    for w in fen.iter("widget"):
        i = w.get("id")
        if i in vus:
            n = 2
            while "%s_%d" % (i, n) in tous:
                n += 1
            nouveau = "%s_%d" % (i, n)
            w.set("id", nouveau)
            tous.add(nouveau)
            print("Fenetre %s : 2e '%s' renomme en '%s'" % (fen.get("id"), i, nouveau))
        else:
            vus.add(i)
with open(f, "w", encoding="utf-8") as out:
    out.write('<?xml version="1.0" standalone="no"?> <!--*- mode: xml -*-->\n')
    out.write('<!DOCTYPE glade-interface SYSTEM "http://glade.gnome.org/glade-2.0.dtd">\n\n')
    out.write(ET.tostring(racine, encoding="unicode"))
    out.write("\n")
print("OK")
