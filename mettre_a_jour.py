# -*- coding: utf-8 -*-
# Met a jour un module a partir du fichier .glade depose par un camarade.
# Usage : python3 mettre_a_jour.py "recus/FICHIER.glade" MODULE PREFIXE
# Exemple : python3 mettre_a_jour.py "recus/WW_passagers.glade" passagers pass
# - copie toutes ses fenetres dans modules/MODULE.glade
#   (sauf ses fenetres de test : connexion et admin)
# - ajoute le prefixe aux noms automatiques (vbox1, label3...)
# - affiche ses fenetres et leurs sous-onglets
import os, re, sys
import xml.etree.ElementTree as ET

if len(sys.argv) != 4:
    sys.exit('Usage : python3 mettre_a_jour.py "recus/FICHIER.glade" MODULE PREFIXE')
src, module, pre = sys.argv[1], sys.argv[2], sys.argv[3].rstrip("_") + "_"
racine = os.path.dirname(os.path.abspath(__file__))
dst = os.path.join(racine, "modules", module + ".glade")

try:
    r = ET.parse(src).getroot()
except Exception as e:
    sys.exit("ERREUR : impossible de lire %s : %s" % (src, e))
if r.tag != "glade-interface":
    sys.exit("ERREUR : %s a ete cree avec Glade 3, il n'est pas compatible avec Glade 2" % src)

def ignoree(i):
    i = i.lower()
    return "login" in i or i in ("window_admin", "window_administrateur", "window1")

def texte(w):
    for x in w.iter("widget"):
        if x.get("class") == "GtkLabel":
            p = x.find("property[@name='label']")
            if p is not None and p.text:
                return re.sub(r"<[^>]+>", "", p.text).strip()
    return "?"

garde = ET.Element("glade-interface")
for fen in r.findall("widget"):
    fid = fen.get("id")
    if ignoree(fid):
        print("  ignoree (fenetre de test) : %s" % fid)
        continue
    garde.append(fen)
    onglets = []
    for nb in fen.iter("widget"):
        if nb.get("class") == "GtkNotebook":
            for c in nb.findall("child"):
                t = c.find("packing/property[@name='type']")
                if t is not None and t.text == "tab":
                    onglets.append(texte(c.find("widget")))
            break
    print("  fenetre : %-32s %s" % (fid, ("sous-onglets : " + ", ".join(onglets)) if onglets else ""))

auto = re.compile(r"^[a-z]+[0-9]+$")
n = 0
for w in garde.iter("widget"):
    i = w.get("id") or ""
    if auto.match(i):
        w.set("id", pre + i)
        n += 1

os.makedirs(os.path.dirname(dst), exist_ok=True)
with open(dst, "w", encoding="utf-8") as out:
    out.write('<?xml version="1.0" standalone="no"?> <!--*- mode: xml -*-->\n')
    out.write('<!DOCTYPE glade-interface SYSTEM "http://glade.gnome.org/glade-2.0.dtd">\n\n')
    out.write(ET.tostring(garde, encoding="unicode"))
    out.write("\n")
print("OK : %d fenetre(s) dans modules/%s.glade (%d nom(s) prefixe(s) par '%s')"
      % (len(garde.findall("widget")), module, n, pre))
