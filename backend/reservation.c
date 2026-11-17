#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "reservation.h"

#define NB_CHAMPS 18  

static int lire_reservation(FILE *f, Reservation *r)
{
    return fscanf(f, "%19s %19s %19s %d %d %d %d %d %d %d %19s %5s %d %d %d %f %f %19s",
                  r->id, r->id_passager, r->id_vol,
                  &r->date_vol.jour, &r->date_vol.mois, &r->date_vol.annee,
                  &r->heure_depart.heure, &r->heure_depart.minute,
                  &r->heure_arrivee.heure, &r->heure_arrivee.minute,
                  r->classe, r->siege, &r->repas, &r->pmr, &r->nb_bagages,
                  &r->prix_base, &r->prix_total, r->statut) == NB_CHAMPS;
}

static void ecrire_reservation(FILE *f, Reservation r)
{
    fprintf(f, "%s %s %s %d %d %d %d %d %d %d %s %s %d %d %d %.2f %.2f %s\n",
            r.id, r.id_passager, r.id_vol,
            r.date_vol.jour, r.date_vol.mois, r.date_vol.annee,
            r.heure_depart.heure, r.heure_depart.minute,
            r.heure_arrivee.heure, r.heure_arrivee.minute,
            r.classe, r.siege, r.repas, r.pmr, r.nb_bagages,
            r.prix_base, r.prix_total, r.statut);
}

static int critere_ok(char *critere, char *valeur)
{
    if (critere == NULL || strlen(critere) == 0) return 1;
    if (strcmp(critere, "Toutes") == 0 || strcmp(critere, "Tous") == 0) return 1;
    return strcmp(critere, valeur) == 0;
}


static void majuscule(char *s)
{
    int i;
    for (i = 0; s[i] != '\0'; i++)
        s[i] = toupper((unsigned char) s[i]);
}

static time_t vers_time(Date d, Heure h)
{
    struct tm t;
    memset(&t, 0, sizeof(t));
    t.tm_mday = d.jour;
    t.tm_mon  = d.mois - 1;
    t.tm_year = d.annee - 1900;
    t.tm_hour = h.heure;
    t.tm_min  = h.minute;
    t.tm_isdst = -1;
    return mktime(&t);
}


int ajouter_reservation(char *fichier, Reservation r)
{
    FILE *f;

    majuscule(r.siege);
    if (existe_reservation(fichier, r.id))   
        return -1;
    if (doublon_passager_vol(fichier, r.id_passager, r.id_vol, ""))
        return -2;
    if (!siege_disponible(fichier, r.id_vol, r.siege, "")) 
        return -3;

    r.prix_total = calculer_prix(r.prix_base, r.classe, r.repas, r.pmr, r.nb_bagages);

    f = fopen(fichier, "a");
    if (f == NULL) 
       return 0;
    ecrire_reservation(f, r);
    fclose(f);
    return 1;
}

/* ============================================================
 *  2. MODIFIER (fichier temporaire puis rename)
 *  retour :  1 OK, 0 introuvable / erreur, -2 doublon, -3 siege pris
 * ============================================================ */
int modifier_reservation(char *fichier, char *id, Reservation nouv)
{
    FILE *f, *tmp;
    Reservation r;
    int trouve = 0;

    majuscule(nouv.siege);
    if (!existe_reservation(fichier, id))
       return 0;
    if (doublon_passager_vol(fichier, nouv.id_passager, nouv.id_vol, id))
       return -2;
    if (!siege_disponible(fichier, nouv.id_vol, nouv.siege, id))          
       return -3;

    nouv.prix_total = calculer_prix(nouv.prix_base, nouv.classe, nouv.repas,
                                    nouv.pmr, nouv.nb_bagages);
    strcpy(nouv.id, id);                       

    f   = fopen(fichier, "r");
    tmp = fopen("temp.txt", "w");
    if (f == NULL || tmp == NULL)
    {
        if (f) fclose(f);
        if (tmp) fclose(tmp);
        return 0;
    }
    while (lire_reservation(f, &r))
    {
        if (strcmp(r.id, id) == 0)
        {
            ecrire_reservation(tmp, nouv);
            trouve = 1;
        }
        else
            ecrire_reservation(tmp, r);
    }
    fclose(f);
    fclose(tmp);
    remove(fichier);
    rename("temp.txt", fichier);
    return trouve;
}

