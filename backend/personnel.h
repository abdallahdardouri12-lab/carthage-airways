#ifndef PERSONNEL_H_INCLUDED
#define PERSONNEL_H_INCLUDED


typedef struct
{
    int spin_pers_jour;
    int spin_pers_mois;
    int spin_pers_annee;
} Date;

typedef struct
{
    char entry_id[20];
    char entry_login[30];
    char entry_pers_nom[30];
    char entry_pers_prenom[30];
    char entry_pers_cin[9];
    Date date_naissance;
    char combo_fonction[20];
    char label_pers_grade[40];
    char combo_base[40];
    Date date_embauche;
    char entry_pers_telephone[9];
    char entry_pers_mail[50];
    int check_disponible;
    float heures_vol;
} Personnel;

typedef struct
{
    char id_personnel[20];
    char numero_vol[20];
    Date date_vol;
    float duree;            
    int statut;              
} Affectation;


int ajouter_personnel(char *fichier, Personnel p);
int modifier_personnel(char *fichier, char *id, Personnel nouveau);
int supprimer_personnel(char *fichier, char *id);
int chercher_personnel(char *fichier, char *id, Personnel *resultat);
int chercher_par_login(char *fichier, char *login, Personnel *resultat);
int id_existe(char *fichier, char *id);
int login_existe(char *fichier, char *login);


int filtrer_personnel(char *fichier, char *fonction, char *base,
                      int disponible, char *fichier_resultat);


int controle_saisie(char *fichier, Personnel p);

int duree_experience(Date embauche, Date aujourdhui);  

int ajouter_affectation(char *fichier, Affectation a);
int modifier_statut_affectation(char *fichier, char *id_personnel,
                                char *numero_vol, int statut);

int planning_personnel(char *fichier, char *id_personnel, char *fichier_planning);

void temps_de_vol(char *fichier, char *id_personnel, Date aujourdhui,
                  float *temps_vol, float *temps_restant);

void statistiques_personnel(char *fichier, int *total, int *pilotes,
                            int *copilotes, int *cabine, int *disponibles);

#endif
