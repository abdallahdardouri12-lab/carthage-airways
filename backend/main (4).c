#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reservation.h"

static void lire_chaine(const char *invite, char *dest, int taille)
{
    char ligne[256];
    printf("%s", invite);
    if (fgets(ligne, sizeof ligne, stdin) == NULL)
    {
        printf("\n  Fin de la saisie.\n");
        exit(0);                      
    }
    ligne[strcspn(ligne, "\n")] = '\0';
    strncpy(dest, ligne, taille - 1);
    dest[taille - 1] = '\0';
}

static int lire_entier(const char *invite, int min, int max)
{
    char tampon[30];
    int x;
    do
    {
        lire_chaine(invite, tampon, sizeof tampon);
        if (sscanf(tampon, "%d", &x) == 1 && x >= min && x <= max)
            return x;
        printf("    -> valeur attendue entre %d et %d\n", min, max);
    } while (1);
}

static float lire_reel(const char *invite)
{
    char tampon[30];
    float x;
    do
    {
        lire_chaine(invite, tampon, sizeof tampon);
        if (sscanf(tampon, "%f", &x) == 1 && x >= 0)
            return x;
        printf("    -> nombre positif attendu\n");
    } while (1);
}

static int lire_oui_non(const char *invite)
{
    char tampon[10];
    lire_chaine(invite, tampon, sizeof tampon);
    return (tampon[0] == 'o' || tampon[0] == 'O');
}

static void pause_ecran(void)
{
    char t[5];
    printf("\n  Appuyez sur Entrée pour continuer...");
    if (fgets(t, sizeof t, stdin) == NULL) {}
}


static void saisir_services(Services *s)
{
    int i;
    memset(s, 0, sizeof *s);
    printf("\n  --- Restauration ---\n");
    s->repas = lire_oui_non("  Repas spécial ? (o/n) : ");
    if (s->repas)
    {
        for (i = 0; i < 12; i++)
            printf("    %2d. %s\n", i, libelle_repas(i));
        s->type_repas = lire_entier("  Type de repas : ", 0, 11);
        lire_chaine("  Allergies (Entrée si aucune) : ", s->allergies, sizeof s->allergies);
    }
    for (i = 0; i < 4; i++)
        printf("    %d. %s\n", i, libelle_boisson(i));
    s->boisson = lire_entier("  Boisson : ", 0, 3);
    s->dessert = lire_oui_non("  Dessert tunisien (6 DT) ? (o/n) : ");

    printf("\n  --- Bagages ---\n");
    s->bagage_sup = lire_oui_non("  Bagages supplémentaires (60 DT piÃ¨ce) ? (o/n) : ");
    if (s->bagage_sup)
        s->nb_bagages = lire_entier("  Nombre (1-5) : ", 1, 5);
    s->kg_excedent = lire_entier("  Kilos d'excédent (0-30, 8 DT/kg) : ", 0, 30);
    for (i = 0; i < 6; i++)
        printf("    %d. %s\n", i, libelle_bagage_special(i));
    s->bagage_special = lire_entier("  Bagage spécial : ", 0, 5);

    printf("\n  --- Assistance ---\n");
    s->pmr = lire_oui_non("  Assistance PMR (20 DT) ? (o/n) : ");
    if (s->pmr)
    {
        for (i = 0; i < 7; i++)
            printf("    %d. %s\n", i, libelle_pmr(i));
        s->type_pmr = lire_entier("  Type d'assistance : ", 0, 6);
        s->fauteuil_perso = lire_oui_non("  Propre fauteuil roulant ? (o/n) : ");
        s->chien_assistance = lire_oui_non("  Chien d'assistance ? (o/n) : ");
    }
    s->umnr = lire_oui_non("  Mineur non accompagné (80 DT) ? (o/n) : ");

    printf("\n  --- Extras ---\n");
    s->priorite  = lire_oui_non("  Embarquement prioritaire (20 DT) ? (o/n) : ");
    s->salon     = lire_oui_non("  Accés salon (45 DT) ? (o/n) : ");
    s->wifi      = lire_oui_non("  Wi-Fi Ã  bord (10 DT) ? (o/n) : ");
    s->assurance = lire_oui_non("  Assurance annulation (25 DT) ? (o/n) : ");
    s->sms = 1;
}

