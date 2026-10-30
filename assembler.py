# -*- coding: utf-8 -*-
# Carthage Airways - regroupe plusieurs fenetres dans UNE fenetre a onglets
# (un onglet par module), d'apres le fichier espaces.txt.
# A lancer APRES integration.py :
#     python3 integration.py
#     python3 assembler.py
import os, sys, re
import xml.etree.ElementTree as ET

racine = os.path.dirname(os.path.abspath(__file__))
projet = os.path.join(racine, "carthage.glade")
config = os.path.join(racine, "espaces.txt")
T = "Fri, 30 Oct 2026 12:00:00 GMT"

def prop(parent, nom, val, trad=False):
    p = ET.SubElement(parent, "property", name=nom)
    if trad: p.set("translatable", "yes")
    p.text = val
    return p

def widget(cls, wid, props):
    w = ET.Element("widget", {"class": cls, "id": wid})
    prop(w, "visible", "True")
    for k, v in props:
        prop(w, k, v, k in ("label", "title"))
    return w

def label(wid, texte, markup=False, xalign="0"):
    return widget("GtkLabel", wid, [("label", texte), ("use_underline", "False"),
        ("use_markup", "True" if markup else "False"), ("justify", "GTK_JUSTIFY_LEFT"),
        ("wrap", "False"), ("selectable", "False"), ("xalign", xalign), ("yalign", "0.5"),
        ("xpad", "0"), ("ypad", "0")])

def enfant(parent, w, packing=None):
    c = ET.SubElement(parent, "child")
    c.append(w)
    if packing:
        pk = ET.SubElement(c, "packing")
        for k, v in packing:
            prop(pk, k, v)
    return c

def boite(wid, enfants):
    # enfants : liste de (widget, expand, fill)
    b = widget("GtkHBox" if "hbox" in wid else "GtkVBox", wid,
               [("homogeneous", "False"), ("spacing", "10")])
    for w, e, f in enfants:
        enfant(b, w, [("padding", "0"), ("expand", e), ("fill", f)])
    return b

# ---------- lecture de espaces.txt ----------
if not os.path.exists(config):
    sys.exit("ERREUR : espaces.txt introuvable a cote de assembler.py")
espaces = []
for ligne in open(config, encoding="utf-8"):
    ligne = ligne.strip()
    if not ligne or ligne.startswith("#"):
        continue
    m = re.match(r"^\[(\w+)\s*\|\s*(.+)\]$", ligne)
    if m:
        espaces.append({"id": m.group(1), "titre": m.group(2).strip(), "onglets": []})
    elif "=" in ligne and espaces:
        onglet, fen = [x.strip() for x in ligne.split("=", 1)]
        espaces[-1]["onglets"].append((onglet, fen))
    else:
        sys.exit("ERREUR : ligne incomprise dans espaces.txt : " + ligne)

try:
    arbre = ET.parse(projet)
except Exception as e:
    sys.exit("ERREUR : carthage.glade illisible (lance d'abord integration.py) : %s" % e)
glade = arbre.getroot()
fenetres = {w.get("id"): w for w in glade.findall("widget")}

for esp in espaces:
    wid = esp["id"]
    if wid in fenetres:
        sys.exit("ERREUR : une fenetre s'appelle deja '%s'" % wid)
    for onglet, fen in esp["onglets"]:
        if fen not in fenetres:
            sys.exit("ERREUR : la fenetre '%s' (onglet %s) n'existe pas dans carthage.glade" % (fen, onglet))

for esp in espaces:
    wid = esp["id"]
    win = widget("GtkWindow", wid, [("title", esp["titre"]), ("type", "GTK_WINDOW_TOPLEVEL"),
        ("window_position", "GTK_WIN_POS_CENTER"), ("modal", "False"),
        ("default_width", "1100"), ("default_height", "700"), ("resizable", "True"),
        ("destroy_with_parent", "False"), ("decorated", "True"),
        ("skip_taskbar_hint", "False"), ("skip_pager_hint", "False"),
        ("type_hint", "GDK_WINDOW_TYPE_HINT_NORMAL"), ("gravity", "GDK_GRAVITY_NORTH_WEST"),
        ("focus_on_map", "True"), ("urgency_hint", "False")])

    bouton = widget("GtkButton", wid + "_deconnexion", [("can_focus", "True"),
        ("label", "gtk-quit"), ("use_stock", "True"), ("relief", "GTK_RELIEF_NORMAL"),
        ("focus_on_click", "True")])
    ET.SubElement(bouton, "signal", name="clicked",
                  handler="on_%s_deconnexion_clicked" % wid, last_modification_time=T)

    entete = boite(wid + "_hbox_entete", [
        (label(wid + "_titre", '<span size="x-large" weight="bold" foreground="#1F4E79">%s</span>'
               % esp["titre"].replace("&", "&amp;"), True), "False", "False"),
        (label(wid + "_connecte", "Connecté : ", False, "1"), "True", "True"),
        (bouton, "False", "False"),
    ])

    nb = widget("GtkNotebook", wid + "_notebook", [("can_focus", "True"),
        ("show_tabs", "True"), ("show_border", "True"), ("tab_pos", "GTK_POS_LEFT"),
        ("scrollable", "True"), ("enable_popup", "False")])

    for n, (onglet, fen) in enumerate(esp["onglets"], 1):
        source = fenetres[fen]
        contenu = source.find("child/widget")
        if contenu is None:
            sys.exit("ERREUR : la fenetre '%s' est vide" % fen)
        enfant(nb, contenu, [("tab_expand", "False"), ("tab_fill", "True")])
        enfant(nb, label("%s_onglet%d" % (wid, n), "  " + onglet + "  "), [("type", "tab")])
        glade.remove(source)

    principal = boite(wid + "_vbox", [(entete, "False", "True"), (nb, "True", "True")])
    prop(principal, "border_width", "8")
    enfant(win, principal)
    glade.insert(0, win)

    # noms en double dans la nouvelle fenetre -> renommes
    tous = set(w.get("id") for w in glade.iter("widget"))
    vus = set()
    for w in win.iter("widget"):
        i = w.get("id")
        if i in vus:
            k = 2
            while "%s_%d" % (i, k) in tous:
                k += 1
            w.set("id", "%s_%d" % (i, k))
            tous.add(w.get("id"))
            print("  attention : dans %s, un 2e '%s' a ete renomme en '%s'" % (wid, i, w.get("id")))
        vus.add(w.get("id"))
    print("  %s : %d onglet(s) -> %s" % (wid, len(esp["onglets"]),
          ", ".join(o for o, f in esp["onglets"])))

with open(projet, "w", encoding="utf-8") as out:
    out.write('<?xml version="1.0" standalone="no"?> <!--*- mode: xml -*-->\n')
    out.write('<!DOCTYPE glade-interface SYSTEM "http://glade.gnome.org/glade-2.0.dtd">\n\n')
    out.write(ET.tostring(glade, encoding="unicode"))
    out.write("\n")
ET.parse(projet)
print("\nOK : fenetres finales dans carthage.glade :")
for w in glade.findall("widget"):
    print("  -", w.get("id"))
