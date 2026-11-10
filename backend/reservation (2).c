#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "reservation.h"


char *noms_repas[12] = {"Standard (inclus)", "Vegetarien (VLML)", "Vegetalien (VGML)", "Halal (MOML)",
                        "Casher (KSML)", "Sans gluten (GFML)", "Diabetique (DBML)", "Hypocalorique (LCML)",
                        "Menu enfant (CHML)", "Repas bebe (BBML)", "Fruits de mer (SFML)", "Saveurs de Tunisie"};
float prix_repas[12] = {0, 12, 14, 10, 18, 15, 15, 12, 10, 8, 22, 25};

char *noms_boisson[4] = {"Eau et soda (inclus)", "The / cafe", "Jus frais", "Pack premium"};
float prix_boisson[4] = {0, 4, 6, 12};

char *noms_special[6] = {"Aucun", "Equipement sportif", "Instrument de musique",
                         "Poussette / siege auto", "Animal en cabine", "Animal en soute"};
float prix_special[6] = {0, 80, 100, 0, 70, 150};

char *noms_pmr[7] = {"WCHR", "WCHS", "WCHC", "BLND", "DEAF", "DPNA", "MAAS"};

float coef_classe[3]   = {1, 2.5, 4};        /* Economique, Affaires, Premiere */
float coef_passager[3] = {1, 0.75, 0.10};    
float remise_niveau[4] = {0, 0.05, 0.10, 0.15};


char *tiret(char *s)
{
    if (strlen(s) == 0)
        return "-";
    return s;
}

void enlever_tiret(char *s)
{
    if (strcmp(s, "-") == 0)
        strcpy(s, "");
}

void enlever_point_virgule(char *s)
{
    int i;
    for (i = 0; s[i] != '\0'; i++)
        if (s[i] == ';')
            s[i] = ',';
}

int meme_date(Date a, Date b)
{
    return a.jour == b.jour && a.mois == b.mois && a.annee == b.annee;
}

int date_en_nombre(Date d)                 /* 15/12/2026 -> 20261215 */
{
    return d.annee * 10000 + d.mois * 100 + d.jour;
}

/* date + heure -> secondes, pour comparer des horaires */
time_t en_secondes(Date d, Heure h)
{
    struct tm t = {0};
    t.tm_year = d.annee - 1900;
    t.tm_mon = d.mois - 1;
    t.tm_mday = d.jour;
    t.tm_hour = h.heure;
    t.tm_min = h.minute;
    t.tm_isdst = -1;
    return mktime(&t);
}


void ecrire_ligne(FILE *f, Reservation r)
{
    Services s = r.services;
    enlever_point_virgule(r.nom);
    enlever_point_virgule(r.email);
    enlever_point_virgule(r.depart);
    enlever_point_virgule(r.arrivee);
    enlever_point_virgule(s.allergies);

    fprintf(f, "%s;%s;%s;%s;%s;%s;%s;%s;", r.id, r.cin, r.nom, tiret(r.tel), tiret(r.email),
            r.vol, tiret(r.depart), tiret(r.arrivee));
    fprintf(f, "%d;%d;%d;%d;%d;%d;%d;", r.date_vol.jour, r.date_vol.mois, r.date_vol.annee,
            r.h_depart.heure, r.h_depart.minute, r.h_arrivee.heure, r.h_arrivee.minute);
    fprintf(f, "%d;%d;%s;", r.classe, r.type_passager, tiret(r.siege));
    fprintf(f, "%d;%d;%d;%d;%s;", s.repas, s.type_repas, s.boisson, s.dessert, tiret(s.allergies));
    fprintf(f, "%d;%d;%d;%d;", s.bagage_sup, s.nb_bagages, s.kg_excedent, s.bagage_special);
    fprintf(f, "%d;%d;%d;%d;%d;", s.pmr, s.type_pmr, s.fauteuil_perso, s.chien_assistance, s.umnr);
    fprintf(f, "%d;%d;%d;%d;%d;", s.priorite, s.salon, s.wifi, s.assurance, s.sms);
    fprintf(f, "%.3f;%s;%d;%d;%d;%d;", r.prix_base, tiret(r.code_promo), r.niveau_fidelite,
            r.mode_paiement, r.paye, r.statut);
    fprintf(f, "%.3f;%d;%.3f\n", r.prix_total, r.points, r.montant_rembourse);
}

