#include <stdio.h>
#include <string.h>
#include "vol.h"

/* Format d'une ligne du fichier des vols :
   numero depart destination jj/mm/aaaa hh:mm hh:mm duree passagers avion statut
   exemple : VL001 Tunis Paris 12/11/2026 08:30 11:00 150 150 TS-IMA Programme */

/* lit un vol dans le fichier : retourne 1 si la lecture a reussi */
int lire_vol(FILE *f, Vol *v)
{
    int r = fscanf(f, "%s %s %s %d/%d/%d %d:%d %d:%d %d %d %s %s",
                   v->numero, v->depart, v->destination,
                   &v->date.jour, &v->date.mois, &v->date.annee,
                   &v->heure_dep.heure, &v->heure_dep.minute,
                   &v->heure_arr.heure, &v->heure_arr.minute,
                   &v->duree, &v->passagers, v->avion, v->statut);
    return (r == 14);
}

/* ecrit un vol dans le fichier */
void ecrire_vol(FILE *f, Vol v)
{
    fprintf(f, "%s %s %s %02d/%02d/%04d %02d:%02d %02d:%02d %d %d %s %s\n",
            v.numero, v.depart, v.destination,
            v.date.jour, v.date.mois, v.date.annee,
            v.heure_dep.heure, v.heure_dep.minute,
            v.heure_arr.heure, v.heure_arr.minute,
            v.duree, v.passagers, v.avion, v.statut);
}

/* ---------------- calculs et controles ---------------- */

/* duree en minutes, -1 si l'arrivee n'est pas apres le depart */
int calculer_duree(Heure dep, Heure arr)
{
    int d = dep.heure * 60 + dep.minute;
    int a = arr.heure * 60 + arr.minute;
    if (a <= d)
        return -1;
    return a - d;
}

int jours_du_mois(int mois, int annee)
{
    if (mois == 2) {
        if ((annee % 4 == 0 && annee % 100 != 0) || annee % 400 == 0)
            return 29;
        return 28;
    }
    if (mois == 4 || mois == 6 || mois == 9 || mois == 11)
        return 30;
    return 31;
}

/* controle de saisie : 1 si tout est correct, 0 sinon */
int verifier_saisie(Vol v)
{
    if (strlen(v.numero) == 0 || strlen(v.depart) == 0 || strlen(v.destination) == 0)
        return 0;
    if (strchr(v.numero, ' ') || strchr(v.depart, ' ') || strchr(v.destination, ' '))
        return 0;
    if (v.passagers <= 0)
        return 0;
    if (v.date.mois < 1 || v.date.mois > 12 || v.date.annee < 2000)
        return 0;
    if (v.date.jour < 1 || v.date.jour > jours_du_mois(v.date.mois, v.date.annee))
        return 0;
    if (v.heure_dep.heure < 0 || v.heure_dep.heure > 23 || v.heure_dep.minute < 0 || v.heure_dep.minute > 59)
        return 0;
    if (v.heure_arr.heure < 0 || v.heure_arr.heure > 23 || v.heure_arr.minute < 0 || v.heure_arr.minute > 59)
        return 0;
    return 1;
}

/* fichier des aeroports : code ville ouvert (1 ou 0), exemple : TUN Tunis 1 */
int aeroport_ouvert(char *fichier_aeroports, char *ville)
{
    FILE *f = fopen(fichier_aeroports, "r");
    char code[10], nom[30];
    int ouvert, resultat = 0;

    if (f == NULL)
        return 0;
    while (fscanf(f, "%s %s %d", code, nom, &ouvert) == 3) {
        if (strcmp(nom, ville) == 0)
            resultat = ouvert;
    }
    fclose(f);
    return resultat;
}

/* controles de coherence (option 1) :
   1 = coherent, -1 = villes identiques, -2 = arrivee avant le depart, -3 = aeroport ferme ou inconnu */
