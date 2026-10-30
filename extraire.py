# -*- coding: utf-8 -*-
# Copie certaines fenetres d'un fichier Glade vers un fichier de module.
# Usage : python3 extraire.py SOURCE.glade DESTINATION.glade fenetre1 fenetre2 ...
# Exemple :
#   python3 extraire.py ~/Projets/carthage/carthage.glade modules/personnel.glade \
#           win_admin_personnel win_form_personnel win_pers_espace
import os, sys
import xml.etree.ElementTree as ET

if len(sys.argv) < 4:
    sys.exit(__doc__ if __doc__ else "Usage : python3 extraire.py SOURCE DESTINATION fenetre1 ...")
src, dst, noms = sys.argv[1], sys.argv[2], sys.argv[3:]
r = ET.parse(os.path.expanduser(src)).getroot()
dest = ET.Element("glade-interface")
for n in noms:
    fen = r.find("widget[@id='%s']" % n)
    if fen is None:
        sys.exit("ERREUR : la fenetre '%s' n'existe pas dans %s" % (n, src))
    dest.append(fen)
dossier = os.path.dirname(os.path.abspath(dst))
os.makedirs(dossier, exist_ok=True)
with open(dst, "w", encoding="utf-8") as out:
    out.write('<?xml version="1.0" standalone="no"?> <!--*- mode: xml -*-->\n')
    out.write('<!DOCTYPE glade-interface SYSTEM "http://glade.gnome.org/glade-2.0.dtd">\n\n')
    out.write(ET.tostring(dest, encoding="unicode"))
    out.write("\n")
print("OK : %d fenetre(s) copiee(s) dans %s" % (len(noms), dst))