/* ------------------------------- Saisie complÃ¨te d'une rÃ©servation */
static void saisir_reservation(Reservation *r, int nouvelle)
{
    int pref;

    if (nouvelle)
    {
        memset(r, 0, sizeof *r);
        generer_id(FICHIER_RESERVATIONS, r->id);
        printf("\n  Référence générée : %s\n", r->id);
    }
    printf("\n  === Etape 1 : vol ===\n");
    lire_chaine("  N° de vol (ex : CA101) : ", r->vol, sizeof r->vol);
    lire_chaine("  Départ (ex : Tunis TUN) : ", r->depart, sizeof r->depart);
    lire_chaine("  Arrivée (ex : Paris CDG) : ", r->arrivee, sizeof r->arrivee);
    r->date_vol.jour  = lire_entier("  Jour du vol : ", 1, 31);
    r->date_vol.mois  = lire_entier("  Mois : ", 1, 12);
    r->date_vol.annee = lire_entier("  Année : ", 2026, 2030);
    r->h_depart.heure   = lire_entier("  Heure de départ (0-23) : ", 0, 23);
    r->h_depart.minute  = lire_entier("  Minute de départ : ", 0, 59);
    r->h_arrivee.heure  = lire_entier("  Heure d'arrivée (0-23) : ", 0, 23);
    r->h_arrivee.minute = lire_entier("  Minute d'arrivée : ", 0, 59);
    r->prix_base = lire_reel("  Prix du vol en économique (DT) : ");

    printf("\n  === Etape 2 : passager et classe ===\n");
    lire_chaine("  CIN (8 chiffres) : ", r->cin, sizeof r->cin);
    lire_chaine("  Nom et prÃ©nom : ", r->nom, sizeof r->nom);
    lire_chaine("  Teléphone (8 chiffres, Entrée pour passer) : ", r->tel, sizeof r->tel);
    lire_chaine("  E-mail (Entrée pour passer) : ", r->email, sizeof r->email);
    r->type_passager = lire_entier("  Type (0 Adulte, 1 Enfant -25%, 2 Bébe -90%) : ", 0, 2);
    r->classe = lire_entier("  Classe (0 Ã‰conomique, 1 Affaires x2,5, 2 Premiere x4) : ", 0, 2);
    r->niveau_fidelite = lire_entier("  Niveau fidelité (0 Standard, 1 Silver, 2 Gold, 3 Platinum) : ", 0, 3);

    printf("\n  === Etape 3 : siege ===\n");
    if (r->type_passager == PASSAGER_BEBE)
    {
        r->siege[0] = '\0';
        printf("  Un bebe voyage sur les genoux : pas de siege.\n");
    }
    else
    {
        printf("  Premiere : 1-2 (A C D F) | Affaires : 3-5 (A C D F) | Eco : 6-14 (A-F), rangee 10 = +35 DT\n");
        lire_chaine("  Siege (ex : 7A) ou « auto » : ", r->siege, sizeof r->siege);
        if (strcmp(r->siege, "auto") == 0)
        {
            pref = lire_entier("  Preference (0 Indifferent, 1 Hublot, 2 Couloir, 3 Milieu) : ", 0, 3);
            if (attribuer_siege_auto(FICHIER_RESERVATIONS, *r, pref, r->siege))
                printf("  Siege attribue : %s\n", r->siege);
            else
            {
                r->siege[0] = '\0';
                printf("  Aucun siege libre : la reservation passera en liste d'attente.\n");
            }
        }
    }

    printf("\n  === Etape 4 : services ===");
    saisir_services(&r->services);

    printf("\n  === Etape 5 : paiement ===\n");
    lire_chaine("  Code promo (CARTHAGE10, ETUDIANT15, BIENVENUE ou Entree) : ",
                r->code_promo, sizeof r->code_promo);
    r->mode_paiement = lire_entier("  Paiement (0 Carte, 1 Especes, 2 Virement, 3 Points) : ", 0, 3);
    r->paye = lire_oui_non("  Paiement reçu ? (o/n) : ");
    r->statut = r->paye ? STATUT_CONFIRMEE : STATUT_EN_ATTENTE;
    if (r->siege[0] == '\0' && r->type_passager != PASSAGER_BEBE)
        r->statut = STATUT_LISTE_ATTENTE;
}

