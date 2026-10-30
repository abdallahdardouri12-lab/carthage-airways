# -*- coding: utf-8 -*-
# Ajoute un prefixe aux noms automatiques de Glade (vbox1, label3, button2...)
# pour eviter les doublons entre modules. Les noms choisis a la main
# (entry_nom, btn_ajouter...) ne sont pas touches.
# Usage : python3 prefixer.py modules/personnel.glade pers
import re, sys
import xml.etree.ElementTree as ET

if len(sys.argv) != 3:
    sys.exit("Usage : python3 prefixer.py modules/MODULE.glade prefixe")
fichier, pre = sys.argv[1], sys.argv[2].rstrip("_") + "_"
arbre = ET.parse(fichier)
auto = re.compile(r"^[a-z]+[0-9]+$")       # vbox1, hbuttonbox2, scrolledwindow1...
n = 0
for w in arbre.getroot().iter("widget"):
    i = w.get("id")
    if auto.match(i) and not i.startswith(pre):
        w.set("id", pre + i)
        n += 1
with open(fichier, "w", encoding="utf-8") as out:
    out.write('<?xml version="1.0" standalone="no"?> <!--*- mode: xml -*-->\n')
    out.write('<!DOCTYPE glade-interface SYSTEM "http://glade.gnome.org/glade-2.0.dtd">\n\n')
    out.write(ET.tostring(arbre.getroot(), encoding="unicode"))
    out.write("\n")
print("OK : %d nom(s) automatique(s) renomme(s) avec le prefixe '%s'" % (n, pre))