int lire_ligne(FILE *f, Reservation *r)
{
    Services *s = &r->services;
    int n = fscanf(f, " %19[^;];%8[^;];%49[^;];%8[^;];%49[^;];%9[^;];%29[^;];%29[^;];"
                      "%d;%d;%d;%d;%d;%d;%d;"
                      "%d;%d;%4[^;];"
                      "%d;%d;%d;%d;%49[^;];"
                      "%d;%d;%d;%d;"
                      "%d;%d;%d;%d;%d;"
                      "%d;%d;%d;%d;%d;"
                      "%f;%19[^;];%d;%d;%d;%d;"
                      "%f;%d;%f",
                   r->id, r->cin, r->nom, r->tel, r->email, r->vol, r->depart, r->arrivee,
                   &r->date_vol.jour, &r->date_vol.mois, &r->date_vol.annee,
                   &r->h_depart.heure, &r->h_depart.minute, &r->h_arrivee.heure, &r->h_arrivee.minute,
                   &r->classe, &r->type_passager, r->siege,
                   &s->repas, &s->type_repas, &s->boisson, &s->dessert, s->allergies,
                   &s->bagage_sup, &s->nb_bagages, &s->kg_excedent, &s->bagage_special,
                   &s->pmr, &s->type_pmr, &s->fauteuil_perso, &s->chien_assistance, &s->umnr,
                   &s->priorite, &s->salon, &s->wifi, &s->assurance, &s->sms,
                   &r->prix_base, r->code_promo, &r->niveau_fidelite, &r->mode_paiement, &r->paye, &r->statut,
                   &r->prix_total, &r->points, &r->montant_rembourse);
    if (n != 46)
        return 0;
    enlever_tiret(r->tel);
    enlever_tiret(r->email);
    enlever_tiret(r->depart);
    enlever_tiret(r->arrivee);
    enlever_tiret(r->siege);
    enlever_tiret(s->allergies);
    enlever_tiret(r->code_promo);
    return 1;
}


/* 1 = ajoutee, 0 = reference deja utilisee, -1 = erreur fichier */
int ajouter_reservation(char *fichier, Reservation r)
{
    FILE *f;
    if (existe_reservation(fichier, r.id))
        return 0;
    f = fopen(fichier, "a");
    if (f == NULL)
        return -1;
    ecrire_ligne(f, r);
    fclose(f);
    return 1;
}

/* o. 1 = modifiee, 0 = introuvable */
int modifier_reservation(char *fichier, char *id, Reservation nouv)
{
    Reservation r;
    int trouve = 0;
    FILE *f = fopen(fichier, "r");
    FILE *tmp = fopen(FICHIER_TEMPORAIRE, "w");
    if (f == NULL || tmp == NULL)
        return -1;
    while (lire_ligne(f, &r))
    {
        if (strcmp(r.id, id) == 0)
        {
            ecrire_ligne(tmp, nouv);
            trouve = 1;
        }
        else
            ecrire_ligne(tmp, r);
    }
    fclose(f);
    fclose(tmp);
    remove(fichier);
    rename(FICHIER_TEMPORAIRE, fichier);
    return trouve;
}

int supprimer_reservation(char *fichier, char *id)
{
    Reservation r;
    int trouve = 0;
    FILE *f = fopen(fichier, "r");
    FILE *tmp = fopen(FICHIER_TEMPORAIRE, "w");
    if (f == NULL || tmp == NULL)
        return -1;
    while (lire_ligne(f, &r))
    {
        if (strcmp(r.id, id) == 0)
            trouve = 1;
        else
            ecrire_ligne(tmp, r);
    }
    fclose(f);
    fclose(tmp);
    remove(fichier);
    rename(FICHIER_TEMPORAIRE, fichier);
    return trouve;
}

