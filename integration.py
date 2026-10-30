# -*- coding: utf-8 -*-
# Carthage Airways - reunit commun.glade + modules/*.glade dans carthage.glade
# Usage (depuis le dossier du depot) : python3 integration.py
import os, glob, sys
import xml.etree.ElementTree as ET

racine = os.path.dirname(os.path.abspath(__file__))
sources = []
if os.path.exists(os.path.join(racine, "commun.glade")):
    sources.append(os.path.join(racine, "commun.glade"))
sources += sorted(glob.glob(os.path.join(racine, "modules", "*.glade")))
if not sources:
    sys.exit("ERREUR : aucun fichier dans modules/ ni commun.glade")

resultat = ET.Element("glade-interface")
deja_vus = {}          # id -> fichier
erreurs = []

for f in sources:
    nom = os.path.relpath(f, racine)
    try:
        r = ET.parse(f).getroot()
    except ET.ParseError as e:
        erreurs.append("%s est abime : %s" % (nom, e))
        continue
    fenetres = r.findall("widget")
    for fen in fenetres:
        for w in fen.iter("widget"):
            i = w.get("id")
            if i in deja_vus:
                erreurs.append("le nom '%s' existe dans %s ET dans %s" % (i, deja_vus[i], nom))
            else:
                deja_vus[i] = nom
        resultat.append(fen)
    print("  %-28s %d fenetre(s) : %s" % (nom, len(fenetres),
          ", ".join(w.get("id") for w in fenetres)))

if erreurs:
    print("\nINTEGRATION ANNULEE, a corriger d'abord :")
    for e in erreurs:
        print("  -", e)
    sys.exit(1)

sortie = os.path.join(racine, "carthage.glade")
with open(sortie, "w", encoding="utf-8") as out:
    out.write('<?xml version="1.0" standalone="no"?> <!--*- mode: xml -*-->\n')
    out.write('<!DOCTYPE glade-interface SYSTEM "http://glade.gnome.org/glade-2.0.dtd">\n\n')
    out.write(ET.tostring(resultat, encoding="unicode"))
    out.write("\n")
ET.parse(sortie)
print("\nOK : carthage.glade reconstruit (%d widgets). Ouvre-le dans Glade puis clique sur Generer." % len(deja_vus))