int supprimer_reservation(char *fichier, char *id)
{
    FILE *f, *tmp;
    Reservation r;
    int trouve = 0;

    f   = fopen(fichier, "r");
    tmp = fopen("temp.txt", "w");
    if (f == NULL || tmp == NULL)
    {
        if (f) fclose(f);
        if (tmp) fclose(tmp);
        return 0;
    }
    while (lire_reservation(f, &r))
    {
        if (strcmp(r.id, id) == 0)
            trouve = 1;                 
        else
            ecrire_reservation(tmp, r);
    }
    fclose(f);
    fclose(tmp);
    remove(fichier);
    rename("temp.txt", fichier);
    return trouve;
}

int chercher_reservation(char *fichier, char *id, Reservation *res)
{
    FILE *f = fopen(fichier, "r");
    Reservation r;

    if (f == NULL)
      return 0;
    while (lire_reservation(f, &r))
    {
        if (strcmp(r.id, id) == 0)
        {
            *res = r;
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
    return chercher_reservation(fichier, id, &r);
}

int afficher_reservations(char *fichier)
{
    FILE *f = fopen(fichier, "r");
    Reservation r;
    int n = 0;

    if (f == NULL)
    {
        printf("Aucune reservation (fichier introuvable).\n");
        return 0;
    }
    printf("\n %-7s %-10s %-7s %-10s %-5s %-5s %-11s %-5s %-4s %-4s %-3s %9s %-10s\n",
           "ID", "Passager", "Vol", "Date", "Dep", "Arr", "Classe", "Siege",
           "Rep", "PMR", "Bag", "Total", "Statut");
    printf("------------------------------------------------------------------------------------\n");
    while (lire_reservation(f, &r))
    {
        printf("%-7s %-10s %-7s %02d/%02d/%04d %02d:%02d %02d:%02d %-11s %-5s %-4s %-4s %-3d %9.2f %-10s\n",
               r.id, r.id_passager, r.id_vol,
               r.date_vol.jour, r.date_vol.mois, r.date_vol.annee,
               r.heure_depart.heure, r.heure_depart.minute,
               r.heure_arrivee.heure, r.heure_arrivee.minute,
               r.classe, r.siege, r.repas ? "Oui" : "Non", r.pmr ? "Oui" : "Non",
               r.nb_bagages, r.prix_total, r.statut);
        n++;
    }
    fclose(f);
    printf("=> %d reservation(s)\n", n);
    return n;
}


int filtrer_reservations(char *fichier_src, char *fichier_dest,
                         char *id_passager, char *id_vol,
                         char *classe, char *statut)
{
    FILE *f, *d;
    Reservation r;
    int n = 0;

    f = fopen(fichier_src, "r");
    d = fopen(fichier_dest, "w");
    if (d == NULL)
    {
        if (f) fclose(f);
        return 0;
    }
    if (f == NULL)
    {
        fclose(d);
        return 0;
    }
    while (lire_reservation(f, &r))
    {
        if (critere_ok(id_passager, r.id_passager) &&
            critere_ok(id_vol, r.id_vol) &&
            critere_ok(classe, r.classe) &&
            critere_ok(statut, r.statut))
        {
            ecrire_reservation(d, r);
            n++;
        }
    }
    fclose(f);
    fclose(d);
    return n;
}

/* 1 si le passager a deja une reservation active sur ce vol */
int doublon_passager_vol(char *fichier, char *id_passager, char *id_vol, char *id_exclu)
{
    FILE *f = fopen(fichier, "r");
    Reservation r;

    if (f == NULL) return 0;
    while (lire_reservation(f, &r))
    {
        if (strcmp(r.id, id_exclu) != 0 &&
            strcmp(r.id_passager, id_passager) == 0 &&
            strcmp(r.id_vol, id_vol) == 0 &&
            strcmp(r.statut, "Annulee") != 0)
        {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

/* 1 si le siege est libre sur ce vol (les reservations annulees liberent le siege) */
int siege_disponible(char *fichier, char *id_vol, char *siege, char *id_exclu)
{
    FILE *f = fopen(fichier, "r");
    Reservation r;
    char s[6];

    strncpy(s, siege, 5);
    s[5] = '\0';
    majuscule(s);
    if (f == NULL) return 1;
    while (lire_reservation(f, &r))
    {
        if (strcmp(r.id, id_exclu) != 0 &&
            strcmp(r.id_vol, id_vol) == 0 &&
            strcmp(r.siege, s) == 0 &&
            strcmp(r.statut, "Annulee") != 0)
        {
            fclose(f);
            return 0;
        }
    }
    fclose(f);
    return 1;
}

/* RES001, RES002 ... -> retourne le suivant */
void generer_id(char *fichier, char *id)
{
    FILE *f = fopen(fichier, "r");
    Reservation r;
    int num, max = 0;

    if (f != NULL)
    {
        while (lire_reservation(f, &r))
        {
            if (sscanf(r.id, "RES%d", &num) == 1 && num > max)
                max = num;
        }
        fclose(f);
    }
    sprintf(id, "RES%03d", max + 1);
}

/* CONTROLES DE SAISIE*/
int date_valide(Date d)
{
    int jours[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (d.annee < 2000 || d.annee > 2100) return 0;
    if (d.mois < 1 || d.mois > 12)        return 0;
    if ((d.annee % 4 == 0 && d.annee % 100 != 0) || d.annee % 400 == 0)
        jours[1] = 29;
    return d.jour >= 1 && d.jour <= jours[d.mois - 1];
}

/* siege = rang 1..60 + lettre A..F  (ex : 1A, 12C, 60F) */
int siege_valide(char *siege)
{
    int i = 0, rang = 0;
    char lettre;

    if (siege == NULL || strlen(siege) < 2 || strlen(siege) > 3) return 0;
    while (isdigit((unsigned char) siege[i]))
    {
        rang = rang * 10 + (siege[i] - '0');
        i++;
    }
    if (i == 0 || siege[i] == '\0' || siege[i + 1] != '\0') return 0;
    lettre = toupper((unsigned char) siege[i]);
    return rang >= 1 && rang <= 60 && lettre >= 'A' && lettre <= 'F';
}

/* retourne 1 si tout est correct, sinon 0 + message d'erreur */
int controle_saisie(Reservation r, char *message)
{
    strcpy(message, "");
    if (strlen(r.id) == 0)            { strcpy(message, "L'identifiant de la reservation est obligatoire."); return 0; }
    if (strchr(r.id, ' '))            { strcpy(message, "L'identifiant ne doit pas contenir d'espace."); return 0; }
    if (strlen(r.id_passager) == 0)   { strcpy(message, "L'identifiant du passager est obligatoire."); return 0; }
    if (strlen(r.id_vol) == 0)        { strcpy(message, "Le numero de vol est obligatoire."); return 0; }
    if (!date_valide(r.date_vol))     { strcpy(message, "La date du vol est invalide."); return 0; }
    if (r.heure_depart.heure < 0  || r.heure_depart.heure > 23 ||
        r.heure_depart.minute < 0 || r.heure_depart.minute > 59 ||
        r.heure_arrivee.heure < 0 || r.heure_arrivee.heure > 23 ||
        r.heure_arrivee.minute < 0 || r.heure_arrivee.minute > 59)
                                      { strcpy(message, "Les horaires sont invalides."); return 0; }
    if (strcmp(r.classe, "Economique") != 0 && strcmp(r.classe, "Affaires") != 0 &&
        strcmp(r.classe, "Premiere") != 0)
                                      { strcpy(message, "Veuillez choisir une classe."); return 0; }
    if (!siege_valide(r.siege))       { strcpy(message, "Siege invalide (ex : 12A, rang 1-60, lettre A-F)."); return 0; }
    if (r.nb_bagages < 0 || r.nb_bagages > MAX_BAGAGES)
                                      { strcpy(message, "Nombre de bagages supplementaires : 0 a 3."); return 0; }
    if (r.prix_base <= 0)             { strcpy(message, "Le prix de base doit etre positif."); return 0; }
    if (strcmp(r.statut, "Confirmee") != 0 && strcmp(r.statut, "En_attente") != 0 &&
        strcmp(r.statut, "Annulee") != 0)
                                      { strcpy(message, "Statut invalide."); return 0; }
    return 1;
}

/* 9. FONCTION SPECIFIQUE : calcul automatique du prix total
     total = prix_base x coef_classe + repas + PMR + bagages*/
float calculer_prix(float prix_base, char *classe, int repas, int pmr, int nb_bagages)
{
    float total;
    float coef = COEF_ECONOMIQUE;

    if (strcmp(classe, "Affaires") == 0)      coef = COEF_AFFAIRES;
    else if (strcmp(classe, "Premiere") == 0) coef = COEF_PREMIERE;

    total = prix_base * coef;
    if (repas) total += PRIX_REPAS;
    if (pmr)   total += PRIX_PMR;
    total += nb_bagages * PRIX_BAGAGE;
    return total;
}

/* ============================================================
 *  10. LIEN AVEC LE MODULE VOLS
 *  Format suppose de vols.txt :
 *  id ville_depart destination jj mm aaaa hD mD hA mA prix
 *  (a adapter au format choisi par l'etudiant "Gestion des vols")
 * ============================================================ */
int chercher_vol(char *fichier_vols, char *id_vol, Vol *v)
{
    FILE *f = fopen(fichier_vols, "r");
    Vol x;

    if (f == NULL) return 0;
    while (fscanf(f, "%19s %29s %29s %d %d %d %d %d %d %d %f",
                  x.id, x.ville_depart, x.destination,
                  &x.date.jour, &x.date.mois, &x.date.annee,
                  &x.heure_depart.heure, &x.heure_depart.minute,
                  &x.heure_arrivee.heure, &x.heure_arrivee.minute, &x.prix) == 11)
    {
        if (strcmp(x.id, id_vol) == 0)
        {
            *v = x;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

/* copie la date, les horaires et le prix du vol dans la reservation */
int charger_infos_vol(char *fichier_vols, Reservation *r)
{
    Vol v;
    if (!chercher_vol(fichier_vols, r->id_vol, &v)) return 0;
    r->date_vol      = v.date;
    r->heure_depart  = v.heure_depart;
    r->heure_arrivee = v.heure_arrivee;
    r->prix_base     = v.prix;
    return 1;
}

/*  11. ESPACE PASSAGER */
int reserver_vol(char *fichier, char *fichier_vols, Reservation *r, char *message)
{
    char id_conflit[20];
    int res;

    generer_id(fichier, r->id);
    strcpy(r->statut, "Confirmee");
    majuscule(r->siege);

    if (!charger_infos_vol(fichier_vols, r))
    {
        strcpy(message, "Vol introuvable.");
        return 0;
    }
    if (!controle_saisie(*r, message)) return 0;

    if (detecter_conflit(fichier, *r, id_conflit))
    {
        sprintf(message, "Conflit d'horaire avec votre reservation %s.", id_conflit);
        return 0;
    }
    r->prix_total = calculer_prix(r->prix_base, r->classe, r->repas, r->pmr, r->nb_bagages);

    res = ajouter_reservation(fichier, *r);
    if (res == -2) { strcpy(message, "Vous avez deja une reservation sur ce vol."); return 0; }
    if (res == -3) { sprintf(message, "Le siege %s est deja pris sur ce vol.", r->siege); return 0; }
    if (res != 1)  { strcpy(message, "Erreur d'enregistrement."); return 0; }

    sprintf(message, "Reservation %s confirmee. Prix total : %.2f DT", r->id, r->prix_total);
    return 1;
}

int mes_reservations(char *fichier_src, char *fichier_dest, char *id_passager)
{
    return filtrer_reservations(fichier_src, fichier_dest, id_passager, "", "", "");
}


int modifier_siege(char *fichier, char *id, char *id_passager, char *nouveau_siege)
{
    Reservation r;

    if (!chercher_reservation(fichier, id, &r))       return 0;
    if (strcmp(r.id_passager, id_passager) != 0)       return -1;
    if (strcmp(r.statut, "Annulee") == 0)              return -4;
    if (!siege_valide(nouveau_siege))                  return -2;
    if (!siege_disponible(fichier, r.id_vol, nouveau_siege, id)) return -3;

    strcpy(r.siege, nouveau_siege);
    return modifier_reservation(fichier, id, r);
}


 /*  12. OPTION 1 : detection de conflits d'horaire*/
int detecter_conflit(char *fichier, Reservation r, char *id_conflit)
{
    FILE *f = fopen(fichier, "r");
    Reservation x;
    time_t dep1, arr1, dep2, arr2;

    if (f == NULL) return 0;

    dep1 = vers_time(r.date_vol, r.heure_depart);
    arr1 = vers_time(r.date_vol, r.heure_arrivee);
    if (arr1 <= dep1) arr1 += 24 * 3600;          /* arrivee le lendemain */

    while (lire_reservation(f, &x))
    {
        if (strcmp(x.id, r.id) == 0) continue;
        if (strcmp(x.id_passager, r.id_passager) != 0) continue;
        if (strcmp(x.statut, "Annulee") == 0) continue;

        dep2 = vers_time(x.date_vol, x.heure_depart);
        arr2 = vers_time(x.date_vol, x.heure_arrivee);
        if (arr2 <= dep2) arr2 += 24 * 3600;

        if (dep1 < arr2 && dep2 < arr1)          /* intervalles qui se chevauchent */
        {
            strcpy(id_conflit, x.id);
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

/* ============================================================
 *  13. OPTION 2 : annulation et remboursement
 *   > 48 h avant le depart : 100 %
 *   24 h a 48 h            :  50 %
 *   < 24 h                 :   0 %
 * ============================================================ */
float calculer_remboursement(Reservation r, Date auj, Heure maintenant)
{
    double heures = difftime(vers_time(r.date_vol, r.heure_depart),
                             vers_time(auj, maintenant)) / 3600.0;

    if (heures > 48)  return r.prix_total;
    if (heures >= 24) return r.prix_total * 0.5f;
    return 0;
}

/* retour : montant rembourse (>= 0), -1 introuvable / pas sa reservation,
 *          -2 deja annulee */
float annuler_reservation(char *fichier, char *id, char *id_passager)
{
    Reservation r;
    Date auj;
    Heure maintenant;
    time_t t = time(NULL);
    struct tm *now = localtime(&t);
    float montant;

    if (!chercher_reservation(fichier, id, &r)) return -1;
    if (id_passager != NULL && strlen(id_passager) > 0 &&
        strcmp(r.id_passager, id_passager) != 0)  return -1;
    if (strcmp(r.statut, "Annulee") == 0)         return -2;

    auj.jour = now->tm_mday;
    auj.mois = now->tm_mon + 1;
    auj.annee = now->tm_year + 1900;
    maintenant.heure = now->tm_hour;
    maintenant.minute = now->tm_min;

    montant = calculer_remboursement(r, auj, maintenant);
    strcpy(r.statut, "Annulee");
    modifier_reservation(fichier, id, r);
    return montant;
}