/* 1 = trouvee (copiee dans *r), 0 = introuvable */
int rechercher_reservation(char *fichier, char *id, Reservation *r)
{
    FILE *f = fopen(fichier, "r");
    if (f == NULL)
        return 0;
    while (lire_ligne(f, r))
    {
        if (strcmp(r->id, id) == 0)
        {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int existe_reservation(char *fichier, char *id)
{
    Reservation r;
    return rechercher_reservation(fichier, id, &r);
}

/* met toutes les reservations dans le tableau, retourne leur nombre */
int charger_reservations(char *fichier, Reservation tab[], int max)
{
    int n = 0;
    FILE *f = fopen(fichier, "r");
    if (f == NULL)
        return 0;
    while (n < max && lire_ligne(f, &tab[n]))
        n++;
    fclose(f);
    return n;
}

void afficher_reservation(Reservation r)
{
    char services[300];
    services_en_texte(r, services);
    printf("\n%s - %s\n", r.id, libelle_statut(r.statut));
    printf("  Passager : %s (CIN %s), %s\n", r.nom, r.cin, libelle_type_passager(r.type_passager));
    printf("  Vol      : %s  %s -> %s  le %02d/%02d/%04d de %02d:%02d a %02d:%02d\n",
           r.vol, r.depart, r.arrivee, r.date_vol.jour, r.date_vol.mois, r.date_vol.annee,
           r.h_depart.heure, r.h_depart.minute, r.h_arrivee.heure, r.h_arrivee.minute);
    printf("  Classe   : %s   Siege : %s\n", libelle_classe(r.classe), tiret(r.siege));
    printf("  Services : %s\n", services);
    printf("  Total    : %.3f DT (%d points)   Paiement : %s, %s\n", r.prix_total, r.points,
           libelle_paiement(r.mode_paiement), r.paye ? "recu" : "non recu");
    if (r.statut == STATUT_ANNULEE)
        printf("  Rembourse : %.3f DT\n", r.montant_rembourse);
}

void afficher_reservations(char *fichier)
{
    Reservation r;
    int n = 0;
    FILE *f = fopen(fichier, "r");
    if (f == NULL)
    {
        printf("Aucune reservation.\n");
        return;
    }
    printf("%-14s %-20s %-7s %-10s %-10s %-5s %-15s %10s\n",
           "Reference", "Passager", "Vol", "Date", "Classe", "Siege", "Statut", "Total");
    while (lire_ligne(f, &r))
    {
        printf("%-14s %-20.20s %-7s %02d/%02d/%04d %-10s %-5s %-15s %10.3f\n",
               r.id, r.nom, r.vol, r.date_vol.jour, r.date_vol.mois, r.date_vol.annee,
               libelle_classe(r.classe), tiret(r.siege), libelle_statut(r.statut), r.prix_total);
        n++;
    }
    fclose(f);
    printf("%d reservation(s)\n", n);
}


void filtre_par_defaut(Filtre *f)
{
    memset(f, 0, sizeof(Filtre));
    f->classe = -1;
    f->statut = -1;
    f->paiement = -1;
}

/* paiement : -1 tous, 0 payee, 1 non payee, 2 remboursee */
int filtrer_reservations(char *fichier, Filtre filtre, Reservation tab[], int max)
{
    Reservation r;
    int n = 0, ok;
    FILE *f = fopen(fichier, "r");
    if (f == NULL)
        return 0;
    while (n < max && lire_ligne(f, &r))
    {
        ok = 1;
        if (strlen(filtre.mot_cle) > 0 && strstr(r.id, filtre.mot_cle) == NULL && strstr(r.nom, filtre.mot_cle) == NULL
            && strstr(r.cin, filtre.mot_cle) == NULL && strstr(r.vol, filtre.mot_cle) == NULL)
            ok = 0;
        if (filtre.classe != -1 && r.classe != filtre.classe)
            ok = 0;
        if (filtre.statut != -1 && r.statut != filtre.statut)
            ok = 0;
        if (strlen(filtre.vol) > 0 && strcmp(r.vol, filtre.vol) != 0)
            ok = 0;
        if (filtre.paiement == 0 && r.paye == 0)
            ok = 0;
        if (filtre.paiement == 1 && r.paye == 1)
            ok = 0;
        if (filtre.paiement == 2 && r.montant_rembourse <= 0)
            ok = 0;
        if (filtre.filtre_periode && (date_en_nombre(r.date_vol) < date_en_nombre(filtre.du)
                                      || date_en_nombre(r.date_vol) > date_en_nombre(filtre.au)))
            ok = 0;
        if (filtre.prix_max > 0 && r.prix_total > filtre.prix_max)
            ok = 0;
        if (ok)
        {
            tab[n] = r;
            n++;
        }
    }
    fclose(f);
    return n;
}

/* "Mes reservations") */
int reservations_passager(char *fichier, char *cin, Reservation tab[], int max)
{
    Reservation r;
    int n = 0;
    FILE *f = fopen(fichier, "r");
    if (f == NULL)
        return 0;
    while (n < max && lire_ligne(f, &r))
    {
        if (strcmp(r.cin, cin) == 0)
        {
            tab[n] = r;
            n++;
        }
    }
    fclose(f);
    return n;
}

/* 1 si a doit etre avant b (criteres dans le meme ordre que combo_tri) */
int avant(Reservation a, Reservation b, int critere)
{
    switch (critere)
    {
    case TRI_DATE_RECENT:   return date_en_nombre(a.date_vol) > date_en_nombre(b.date_vol);
    case TRI_DATE_ANCIEN:   return date_en_nombre(a.date_vol) < date_en_nombre(b.date_vol);
    case TRI_PRIX_CROISS:   return a.prix_total < b.prix_total;
    case TRI_PRIX_DECROISS: return a.prix_total > b.prix_total;
    case TRI_NOM_AZ:        return strcmp(a.nom, b.nom) < 0;
    case TRI_NOM_ZA:        return strcmp(a.nom, b.nom) > 0;
    default:                return strcmp(a.id, b.id) < 0;
    }
}

void trier_reservations(Reservation tab[], int n, int critere)
{
    int i, j;
    Reservation aux;
    for (i = 0; i < n - 1; i++)
        for (j = 0; j < n - 1 - i; j++)
            if (avant(tab[j + 1], tab[j], critere))
            {
                aux = tab[j];
                tab[j] = tab[j + 1];
                tab[j + 1] = aux;
            }
}


int que_des_chiffres(char *s, int longueur)
{
    int i;
    if ((int)strlen(s) != longueur)
        return 0;
    for (i = 0; i < longueur; i++)
        if (s[i] < '0' || s[i] > '9')
            return 0;
    return 1;
}

/* 1 = correct, sinon 0 et le message d'erreur dans message */
int controle_saisie(Reservation r, char *message)
{
    double h = heures_avant_depart(r);
    strcpy(message, "");

    if (strlen(r.id) == 0)
        strcpy(message, "Le numero de reservation est obligatoire.");
    else if (!que_des_chiffres(r.cin, 8))
        strcpy(message, "Le CIN doit contenir 8 chiffres.");
    else if (strlen(r.nom) < 3)
        strcpy(message, "Le nom du passager est obligatoire.");
    else if (strlen(r.tel) > 0 && !que_des_chiffres(r.tel, 8))
        strcpy(message, "Le telephone doit contenir 8 chiffres.");
    else if (strlen(r.email) > 0 && (strchr(r.email, '@') == NULL || strchr(r.email, '.') == NULL))
        strcpy(message, "Adresse e-mail invalide.");
    else if (strlen(r.vol) == 0)
        strcpy(message, "Le numero de vol est obligatoire.");
    else if (r.date_vol.jour < 1 || r.date_vol.jour > 31 || r.date_vol.mois < 1 || r.date_vol.mois > 12
             || r.date_vol.annee < 2026)
        strcpy(message, "La date du vol est invalide.");
    else if (r.h_depart.heure > 23 || r.h_depart.minute > 59 || r.h_arrivee.heure > 23 || r.h_arrivee.minute > 59)
        strcpy(message, "Horaire invalide.");
    else if (r.prix_base <= 0)
        strcpy(message, "Le prix du vol doit etre positif.");
    else if (r.type_passager != PASSAGER_BEBE && !siege_valide(r.classe, r.siege))
        strcpy(message, "Siege invalide pour cette classe.");
    else if (r.type_passager != PASSAGER_ADULTE && atoi(r.siege) == RANG_ISSUE_SECOURS)
        strcpy(message, "La rangee de l'issue de secours est reservee aux adultes.");
    else if (r.services.bagage_sup && (r.services.nb_bagages < 1 || r.services.nb_bagages > 5))
        strcpy(message, "Bagages supplementaires : entre 1 et 5.");
    else if (r.services.kg_excedent < 0 || r.services.kg_excedent > 30)
        strcpy(message, "Excedent de bagage : entre 0 et 30 kg.");
    else if (r.services.repas && h > 0 && h < 48)
        strcpy(message, "Un repas special se commande au moins 48 h avant le depart.");

    return strlen(message) == 0;
}

int siege_valide(int classe, char *siege)
{
    int rang = atoi(siege);
    char lettre;
    if (strlen(siege) < 2)
        return 0;
    lettre = siege[strlen(siege) - 1];
    if (lettre < 'A' || lettre > 'F')
        return 0;
    if (classe == CLASSE_PREMIERE)
        return rang >= 1 && rang <= 2 && lettre != 'B' && lettre != 'E';
    if (classe == CLASSE_AFFAIRES)
        return rang >= 3 && rang <= 5 && lettre != 'B' && lettre != 'E';
    return rang >= 6 && rang <= 14;
}

/* 1 si le siege est deja pris sur ce vol ce jour-la (on ignore la reservation id_exclu) */
int siege_occupe(char *fichier, char *vol, Date d, char *siege, char *id_exclu)
{
    Reservation r;
    FILE *f = fopen(fichier, "r");
    if (f == NULL)
        return 0;
    while (lire_ligne(f, &r))
    {
        if (r.statut != STATUT_ANNULEE && r.statut != STATUT_LISTE_ATTENTE && strcmp(r.id, id_exclu) != 0
            && strcmp(r.vol, vol) == 0 && meme_date(r.date_vol, d) && strcmp(r.siege, siege) == 0)
        {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

/* bouton "Choisir pour moi" : premier siege libre de la classe
   preference : 0 indifferent, 1 hublot (A F), 2 couloir (C D), 3 milieu (B E) */
int attribuer_siege_auto(char *fichier, Reservation r, int preference, char *siege)
{
    char lettres[] = "ABCDEF";
    char essai[12];
    int debut = 6, fin = 14, rang, i;
    char l;

    if (r.classe == CLASSE_PREMIERE) { debut = 1; fin = 2; }
    if (r.classe == CLASSE_AFFAIRES) { debut = 3; fin = 5; }

    for (rang = debut; rang <= fin; rang++)
    {
        if (rang == RANG_ISSUE_SECOURS)
            continue;                                 /* rangee payante */
        for (i = 0; i < 6; i++)
        {
            l = lettres[i];
            if (r.classe != CLASSE_ECO && (l == 'B' || l == 'E'))
                continue;
            if (preference == PREF_HUBLOT && l != 'A' && l != 'F')
                continue;
            if (preference == PREF_COULOIR && l != 'C' && l != 'D')
                continue;
            if (preference == PREF_MILIEU && l != 'B' && l != 'E')
                continue;
            sprintf(essai, "%d%c", rang, l);
            if (!siege_occupe(fichier, r.vol, r.date_vol, essai, r.id))
            {
                strcpy(siege, essai);
                return 1;
            }
        }
    }
    return 0;                                         /* avion plein */
}

/* option 1 : 1 si le passager a deja un vol qui chevauche celui-ci
   (la reservation genante est copiee dans *conflit pour la fenetre de conflit) */
int verifier_conflit(char *fichier, Reservation nouv, Reservation *conflit)
{
    Reservation r;
    time_t debut1, fin1, debut2, fin2;
    FILE *f = fopen(fichier, "r");
    if (f == NULL)
        return 0;

    debut1 = en_secondes(nouv.date_vol, nouv.h_depart);
    fin1 = en_secondes(nouv.date_vol, nouv.h_arrivee);
    if (fin1 <= debut1)
        fin1 = fin1 + 24 * 3600;                      /* arrivee le lendemain */

    while (lire_ligne(f, &r))
    {
        if (strcmp(r.cin, nouv.cin) != 0 || strcmp(r.id, nouv.id) == 0 || r.statut == STATUT_ANNULEE)
            continue;
        debut2 = en_secondes(r.date_vol, r.h_depart);
        fin2 = en_secondes(r.date_vol, r.h_arrivee);
        if (fin2 <= debut2)
            fin2 = fin2 + 24 * 3600;
        if (debut1 < fin2 && debut2 < fin1)           /* les deux vols se croisent */
        {
            *conflit = r;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}


/* franchise incluse : Eco 1 x 23 kg, Affaires 2 x 32 kg, Premiere 3 x 32 kg */
float franchise_bagage_kg(int classe, int *nb_pieces)
{
    if (classe == CLASSE_PREMIERE)
    {
        *nb_pieces = 3;
        return 32;
    }
    if (classe == CLASSE_AFFAIRES)
    {
        *nb_pieces = 2;
        return 32;
    }
    *nb_pieces = 1;
    return 23;
}

/* bouton "Appliquer" du code promo. 1 = code valide */
int appliquer_code_promo(char *code, int classe, float *taux, float *montant_fixe)
{
    *taux = 0;
    *montant_fixe = 0;
    if (strcmp(code, "CARTHAGE10") == 0)
        *taux = 0.10;
    else if (strcmp(code, "ETUDIANT15") == 0 && classe == CLASSE_ECO)
        *taux = 0.15;
    else if (strcmp(code, "BIENVENUE") == 0)
        *montant_fixe = 20;
    else
        return 0;
    return 1;
}

DetailPrix calculer_prix(Reservation *r)
{
    DetailPrix d;
    Services s = r->services;
    float taux_promo, promo_fixe;

    memset(&d, 0, sizeof(d));

    
    d.billet = r->prix_base * coef_classe[r->classe];
    d.reduction_type = d.billet - d.billet * coef_passager[r->type_passager];

    /* siege a l'issue de secours */
    if (atoi(r->siege) == RANG_ISSUE_SECOURS)
        d.siege = PRIX_ISSUE_SECOURS;

    
    if (s.repas)
        d.restauration = prix_repas[s.type_repas];
    d.restauration = d.restauration + prix_boisson[s.boisson] + s.dessert * 6;


    if (s.bagage_sup)
        d.bagages = s.nb_bagages * 60;
    d.bagages = d.bagages + s.kg_excedent * 8 + prix_special[s.bagage_special];
    
    if (s.pmr && r->niveau_fidelite < NIVEAU_GOLD)
        d.assistance = 20;
    if (s.umnr)
        d.assistance = d.assistance + 80;

    if (r->classe == CLASSE_ECO)
        d.extras = s.priorite * 20 + s.salon * 45;
    if (r->classe != CLASSE_PREMIERE)
        d.extras = d.extras + s.wifi * 10 + s.assurance * 25;

    /* sous-total, remises, taxes */
    d.sous_total = d.billet - d.reduction_type + d.siege + d.restauration + d.bagages + d.assistance + d.extras;
    d.remise_fidelite = d.sous_total * remise_niveau[r->niveau_fidelite];
    appliquer_code_promo(r->code_promo, r->classe, &taux_promo, &promo_fixe);
    d.remise_promo = (d.sous_total - d.remise_fidelite) * taux_promo + promo_fixe;
    d.taxes = TAXES_AEROPORT;

    d.total = d.sous_total - d.remise_fidelite - d.remise_promo;
    if (d.total < 0)
        d.total = 0;
    d.total = d.total + d.taxes;
    d.points = (int)(d.total / 10);                   /* 1 point par tranche de 10 DT */

    r->prix_total = d.total;
    r->points = d.points;
    return d;
}


double heures_avant_depart(Reservation r)
{
    return difftime(en_secondes(r.date_vol, r.h_depart), time(NULL)) / 3600;
}

/* option 2 : 100 % a plus de 48 h, 50 % entre 24 et 48 h, 0 % a moins de 24 h,
   100 % avec l'assurance (incluse en Premiere). Ne modifie pas le fichier. */
float calculer_remboursement(Reservation r, int *taux)
{
    double h = heures_avant_depart(r);

    if (h <= 0)
        *taux = 0;                                    /* vol deja parti */
    else if (r.services.assurance || r.classe == CLASSE_PREMIERE || h > 48)
        *taux = 100;
    else if (h >= 24)
        *taux = 50;
    else
        *taux = 0;

    if (r.paye == 0)
        return 0;
    return r.prix_total * (*taux) / 100;
}

/*    1 = annulee, 0 = introuvable, -1 = deja annulee */
int annuler_reservation(char *fichier, char *id, float *montant)
{
    Reservation r;
    int taux;
    if (!rechercher_reservation(fichier, id, &r))
        return 0;
    if (r.statut == STATUT_ANNULEE)
        return -1;
    *montant = calculer_remboursement(r, &taux);
    r.montant_rembourse = *montant;
    r.statut = STATUT_ANNULEE;
    modifier_reservation(fichier, id, r);
    promouvoir_liste_attente(fichier, r.vol, r.date_vol);
    return 1;
}


int promouvoir_liste_attente(char *fichier, char *vol, Date d)
{
    Reservation r;
    FILE *f = fopen(fichier, "r");
    if (f == NULL)
        return 0;
    while (lire_ligne(f, &r))
    {
        if (r.statut == STATUT_LISTE_ATTENTE && strcmp(r.vol, vol) == 0 && meme_date(r.date_vol, d))
        {
            fclose(f);
            r.statut = STATUT_EN_ATTENTE;
            modifier_reservation(fichier, r.id, r);
            return 1;
        }
    }
    fclose(f);
    return 0;
}
/* nouvelle reference unique : RES-2026-0001, RES-2026-0002, ... */
void generer_id(char *fichier, char *id)
{
    Reservation r;
    int annee, numero, max = 0;
    FILE *f = fopen(fichier, "r");
    if (f != NULL)
    {
        while (lire_ligne(f, &r))
            if (sscanf(r.id, "RES-%d-%d", &annee, &numero) == 2 && numero > max)
                max = numero;
        fclose(f);
    }
    sprintf(id, "RES-2026-%04d", max + 1);
}

/* sieges occupes sur un vol (les bebes n'ont pas de siege)
   le module Avions calcule : PlacesDisponibles = CapaciteMax - ce nombre */
int nb_places_reservees(char *fichier, char *vol, Date d)
{
    Reservation r;
    int n = 0;
    FILE *f = fopen(fichier, "r");
    if (f == NULL)
        return 0;
    while (lire_ligne(f, &r))
        if (strcmp(r.vol, vol) == 0 && meme_date(r.date_vol, d) && r.statut != STATUT_ANNULEE
            && r.statut != STATUT_LISTE_ATTENTE && r.type_passager != PASSAGER_BEBE)
            n++;
    fclose(f);
    return n;
}


void calculer_statistiques(char *fichier, Statistiques *s)
{
    Reservation r;
    FILE *f = fopen(fichier, "r");
    memset(s, 0, sizeof(Statistiques));
    if (f == NULL)
        return;
    while (lire_ligne(f, &r))
    {
        s->total++;
        if (r.statut == STATUT_CONFIRMEE)
            s->confirmees++;
        else if (r.statut == STATUT_ANNULEE)
        {
            s->annulees++;
            s->total_rembourse = s->total_rembourse + r.montant_rembourse;
        }
        else
            s->en_attente++;                          
        if (r.statut != STATUT_ANNULEE)
            s->chiffre_affaires = s->chiffre_affaires + r.prix_total;
    }
    fclose(f);
}

/* resume des services (colonne "Services" et apercu) */
void services_en_texte(Reservation r, char *texte)
{
    Services s = r.services;
    char tmp[80];
    strcpy(texte, "");
    if (s.repas)
    {
        sprintf(tmp, "Repas %s, ", libelle_repas(s.type_repas));
        strcat(texte, tmp);
    }
    if (s.bagage_sup)
    {
        sprintf(tmp, "+%d bagage(s), ", s.nb_bagages);
        strcat(texte, tmp);
    }
    if (s.kg_excedent > 0)
    {
        sprintf(tmp, "+%d kg, ", s.kg_excedent);
        strcat(texte, tmp);
    }
    if (s.bagage_special > 0)
    {
        sprintf(tmp, "%s, ", libelle_bagage_special(s.bagage_special));
        strcat(texte, tmp);
    }
    if (s.pmr)
    {
        sprintf(tmp, "PMR %s, ", libelle_pmr(s.type_pmr));
        strcat(texte, tmp);
    }
    if (s.umnr)      strcat(texte, "UMNR, ");
    if (s.priorite)  strcat(texte, "Priorite, ");
    if (s.salon)     strcat(texte, "Salon, ");
    if (s.wifi)      strcat(texte, "Wi-Fi, ");
    if (s.assurance) strcat(texte, "Assurance, ");

    if (strlen(texte) == 0)
        strcpy(texte, "Aucun");
    else
        texte[strlen(texte) - 2] = '\0';              
}

/* bouton "Exporter (.txt)" */
int exporter_reservations(char *fichier, char *fichier_export)
{
    Reservation r;
    Statistiques s;
    int n = 0;
    FILE *f = fopen(fichier, "r");
    FILE *e = fopen(fichier_export, "w");
    if (e == NULL)
        return -1;
    fprintf(e, "CARTHAGE AIRWAYS - LISTE DES RESERVATIONS\n\n");
    if (f != NULL)
    {
        while (lire_ligne(f, &r))
        {
            fprintf(e, "%-14s %-20.20s %-9s %-7s %02d/%02d/%04d %-10s %-5s %-15s %10.3f DT\n",
                    r.id, r.nom, r.cin, r.vol, r.date_vol.jour, r.date_vol.mois, r.date_vol.annee,
                    libelle_classe(r.classe), tiret(r.siege), libelle_statut(r.statut), r.prix_total);
            n++;
        }
        fclose(f);
    }
    calculer_statistiques(fichier, &s);
    fprintf(e, "\nTotal %d | Confirmees %d | En attente %d | Annulees %d\n",
            s.total, s.confirmees, s.en_attente, s.annulees);
    fprintf(e, "Chiffre d'affaires %.3f DT | Rembourse %.3f DT\n", s.chiffre_affaires, s.total_rembourse);
    fclose(e);
    return n;
}

int calculer_zone(Reservation r)
{
    if (r.classe != CLASSE_ECO || r.services.priorite)
        return 1;
    if (atoi(r.siege) <= 9)
        return 2;
    return 3;
}

void calculer_porte(char *vol, char *porte)
{
    int i, somme = 0;
    for (i = 0; vol[i] != '\0'; i++)
        somme = somme + vol[i];
    sprintf(porte, "%c%d", 'A' + somme % 4, 1 + somme % 9);
}

/* carte d'embarquement dans un fichier texte (seulement si confirmee) */
int generer_carte_embarquement(Reservation r, char *fichier_carte)
{
    char porte[8];
    int embarquement = r.h_depart.heure * 60 + r.h_depart.minute - 45;   /* 45 min avant */
    FILE *f;

    if (r.statut != STATUT_CONFIRMEE)
        return 0;
    f = fopen(fichier_carte, "w");
    if (f == NULL)
        return -1;
    if (embarquement < 0)
        embarquement = embarquement + 24 * 60;
    calculer_porte(r.vol, porte);

    fprintf(f, "========= CARTHAGE AIRWAYS - CARTE D'EMBARQUEMENT =========\n");
    fprintf(f, "Passager     : %s\n", r.nom);
    fprintf(f, "Trajet       : %s  >>>  %s\n", r.depart, r.arrivee);
    fprintf(f, "Vol          : %s   le %02d/%02d/%04d\n", r.vol, r.date_vol.jour, r.date_vol.mois, r.date_vol.annee);
    fprintf(f, "Embarquement : %02d:%02d   Porte %s   Zone %d\n", embarquement / 60, embarquement % 60,
            porte, calculer_zone(r));
    fprintf(f, "Siege        : %s   Classe %s\n", tiret(r.siege), libelle_classe(r.classe));
    fprintf(f, "Reference    : %s\n", r.id);
    fprintf(f, "  ||| || ||||| || ||| |||| ||  |||| || ||| || ||| |||\n");
    fprintf(f, "===========================================================\n");
    fclose(f);
    return 1;
}



const char *libelle_classe(int c)
{
    char *t[] = {"Economique", "Affaires", "Premiere"};
    if (c < 0 || c > 2) return "?";
    return t[c];
}

const char *libelle_type_passager(int c)
{
    char *t[] = {"Adulte", "Enfant", "Bebe"};
    if (c < 0 || c > 2) return "?";
    return t[c];
}

const char *libelle_statut(int c)
{
    char *t[] = {"Confirmee", "En attente", "Liste d'attente", "Annulee"};
    if (c < 0 || c > 3) return "?";
    return t[c];
}

const char *libelle_paiement(int c)
{
    char *t[] = {"Carte bancaire", "Especes", "Virement", "Points fidelite"};
    if (c < 0 || c > 3) return "?";
    return t[c];
}

const char *libelle_niveau(int c)
{
    char *t[] = {"Standard", "Silver", "Gold", "Platinum"};
    if (c < 0 || c > 3) return "?";
    return t[c];
}

const char *libelle_repas(int c)
{
    if (c < 0 || c > 11) return "?";
    return noms_repas[c];
}

const char *libelle_boisson(int c)
{
    if (c < 0 || c > 3) return "?";
    return noms_boisson[c];
}

const char *libelle_bagage_special(int c)
{
    if (c < 0 || c > 5) return "?";
    return noms_special[c];
}

const char *libelle_pmr(int c)
{
    if (c < 0 || c > 6) return "?";
    return noms_pmr[c];
}