int verifier_coherence(Vol v, char *fichier_aeroports)
{
    if (strcmp(v.depart, v.destination) == 0)
        return -1;
    if (calculer_duree(v.heure_dep, v.heure_arr) == -1)
        return -2;
    if (aeroport_ouvert(fichier_aeroports, v.depart) == 0 || aeroport_ouvert(fichier_aeroports, v.destination) == 0)
        return -3;
    return 1;
}

/* ---------------- gestion des vols ---------------- */

/* retourne 1 : ajoute, 0 : erreur de fichier, -1 : numero deja utilise,
   -2 : saisie invalide, -3 : arrivee avant le depart */
int ajouter_vol(char *fichier, Vol v)
{
    FILE *f;
    Vol t;

    if (verifier_saisie(v) == 0)
        return -2;
    v.duree = calculer_duree(v.heure_dep, v.heure_arr);
    if (v.duree == -1)
        return -3;

    f = fopen(fichier, "r");
    if (f != NULL) {
        while (lire_vol(f, &t)) {
            if (strcmp(t.numero, v.numero) == 0) {
                fclose(f);
                return -1;
            }
        }
        fclose(f);
    }

    strcpy(v.avion, "-");
    strcpy(v.statut, "Programme");
    f = fopen(fichier, "a");
    if (f == NULL)
        return 0;
    ecrire_vol(f, v);
    fclose(f);
    return 1;
}

/* retourne 1 : modifie, 0 : vol introuvable ou erreur, -2 : saisie invalide, -3 : arrivee avant le depart */
int modifier_vol(char *fichier, Vol v)
{
    FILE *f, *temp;
    Vol t;
    int trouve = 0;

    if (verifier_saisie(v) == 0)
        return -2;
    v.duree = calculer_duree(v.heure_dep, v.heure_arr);
    if (v.duree == -1)
        return -3;

    f = fopen(fichier, "r");
    temp = fopen("temp.txt", "w");
    if (f == NULL || temp == NULL) {
        if (f) fclose(f);
        if (temp) fclose(temp);
        return 0;
    }
    while (lire_vol(f, &t)) {
        if (strcmp(t.numero, v.numero) == 0) {
            strcpy(v.avion, t.avion);       /* l'avion et le statut ne changent pas */
            strcpy(v.statut, t.statut);
            ecrire_vol(temp, v);
            trouve = 1;
        } else {
            ecrire_vol(temp, t);
        }
    }
    fclose(f);
    fclose(temp);
    remove(fichier);
    rename("temp.txt", fichier);
    return trouve;
}

/* retourne 1 : supprime, 0 : vol introuvable ou erreur */
int supprimer_vol(char *fichier, char *numero)
{
    FILE *f, *temp;
    Vol t;
    int trouve = 0;

    f = fopen(fichier, "r");
    temp = fopen("temp.txt", "w");
    if (f == NULL || temp == NULL) {
        if (f) fclose(f);
        if (temp) fclose(temp);
        return 0;
    }
    while (lire_vol(f, &t)) {
        if (strcmp(t.numero, numero) == 0)
            trouve = 1;
        else
            ecrire_vol(temp, t);
    }
    fclose(f);
    fclose(temp);
    remove(fichier);
    rename("temp.txt", fichier);
    return trouve;
}

/* retourne le vol trouve ; si le vol n'existe pas, son numero est vide */
Vol chercher_vol(char *fichier, char *numero)
{
    FILE *f = fopen(fichier, "r");
    Vol v, vide;

    strcpy(vide.numero, "");
    if (f == NULL)
        return vide;
    while (lire_vol(f, &v)) {
        if (strcmp(v.numero, numero) == 0) {
            fclose(f);
            return v;
        }
    }
    fclose(f);
    return vide;
}

/* ecrit dans le fichier "resultat" les vols qui correspondent aux criteres.
   Un critere est ignore si depart ou destination est "" ou si date.jour vaut 0.
   Si exclure_annules vaut 1, les vols annules ne sont pas gardes.
   retourne le nombre de vols trouves, -1 si erreur de fichier */