static int verifier_avant_enregistrement(Reservation *r)
{
    char message[150];
    Reservation conflit;
    DetailPrix d;

    if (r->statut != STATUT_LISTE_ATTENTE && !controle_saisie(*r, message))
    {
        printf("\n  ERREUR : %s\n", message);
        return 0;
    }
    if (r->siege[0] != '\0' &&
        siege_occupe(FICHIER_RESERVATIONS, r->vol, r->date_vol, r->siege, r->id))
    {
        printf("\n  ERREUR : le siege %s est deja  occupee sur ce vol.\n", r->siege);
        return 0;
    }
    if (verifier_conflit(FICHIER_RESERVATIONS, *r, &conflit))
    {
        printf("\n  CONFLIT D'HORAIRE : ce passager est deja  sur le vol %s du %02d/%02d/%04d "
               "(%02d:%02d - %02d:%02d, reservation %s).\n",
               conflit.vol, conflit.date_vol.jour, conflit.date_vol.mois, conflit.date_vol.annee,
               conflit.h_depart.heure, conflit.h_depart.minute,
               conflit.h_arrivee.heure, conflit.h_arrivee.minute, conflit.id);
        return 0;
    }
    d = calculer_prix(r);
    printf("\n  --- Recapitulatif ---\n");
    printf("  Billet %s            : %10.3f DT\n", libelle_classe(r->classe), d.billet);
    printf("  Reduction %-10s : %10.3f DT\n", libelle_type_passager(r->type_passager), -d.reduction_type);
    printf("  Siege                    : %10.3f DT\n", d.siege);
    printf("  Restauration             : %10.3f DT\n", d.restauration);
    printf("  Bagages                  : %10.3f DT\n", d.bagages);
    printf("  Assistance               : %10.3f DT\n", d.assistance);
    printf("  Extras                   : %10.3f DT\n", d.extras);
    printf("  Sous-total               : %10.3f DT\n", d.sous_total);
    printf("  Remise fidelite %-8s : %10.3f DT\n", libelle_niveau(r->niveau_fidelite), -d.remise_fidelite);
    printf("  Code promo               : %10.3f DT\n", -d.remise_promo);
    printf("  Taxes aeroport           : %10.3f DT\n", d.taxes);
    printf("  TOTAL                    : %10.3f DT  (+%d points)\n", d.total, d.points);
    return 1;
}

/*  Actions du menu */
static void menu_ajouter(void)
{
    Reservation r;
    int res;
    saisir_reservation(&r, 1);
    if (!verifier_avant_enregistrement(&r))
        return;
    if (!lire_oui_non("\n  Confirmer l'enregistrement ? (o/n) : "))
        return;
    res = ajouter_reservation(FICHIER_RESERVATIONS, r);
    if (res == 1)
        printf("  Reservation %s enregistree (%s).\n", r.id, libelle_statut(r.statut));
    else if (res == 0)
        printf("  ERREUR : la reference %s existe deja .\n", r.id);
    else
        printf("  ERREUR : impossible d'ouvrir le fichier.\n");
}

