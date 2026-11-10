#ifndef VOL_H
#define VOL_H

typedef struct {
    int jour;
    int mois;
    int annee;
} Date;

typedef struct {
    int heure;
    int minute;
} Heure;

typedef struct {
    char numero[10];        /* numero du vol (unique)            */
    char depart[30];        /* ville de depart, ex : Tunis       */
    char destination[30];   /* ville d'arrivee, ex : Paris       */
    Date date;
    Heure heure_dep;
    Heure heure_arr;
    int duree;              /* en minutes, calculee automatiquement */
    int passagers;          /* passagers prevus                  */
    char avion[15];         /* immatriculation, "-" si aucun avion */
    char statut[15];        /* Programme ou Annule               */
} Vol;

/* gestion des vols */
int ajouter_vol(char *fichier, Vol v);
int modifier_vol(char *fichier, Vol v);
int supprimer_vol(char *fichier, char *numero);
Vol chercher_vol(char *fichier, char *numero);
int filtrer_vols(char *fichier, char *resultat, char *depart, char *destination, Date date, int exclure_annules);

/* calculs et controles */
int calculer_duree(Heure dep, Heure arr);
int verifier_saisie(Vol v);
int verifier_coherence(Vol v, char *fichier_aeroports);

/* affectation d'un avion */
int capacite_avion(char *fichier_avions, char *id_avion);
int affecter_avion(char *fichier_vols, char *numero, char *fichier_avions, char *id_avion);

/* annulation */
int annuler_vol(char *fichier, char *numero);

#endif
