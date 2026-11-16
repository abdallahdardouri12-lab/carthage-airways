#ifndef PASSAGER_H_INCLUDED
#define PASSAGER_H_INCLUDED
#include <stdio.h>

typedef struct
{
    int jour;
    int mois;
    int annee;
} Date;

typedef struct
{
    char cin[9];
    char nom[30];
    char prenom[30];
    Date date_naissance;
    char sexe[10];
    char ville[30];
    char email[50];
    char telephone[15];
    int pmr;
    int newsletter;
    int points;
} passager;




int ajouter(char *, passager);      
int modifier(char *, char *, passager);
int supprimer(char *, char *);
passager chercher(char *, char *);  

int rechercher(char *, char *, char *); 
int trier(char *, int);             

void niveau_fidelite(int, char *);
int remise_fidelite(int);
int ajouter_points(char *, char *, float);

int valider_cin(char *);
int valider_telephone(char *);
int valider_email(char *);
int valider_date(Date);

#endif