static void menu_modifier(void)
{
    char id[20];
    Reservation r;
    int choix;

    lire_chaine("\n  Reference a  modifier : ", id, sizeof id);
    if (!rechercher_reservation(FICHIER_RESERVATIONS, id, &r))
    {
        printf("  Reservation introuvable.\n");
        return;
    }
    afficher_reservation(r);
    printf("\n  1. Siege   2. Classe   3. Services   4. Statut et paiement   5. Tout ressaisir\n");
    choix = lire_entier("  Choix : ", 1, 5);
    if (choix == 1)
    {
        lire_chaine("  Nouveau siege (ex : 8C) ou « auto » : ", r.siege, sizeof r.siege);
        if (strcmp(r.siege, "auto") == 0)
        {
            if (attribuer_siege_auto(FICHIER_RESERVATIONS, r,
                                     lire_entier("  Preference (0 Indifferent, 1 Hublot, 2 Couloir, 3 Milieu) : ", 0, 3), r.siege))
                printf("  Siege attribue : %s\n", r.siege);
            else
                printf("  Aucun siege libre dans cette classe.\n");
        }
    }
    else if (choix == 2)
    {
        r.classe = lire_entier("  Classe (0 Economique, 1 Affaires, 2 Premiere) : ", 0, 2);
        lire_chaine("  Nouveau siege dans cette classe : ", r.siege, sizeof r.siege);
    }
    else if (choix == 3)
        saisir_services(&r.services);
    else if (choix == 4)
    {
        r.statut = lire_entier("  Statut (0 Confirmee, 1 En attente, 2 Liste d'attente, 3 Annulee) : ", 0, 3);
        r.mode_paiement = lire_entier("  Paiement (0 Carte, 1 Especes, 2 Virement, 3 Points) : ", 0, 3);
        r.paye = lire_oui_non("  Paiement reçu ? (o/n) : ");
    }
    else
        saisir_reservation(&r, 0);

    if (!verifier_avant_enregistrement(&r))
        return;
    if (modifier_reservation(FICHIER_RESERVATIONS, id, r) == 1)
        printf("  Reservation modifiée.\n");
    else
        printf("  ERREUR lors de la modification.\n");
}

static void menu_supprimer(void)
{
    char id[20];
    lire_chaine("\n  Reference a  supprimer : ", id, sizeof id);
    if (!existe_reservation(FICHIER_RESERVATIONS, id))
    {
        printf("  Reservation introuvable.\n");
        return;
    }
    if (lire_oui_non("  Suppression definitive. Confirmer ? (o/n) : ") &&
        supprimer_reservation(FICHIER_RESERVATIONS, id) == 1)
        printf("  Reservation supprimee.\n");
}

static void menu_rechercher(void)
{
    char id[20];
    Reservation r;
    lire_chaine("\n  Reference : ", id, sizeof id);
    if (rechercher_reservation(FICHIER_RESERVATIONS, id, &r))
        afficher_reservation(r);
    else
        printf("  Reservation introuvable.\n");
}

