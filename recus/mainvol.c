/* Jeu d'essai du module Vol.
   Aucun affichage : on verifie le resultat directement dans les fichiers texte
   vols.txt et resultat_filtre.txt */
#include <stdio.h>
#include <string.h>
#include "vol.h"

Vol creer_vol(char *numero, char *depart, char *destination, int j, int m, int a,
              int hd, int md, int ha, int ma, int passagers)
{
    Vol v;
    strcpy(v.numero, numero);
    strcpy(v.depart, depart);
    strcpy(v.destination, destination);
    v.date.jour = j;
    v.date.mois = m;
    v.date.annee = a;
    v.heure_dep.heure = hd;
    v.heure_dep.minute = md;
    v.heure_arr.heure = ha;
    v.heure_arr.minute = ma;
    v.passagers = passagers;
    strcpy(v.avion, "-");
    strcpy(v.statut, "Programme");
    v.duree = 0;
    return v;
}

int main()
{
    Vol v;
    Date d;
    FILE *f;

    f = fopen("vols.txt", "w");      /* on repart d'un fichier vide */
    fclose(f);

    /* ajout d'un vol */
    ajouter_vol("vols.txt", creer_vol("VL001", "Tunis", "Paris", 12, 11, 2026, 8, 30, 11, 0, 150));

    /* affectation d'un avion */
    affecter_avion("vols.txt", "VL001", "avions.txt", "TS-IMA");

    /* modification : VL001 passe a 100 passagers et part a 8h45 */
    modifier_vol("vols.txt", creer_vol("VL001", "Tunis", "Paris", 12, 11, 2026, 8, 45, 11, 15, 100));

    /* recherche du vol VL001 */
    v = chercher_vol("vols.txt", "VL001");

    /* filtre : vols au depart de Tunis vers Paris le 12/11/2026 -> resultat_filtre.txt */
    d.jour = 12; d.mois = 11; d.annee = 2026;
    filtrer_vols("vols.txt", "resultat_filtre.txt", "Tunis", "Paris", d, 1);

    /* annulation du vol VL001 */
    annuler_vol("vols.txt", "VL001");

    /* suppression du vol VL001 */
    supprimer_vol("vols.txt", "VL001");

    return 0;
}
