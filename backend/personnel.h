#ifndef PERSONNEL_H_INCLUDED
#define PERSONNEL_H_INCLUDED

typedef struct
{
    int pers_jour;
    int pers_mois;
    int pers_annee;
} DatePersonnel;

typedef struct
{
    char id[20];
    char login[30];
    char nom[30];
    char prenom[30];
    char cin[9];
    DatePersonnel date_naissance;
    char fonction[20];
    char grade[40];
    char base[40];
    DatePersonnel date_embauche;
    char telephone[9];
    char mail[50];
    int check_disponible;
    float heures_vol;
} Personnel;

typedef struct
{
    char id_personnel[20];
    char numero_vol[20];
    DatePersonnel date_vol;
    float duree;
    int statut;
} AffectationPersonnel;

int ajouter_personnel(char *fichier, Personnel p);
int modifier_personnel(char *fichier, char *id, Personnel nouveau);
int supprimer_personnel(char *fichier, char *id);
int chercher_personnel(char *fichier, char *id, Personnel *resultat);
int chercher_personnel_par_login(char *fichier, char *login, Personnel *resultat);
int id_personnel_existe(char *fichier, char *id);
int login_personnel_existe(char *fichier, char *login);

int filtrer_personnel(char *fichier, char *fonction, char *base,
                      int disponible, char *fichier_resultat);

int controle_saisie_personnel(char *fichier, Personnel p);

int duree_experience_personnel(DatePersonnel embauche, DatePersonnel aujourdhui);

int ajouter_affectation_personnel(char *fichier, AffectationPersonnel a);
int modifier_statut_affectation_personnel(char *fichier, char *id_personnel,
                                char *numero_vol, int statut);

int planning_personnel(char *fichier, char *id_personnel, char *fichier_planning);

void temps_de_vol_personnel(char *fichier, char *id_personnel, DatePersonnel aujourdhui,
                  float *temps_vol, float *temps_restant);

void statistiques_personnel(char *fichier, int *total, int *pilotes,
                            int *copilotes, int *cabine, int *disponibles);

#endif