static void menu_filtrer(void)
{
    Filtre f;
    Reservation tab[MAX_RESERVATIONS];
    int n, i, tri;

    filtre_par_defaut(&f);
    lire_chaine("\n  Mot-clée (réf., nom, CIN, vol ; Entrée = tous) : ", f.mot_cle, sizeof f.mot_cle);
    f.classe = lire_entier("  Classe (-1 Toutes, 0 Eco, 1 Affaires, 2 Premiere) : ", -1, 2);
    f.statut = lire_entier("  Statut (-1 Tous, 0 Confirmée, 1 En attente, 2 Liste d'attente, 3 Annulée) : ", -1, 3);
    f.paiement = lire_entier("  Paiement (-1 Tous, 0 Payée, 1 Non payée, 2 Remboursée) : ", -1, 2);
    f.prix_max = lire_reel("  Prix maximum (0 = sans limite) : ");
    f.filtre_periode = lire_oui_non("  Filtrer par période ? (o/n) : ");
    if (f.filtre_periode)
    {
        f.du.jour = lire_entier("  Du jour : ", 1, 31);  f.du.mois = lire_entier("  mois : ", 1, 12);
        f.du.annee = lire_entier("  année : ", 2026, 2030);
        f.au.jour = lire_entier("  Au jour : ", 1, 31);  f.au.mois = lire_entier("  mois : ", 1, 12);
        f.au.annee = lire_entier("  année : ", 2026, 2030);
    }
    tri = lire_entier("  Tri (0 Date recente, 1 Date ancienne, 2 Prix +, 3 Prix -, 4 Nom A-Z, 5 Nom Z-A, 6 Réf.) : ", 0, 6);

    n = filtrer_reservations(FICHIER_RESERVATIONS, f, tab, MAX_RESERVATIONS);
    trier_reservations(tab, n, tri);
    printf("\n  %d resultat(s)\n", n);
    for (i = 0; i < n; i++)
        printf("  %-15s %-22.22s %-7s %02d/%02d/%04d %-10s %-16s %10.3f DT\n",
               tab[i].id, tab[i].nom, tab[i].vol, tab[i].date_vol.jour, tab[i].date_vol.mois,
               tab[i].date_vol.annee, libelle_classe(tab[i].classe),
               libelle_statut(tab[i].statut), tab[i].prix_total);
}

static void menu_annuler(void)
{
    char id[20];
    Reservation r;
    int taux, res;
    float montant;

    lire_chaine("\n  Reference a  annuler : ", id, sizeof id);
    if (!rechercher_reservation(FICHIER_RESERVATIONS, id, &r))
    {
        printf("  Reservation introuvable.\n");
        return;
    }
    montant = calculer_remboursement(r, &taux);
    printf("  Depart dans %.1f heures -> remboursement de %d %% soit %.3f DT%s\n",
           heures_avant_depart(r), taux, montant,
           (r.services.assurance || r.classe == CLASSE_PREMIERE) ? " (assurance annulation)" : "");
    if (!lire_oui_non("  Confirmer l'annulation ? (o/n) : "))
        return;
    res = annuler_reservation(FICHIER_RESERVATIONS, id, &montant);
    if (res == 1)
        printf("  Reservation annulée. Montant remboursée : %.3f DT\n", montant);
    else if (res == -1)
        printf("  Cette reservation est deja  annuléee.\n");
}

static void menu_statistiques(void)
{
    Statistiques s;
    char vol[10];
    Date d;

    calculer_statistiques(FICHIER_RESERVATIONS, &s);
    printf("\n  Total : %d | Confirmées : %d | En attente : %d | Annulées : %d\n",
           s.total, s.confirmees, s.en_attente, s.annulees);
    printf("  Chiffre d'affaires : %.3f DT | Total remboursée : %.3f DT\n",
           s.chiffre_affaires, s.total_rembourse);
    if (lire_oui_non("  Voir le remplissage d'un vol ? (o/n) : "))
    {
        lire_chaine("  N° de vol : ", vol, sizeof vol);
        d.jour = lire_entier("  Jour : ", 1, 31);
        d.mois = lire_entier("  Mois : ", 1, 12);
        d.annee = lire_entier("  Année : ", 2026, 2030);
        printf("  %d siege(s) reservee(s) sur 74.\n", nb_places_reservees(FICHIER_RESERVATIONS, vol, d));
    }
}

static void menu_carte(void)
{
    char id[20], nom_fichier[40];
    Reservation r;
    int res;

    lire_chaine("\n  Reference : ", id, sizeof id);
    if (!rechercher_reservation(FICHIER_RESERVATIONS, id, &r))
    {
        printf("  Reservation introuvable.\n");
        return;
    }
    sprintf(nom_fichier, "carte_%s.txt", id);
    res = generer_carte_embarquement(r, nom_fichier);
    if (res == 1)
        printf("  Carte d'embarquement enregistree dans %s\n", nom_fichier);
    else if (res == 0)
        printf("  La carte n'est disponible que pour une reservation confirmee.\n");
}

