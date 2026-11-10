#ifndef PASSAGER_H_INCLUDED
#define PASSAGER_H_INCLUDED
#include <stdio.h>

typedef struct
{
    char cin[9];
    char nom[30];
    char prenom[30];
    int jour, mois, annee;
    char sexe[10];
    char ville[30];
    char email[50];
    char telephone[15];
    int pmr;
    int newsletter;
    int points;
} passager;

int ajouter(char *, passager);      /* 1 ok, 0 erreur fichier, -1 CIN existe deja */
int modifier(char *, char *, passager);
int supprimer(char *, char *);
passager chercher(char *, char *);  /* cin vide si introuvable */

int trier(char *, int);             /* ordre : 1 croissant, 0 decroissant */

void niveau_fidelite(int, char *);
int remise_fidelite(int);
int ajouter_points(char *, char *, float);

int valider_cin(char *);
int valider_telephone(char *);
int valider_email(char *);
int valider_date(int, int, int);

#endif