int filtrer_vols(char *fichier, char *resultat, char *depart, char *destination, Date date, int exclure_annules)
{
    FILE *f = fopen(fichier, "r");
    FILE *r = fopen(resultat, "w");
    Vol v;
    int nb = 0;

    if (r == NULL) {
        if (f) fclose(f);
        return -1;
    }
    if (f == NULL) {
        fclose(r);
        return 0;
    }
    while (lire_vol(f, &v)) {
        if (strlen(depart) > 0 && strcmp(v.depart, depart) != 0)
            continue;
        if (strlen(destination) > 0 && strcmp(v.destination, destination) != 0)
            continue;
        if (date.jour != 0 && (v.date.jour != date.jour || v.date.mois != date.mois || v.date.annee != date.annee))
            continue;
        if (exclure_annules == 1 && strcmp(v.statut, "Annule") == 0)
            continue;
        ecrire_vol(r, v);
        nb++;
    }
    fclose(f);
    fclose(r);
    return nb;
}

/* ---------------- affectation d'un avion ---------------- */

/* fichier des avions : immatriculation modele capacite etat, exemple : TS-IMA Airbus_A320 180 Operationnel
   retourne la capacite de l'avion, -1 si l'avion n'existe pas */
int capacite_avion(char *fichier_avions, char *id_avion)
{
    FILE *f = fopen(fichier_avions, "r");
    char id[15], modele[30], etat[20];
    int capacite;

    if (f == NULL)
        return -1;
    while (fscanf(f, "%s %s %d %s", id, modele, &capacite, etat) == 4) {
        if (strcmp(id, id_avion) == 0) {
            fclose(f);
            return capacite;
        }
    }
    fclose(f);
    return -1;
}

/* retourne 1 : avion affecte, 0 : vol introuvable, -1 : avion introuvable,
   -2 : capacite insuffisante pour les passagers prevus */
int affecter_avion(char *fichier_vols, char *numero, char *fichier_avions, char *id_avion)
{
    FILE *f, *temp;
    Vol t;
    int capacite;

    t = chercher_vol(fichier_vols, numero);
    if (strlen(t.numero) == 0)
        return 0;
    capacite = capacite_avion(fichier_avions, id_avion);
    if (capacite == -1)
        return -1;
    if (capacite < t.passagers)
        return -2;

    f = fopen(fichier_vols, "r");
    temp = fopen("temp.txt", "w");
    if (f == NULL || temp == NULL) {
        if (f) fclose(f);
        if (temp) fclose(temp);
        return 0;
    }
    while (lire_vol(f, &t)) {
        if (strcmp(t.numero, numero) == 0)
            strcpy(t.avion, id_avion);
        ecrire_vol(temp, t);
    }
    fclose(f);
    fclose(temp);
    remove(fichier_vols);
    rename("temp.txt", fichier_vols);
    return 1;
}

/* ---------------- annulation ---------------- */

/* le vol passe a "Annule" et son avion est libere
   retourne 1 : annule, 0 : vol introuvable, -1 : vol deja annule */
int annuler_vol(char *fichier, char *numero)
{
    FILE *f, *temp;
    Vol t;
    int resultat = 0;

    f = fopen(fichier, "r");
    temp = fopen("temp.txt", "w");
    if (f == NULL || temp == NULL) {
        if (f) fclose(f);
        if (temp) fclose(temp);
        return 0;
    }
    while (lire_vol(f, &t)) {
        if (strcmp(t.numero, numero) == 0) {
            if (strcmp(t.statut, "Annule") == 0) {
                resultat = -1;
            } else {
                strcpy(t.statut, "Annule");
                strcpy(t.avion, "-");
                resultat = 1;
            }
        }
        ecrire_vol(temp, t);
    }
    fclose(f);
    fclose(temp);
    remove(fichier);
    rename("temp.txt", fichier);
    return resultat;
}
