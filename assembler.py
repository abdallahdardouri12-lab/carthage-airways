# -*- coding: utf-8 -*-
# Carthage Airways - construit les espaces par role (admin, passager...)
# : une fenetre avec en-tete, menu a gauche et un onglet par module.
# Configuration : espaces.txt     A lancer APRES integration.py :
#     python3 integration.py
#     python3 assembler.py
import os, sys, re, copy, unicodedata
import xml.etree.ElementTree as ET

racine = os.path.dirname(os.path.abspath(__file__))
projet = os.path.join(racine, "carthage.glade")
config = os.path.join(racine, "espaces.txt")
T = "Fri, 30 Oct 2026 12:00:00 GMT"
BLEU = "#1F4E79"

ICONES = [("tableau", "gtk-home"), ("vol", "gtk-go-up"), ("passager", "gtk-orientation-portrait"),
          ("reserv", "gtk-index"), ("réserv", "gtk-index"), ("personnel", "gtk-justify-fill"),
          ("avion", "gtk-media-forward"), ("aéroport", "gtk-network"), ("aeroport", "gtk-network"),
          ("profil", "gtk-orientation-portrait"), ("recherch", "gtk-find"), ("fid", "gtk-about"),
          ("planning", "gtk-index")]
DESCRIPTIONS = {
    "vols": "Ajouter, modifier, supprimer, rechercher et filtrer les vols ; affecter un avion.",
    "passagers": "Gérer les comptes passagers, trier par nom, suivre la fidélité.",
    "réservations": "Ajouter, modifier, supprimer et filtrer les réservations.",
    "personnel navigant": "Gérer les membres du personnel navigant.",
    "avions": "Gérer les avions, leur état et les places disponibles.",
    "aéroports": "Gérer les aéroports et les affecter à un vol.",
}
compteur = [0]

def nid(base):
    compteur[0] += 1
    return "%s_%d" % (base, compteur[0])

def prop(parent, nom, val, trad=False):
    p = ET.SubElement(parent, "property", name=nom)
    if trad: p.set("translatable", "yes")
    p.text = val

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

def image(wid, stock):
    return widget("GtkImage", wid, [("stock", stock), ("icon_size", "1"), ("xalign", "0.5"),
        ("yalign", "0.5"), ("xpad", "0"), ("ypad", "0")])

def enfant(parent, w, packing=None):
    c = ET.SubElement(parent, "child")
    c.append(w)
    if packing:
        pk = ET.SubElement(c, "packing")
        for k, v in packing:
            prop(pk, k, v)

def boite(vertical, wid, enfants, spacing="8", border=None):
    b = widget("GtkVBox" if vertical else "GtkHBox", wid,
               [("homogeneous", "False"), ("spacing", spacing)])
    if border: prop(b, "border_width", border)
    for w, e, f in enfants:
        enfant(b, w, [("padding", "0"), ("expand", e), ("fill", f)])
    return b

def table(wid, cellules, nl, nc):
    t = widget("GtkTable", wid, [("n_rows", str(nl)), ("n_columns", str(nc)),
        ("homogeneous", "False"), ("row_spacing", "8"), ("column_spacing", "25")])
    for (w, l, c) in cellules:
        enfant(t, w, [("left_attach", str(c)), ("right_attach", str(c + 1)),
                      ("top_attach", str(l)), ("bottom_attach", str(l + 1)),
                      ("x_options", "fill"), ("y_options", "")])
    return t

def cadre(wid, titre, contenu):
    f = widget("GtkFrame", wid, [("label_xalign", "0"), ("label_yalign", "0.5"),
                                 ("shadow_type", "GTK_SHADOW_ETCHED_IN")])
    enfant(f, contenu)
    enfant(f, label(wid + "_t", "<b> %s </b>" % titre, True), [("type", "label_item")])
    return f

def icone_pour(nom):
    n = nom.lower()
    for cle, stock in ICONES:
        if cle in n:
            return stock
    return "gtk-file"