static void ajouter_exemples(void)
{
    Reservation r;
    int i;
    const char *noms[] = {"Amira Ben Salah", "Youssef Trabelsi", "Sarra Gharbi", "Mohamed Jaziri"};
    const char *cins[] = {"09876543", "11223344", "07654321", "12345678"};
    const char *sieges[] = {"7A", "3C", "1F", "10D"};
    const int classes[] = {CLASSE_ECO, CLASSE_AFFAIRES, CLASSE_PREMIERE, CLASSE_ECO};

    for (i = 0; i < 4; i++)
    {
        memset(&r, 0, sizeof r);
        generer_id(FICHIER_RESERVATIONS, r.id);
        strcpy(r.cin, cins[i]);
        strcpy(r.nom, noms[i]);
        strcpy(r.tel, "98123456");
        strcpy(r.email, "client@mail.tn");
        strcpy(r.vol, i < 2 ? "CA101" : "CA204");
        strcpy(r.depart, "Tunis TUN");
        strcpy(r.arrivee, i < 2 ? "Paris CDG" : "Istanbul IST");
        r.date_vol.jour = 15 + i; r.date_vol.mois = 12; r.date_vol.annee = 2026;
        r.h_depart.heure = 8 + i;  r.h_depart.minute = 30;
        r.h_arrivee.heure = 11 + i; r.h_arrivee.minute = 0;
        r.classe = classes[i];
        strcpy(r.siege, sieges[i]);
        r.prix_base = (i < 2) ? 450 : 520;
        r.niveau_fidelite = i % 4;
        r.services.repas = (i == 0);
        r.services.type_repas = 3;          /* Halal */
        r.services.boisson = 1;
        r.services.bagage_sup = (i == 3);
        r.services.nb_bagages = 1;
        r.services.assurance = (i == 0);
        r.services.sms = 1;
        r.paye = (i != 1);
        r.statut = r.paye ? STATUT_CONFIRMEE : STATUT_EN_ATTENTE;
        calculer_prix(&r);
        ajouter_reservation(FICHIER_RESERVATIONS, r);
    }
    printf("\n  4 reservations d'exemple ajoutees.\n");
}

/*main */
int main(void)
{
    int choix;
    do
    {
        printf("\n  ===========================================\n");
        printf("    CARTHAGE AIRWAYS - Gestion des reservations\n");
        printf("  ===========================================\n");
        printf("   1. Ajouter une reservation\n");
        printf("   2. Afficher toutes les reservations\n");
        printf("   3. Modifier une reservation\n");
        printf("   4. Supprimer une reservation\n");
        printf("   5. Rechercher une reservation\n");
        printf("   6. Recherche multicritere et tri\n");
        printf("   7. Annuler (calcul du remboursement)\n");
        printf("   8. Statistiques\n");
        printf("   9. Exporter la liste (export_reservations.txt)\n");
        printf("  10. Generer une carte d'embarquement\n");
        printf("  11. Ajouter des reservations d'exemple\n");
        printf("   0. Quitter\n");
        choix = lire_entier("  Votre choix : ", 0, 11);

        switch (choix)
        {
        case 1:  menu_ajouter(); break;
        case 2:  afficher_reservations(FICHIER_RESERVATIONS); break;
        case 3:  menu_modifier(); break;
        case 4:  menu_supprimer(); break;
        case 5:  menu_rechercher(); break;
        case 6:  menu_filtrer(); break;
        case 7:  menu_annuler(); break;
        case 8:  menu_statistiques(); break;
        case 9:
            if (exporter_reservations(FICHIER_RESERVATIONS, "export_reservations.txt") >= 0)
                printf("\n  Liste exportee dans export_reservations.txt\n");
            break;
        case 10: menu_carte(); break;
        case 11: ajouter_exemples(); break;
        }
        if (choix != 0)
            pause_ecran();
    } while (choix != 0);

    printf("\n  Au revoir !\n");
    return 0;
}
