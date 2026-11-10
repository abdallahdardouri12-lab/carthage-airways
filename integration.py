# -*- coding: utf-8 -*-
# Carthage Airways - reunit commun.glade + modules/*.glade dans carthage.glade
# Usage (depuis le dossier du depot) : python3 integration.py
import os, glob, sys, re, unicodedata
import xml.etree.ElementTree as ET

racine = os.path.dirname(os.path.abspath(__file__))
sources = []
if os.path.exists(os.path.join(racine, "commun.glade")):
    sources.append(os.path.join(racine, "commun.glade"))
sources += sorted(glob.glob(os.path.join(racine, "modules", "*.glade")))
if not sources:
    sys.exit("ERREUR : aucun fichier dans modules/ ni commun.glade")

resultat = ET.Element("glade-interface")
fenetres_vues = {}     # nom de fenetre -> fichier
noms_globaux = {}      # nom de widget -> (fichier, fenetre)
erreurs = []
avertissements = []

for f in sources:
    nom = os.path.relpath(f, racine)
    try:
        r = ET.parse(f).getroot()
    except ET.ParseError as e:
        erreurs.append("%s est abime : %s" % (nom, e))
        continue
    if r.tag != "glade-interface":
        erreurs.append("%s n'est pas au format Glade 2 (cree avec Glade 3 ?)" % nom)
        continue
    fenetres = r.findall("widget")
    # noms invalides en C (accents, espaces, apostrophes...) -> corriges
    for w in r.iter("widget"):
        ancien = w.get("id") or ""
        nouveau = unicodedata.normalize("NFKD", ancien).encode("ascii", "ignore").decode()
        nouveau = re.sub(r"[^A-Za-z0-9_]", "_", nouveau)
        if nouveau == "" or nouveau[0].isdigit():
            nouveau = "w_" + nouveau
        if nouveau != ancien:
            w.set("id", nouveau)
            print("  nom corrige dans %s : %r -> %r" % (nom, ancien, nouveau))
    for fen in fenetres:
        fid = fen.get("id")
        if fid in fenetres_vues:
            erreurs.append("la fenetre '%s' existe dans %s ET dans %s" % (fid, fenetres_vues[fid], nom))
            continue
        fenetres_vues[fid] = nom
        dans_fenetre = set()
        for w in fen.iter("widget"):
            i = w.get("id")
            if i in dans_fenetre:
                erreurs.append("le nom '%s' est utilise 2 fois dans la fenetre '%s' (%s)" % (i, fid, nom))
            dans_fenetre.add(i)
            if i in noms_globaux and noms_globaux[i][1] != fid:
                avertissements.append("'%s' existe dans '%s' et dans '%s' (accepte)" % (i, noms_globaux[i][1], fid))
            noms_globaux.setdefault(i, (nom, fid))
        resultat.append(fen)
    print("  %-28s %d fenetre(s) : %s" % (nom, len(fenetres),
          ", ".join(w.get("id") for w in fenetres)))

if avertissements:
    print("\nRemarque : %d nom(s) de widget repete(s) dans des fenetres differentes (sans danger)." % len(avertissements))

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
print("\nOK : carthage.glade reconstruit (%d fenetres). Ouvre-le dans Glade puis clique sur Generer." % len(fenetres_vues))