def page_titre(wid, titre, sous_titre):
    return boite(True, nid(wid + "_vt"), [
        (label(nid(wid + "_pt"), '<span size="x-large" weight="bold" foreground="%s">%s</span>' % (BLEU, titre), True), "False", "False"),
        (label(nid(wid + "_ps"), '<span foreground="#666666">%s</span>' % sous_titre, True), "False", "False"),
    ], "2")

def page_tableau(wid, onglets):
    modules = [o for o, f in onglets if not f.startswith("@tableau")]
    cartes = []
    for k, m in enumerate(modules):
        cle = unicodedata.normalize("NFKD", m).encode("ascii", "ignore").decode().lower()
        cle = re.sub(r"[^a-z0-9]+", "_", cle).strip("_")
        carte = boite(True, nid(wid + "_carte"), [
            (label(nid(wid + "_cn"), '<b><span foreground="%s">%s</span></b>' % (BLEU, m), True), "False", "False"),
            (label("%s_stat_%s" % (wid, cle), '<span size="xx-large" weight="bold">0</span>', True), "False", "False"),
            (label(nid(wid + "_cd"), '<span size="small" foreground="#666666">enregistré(s)</span>', True), "False", "False"),
        ], "2", "6")
        cartes.append((carte, k // 3, k % 3))
    vue = cadre(nid(wid + "_vue"), "Vue d'ensemble",
                table(nid(wid + "_tvue"), cartes, max(1, (len(modules) + 2) // 3), 3))
    lignes = []
    for k, m in enumerate(modules):
        lignes.append((label(nid(wid + "_mn"), "<b>%s</b>" % m, True), k, 0))
        lignes.append((label(nid(wid + "_md"), DESCRIPTIONS.get(m.lower(), "")), k, 1))
    liste = cadre(nid(wid + "_mods"), "Modules (menu à gauche)",
                  table(nid(wid + "_tmods"), lignes, max(1, len(modules)), 2))
    return boite(True, nid(wid + "_tdb"), [
        (page_titre(wid, "Tableau de bord", "Choisissez un module dans le menu à gauche"), "False", "False"),
        (vue, "False", "True"), (liste, "False", "True"),
    ], "15", "15")

def page_vide(wid, nom):
    return boite(True, nid(wid + "_vide"), [
        (page_titre(wid, nom, "Module en cours d'intégration"), "False", "False"),
    ], "15", "15")

def normal(t):
    t = unicodedata.normalize("NFKD", t or "").encode("ascii", "ignore").decode()
    return t.lower()

def parties(valeur):
    """ "Titre: fenetre [sans: mot1, mot2]" -> liste de (titre, fenetre, mode, mots) """
    res = []
    for morceau in valeur.split("+"):
        morceau = morceau.strip()
        m = re.match(r"^(?:([^:\[]+):)?\s*([A-Za-z0-9_@]+)\s*(?:\[\s*(avec|sans)\s*:\s*([^\]]*)\])?\s*$", morceau)
        if not m:
            sys.exit("ERREUR : ecriture incomprise dans espaces.txt : " + morceau)
        f = m.group(2)
        t = (m.group(1) or f).strip()
        mots = [normal(x.strip()) for x in (m.group(4) or "").split(",") if x.strip()]
        res.append((t, f, m.group(3), mots))
    return res

def texte_onglet(w):
    if w is None:
        return ""
    for x in w.iter("widget"):
        if x.get("class") == "GtkLabel":
            p = x.find("property[@name='label']")
            if p is not None and p.text:
                return re.sub(r"<[^>]+>", "", p.text).strip()
    return ""

def filtrer_onglets(contenu, mode, mots, fen):
    """Garde (avec) ou enleve (sans) les sous-onglets dont le titre contient un des mots."""
    parents = {c: p for p in contenu.iter() for c in p}
    nb = None
    for w in contenu.iter("widget"):
        if w.get("class") == "GtkNotebook":
            nb = w
            break
    if nb is None:
        sys.exit("ERREUR : la fenetre '%s' n'a pas de sous-onglets, on ne peut pas faire [%s: ...]" % (fen, mode))
    paires = []
    for c in nb.findall("child"):
        t = c.find("packing/property[@name='type']")
        if t is not None and t.text == "tab":
            if paires:
                paires[-1][1] = c
        else:
            paires.append([c, None])
    gardes, titres = [], []
    for page, tab in paires:
        titre = texte_onglet(tab.find("widget") if tab is not None else None)
        trouve = any(m in normal(titre) for m in mots)
        if (mode == "avec") == trouve:
            gardes.append(page)
            titres.append(titre)
        else:
            nb.remove(page)
            if tab is not None:
                nb.remove(tab)
    if not gardes:
        tous = [texte_onglet(t.find("widget")) if t is not None else "?" for p, t in paires]
        sys.exit("ERREUR : aucun sous-onglet de '%s' ne correspond a [%s: %s]. Sous-onglets : %s"
                 % (fen, mode, ", ".join(mots), ", ".join(tous)))
    if len(gardes) == 1:
        seul = gardes[0].find("widget")
        if nb is contenu:
            contenu = seul
        else:
            pc = parents[nb]
            i = list(pc).index(nb)
            pc.remove(nb)
            pc.insert(i, seul)
    return contenu, titres

# ---------- lecture de espaces.txt ----------
if not os.path.exists(config):
    sys.exit("ERREUR : espaces.txt introuvable a cote de assembler.py")
espaces = []
garder = set()
for ligne in open(config, encoding="utf-8"):
    ligne = ligne.strip()
    if not ligne or ligne.startswith("#"):
        continue
    if ligne.startswith("!garder"):
        garder.update(x.strip() for x in ligne[len("!garder"):].split(",") if x.strip())
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
utilisees = set()

for esp in espaces:
    if esp["id"] in fenetres:
        sys.exit("ERREUR : une fenetre s'appelle deja '%s' (relance d'abord integration.py)" % esp["id"])
    for onglet, fen in esp["onglets"]:
        if fen.startswith("@"):
            continue
        for titre_sous, f, mode, mots in parties(fen):
            if f not in fenetres:
                sys.exit("ERREUR : la fenetre '%s' (onglet %s) n'existe pas dans carthage.glade" % (f, onglet))

for pos, esp in enumerate(espaces):
    wid = esp["id"]
    titre = esp["titre"]
    principal_txt, _, sous = titre.partition(" - ")
    win = widget("GtkWindow", wid, [("title", titre), ("type", "GTK_WINDOW_TOPLEVEL"),
        ("window_position", "GTK_WIN_POS_CENTER"), ("modal", "False"),
        ("default_width", "950"), ("default_height", "620"), ("resizable", "True"),
        ("destroy_with_parent", "False"), ("decorated", "True"),
        ("skip_taskbar_hint", "False"), ("skip_pager_hint", "False"),
        ("type_hint", "GDK_WINDOW_TYPE_HINT_NORMAL"), ("gravity", "GDK_GRAVITY_NORTH_WEST"),
        ("focus_on_map", "True"), ("urgency_hint", "False")])

    # ----- en-tete -----
    role = "Administrateur" if "admin" in wid else ("Passager" if "passager" in wid else "Utilisateur")
    bouton = widget("GtkButton", wid + "_deconnexion", [("can_focus", "True"),
        ("relief", "GTK_RELIEF_NORMAL"), ("focus_on_click", "True")])
    ET.SubElement(bouton, "signal", name="clicked", handler="on_%s_deconnexion_clicked" % wid,
                  last_modification_time=T)
    enfant(bouton, boite(False, wid + "_deco_hbox", [
        (image(wid + "_deco_img", "gtk-quit"), "False", "False"),
        (label(wid + "_deco_lbl", "Déconnexion"), "False", "False")], "4"))
    entete = boite(False, wid + "_entete", [
        (boite(True, wid + "_entete_titres", [
            (label(wid + "_titre", '<span size="x-large" weight="bold" foreground="%s">✈ %s</span>' % (BLEU, principal_txt.strip()), True), "False", "False"),
            (label(wid + "_sous_titre", '<span foreground="#666666">%s</span>' % sous.strip(), True), "False", "False"),
        ], "0"), "False", "False"),
        (label(wid + "_connecte", "%s connecté : —" % role, False, "1"), "True", "True"),
        (bouton, "False", "False"),
    ], "15")

    # ----- onglets a gauche -----
    nb = widget("GtkNotebook", wid + "_notebook", [("can_focus", "True"),
        ("show_tabs", "True"), ("show_border", "True"), ("tab_pos", "GTK_POS_LEFT"),
        ("scrollable", "True"), ("enable_popup", "False")])
    for n, (onglet, fen) in enumerate(esp["onglets"], 1):
        if fen.startswith("@tableau"):
            contenu = page_tableau(wid, esp["onglets"])
        elif fen.startswith("@"):
            contenu = page_vide(wid, onglet)
        else:
            morceaux = parties(fen)
            pages = []
            for titre_sous, f, mode, mots in morceaux:
                c = copy.deepcopy(fenetres[f].find("child/widget"))
                if c is None:
                    sys.exit("ERREUR : la fenetre '%s' est vide" % f)
                if mode:
                    c, gardes = filtrer_onglets(c, mode, mots, f)
                    print("  %s > %s : sous-onglets gardes = %s" % (wid, onglet, ", ".join(gardes)))
                utilisees.add(f)
                pages.append((titre_sous, c))
            if len(pages) == 1:
                contenu = pages[0][1]
            else:
                contenu = widget("GtkNotebook", "%s_sousnb%d" % (wid, n), [("can_focus", "True"),
                    ("show_tabs", "True"), ("show_border", "True"), ("tab_pos", "GTK_POS_TOP"),
                    ("scrollable", "False"), ("enable_popup", "False")])
                for k, (titre_sous, c) in enumerate(pages, 1):
                    enfant(contenu, c, [("tab_expand", "False"), ("tab_fill", "True")])
                    enfant(contenu, label("%s_sousnb%d_t%d" % (wid, n, k), "  %s  " % titre_sous, False, "0.5"),
                           [("type", "tab")])
        # chaque onglet dans une zone defilante : la fenetre peut etre petite
        sw = widget("GtkScrolledWindow", "%s_sw%d" % (wid, n), [("can_focus", "True"),
            ("hscrollbar_policy", "GTK_POLICY_AUTOMATIC"), ("vscrollbar_policy", "GTK_POLICY_AUTOMATIC"),
            ("shadow_type", "GTK_SHADOW_NONE"), ("window_placement", "GTK_CORNER_TOP_LEFT")])
        vp = widget("GtkViewport", "%s_vp%d" % (wid, n), [("shadow_type", "GTK_SHADOW_NONE")])
        enfant(vp, contenu)
        enfant(sw, vp)
        enfant(nb, sw, [("tab_expand", "False"), ("tab_fill", "True")])
        tab = boite(False, "%s_onglet%d" % (wid, n), [
            (image("%s_onglet%d_img" % (wid, n), icone_pour(onglet)), "False", "False"),
            (label("%s_onglet%d_lbl" % (wid, n), onglet), "False", "False")], "6", "3")
        enfant(nb, tab, [("type", "tab")])

    enfant(win, boite(True, wid + "_vbox", [(entete, "False", "True"),
                                            (widget("GtkHSeparator", wid + "_sep", []), "False", "True"),
                                            (nb, "True", "True")], "8", "8"))
    glade.insert(pos, win)

    # noms en double dans la fenetre -> renommes
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
    print("  %s : %s" % (wid, ", ".join(o for o, f in esp["onglets"])))

for fen in utilisees - garder:
    glade.remove(fenetres[fen])

with open(projet, "w", encoding="utf-8") as out:
    out.write('<?xml version="1.0" standalone="no"?> <!--*- mode: xml -*-->\n')
    out.write('<!DOCTYPE glade-interface SYSTEM "http://glade.gnome.org/glade-2.0.dtd">\n\n')
    out.write(ET.tostring(glade, encoding="unicode"))
    out.write("\n")
ET.parse(projet)
print("\nOK : fenetres finales dans carthage.glade :")
for w in glade.findall("widget"):
    print("  -", w.get("id"))
