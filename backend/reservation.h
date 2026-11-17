
#ifndef RESERVATION_H_INCLUDED
#define RESERVATION_H_INCLUDED


#define FICHIER_RESERVATIONS "reservations.txt"
#define FICHIER_VOLS         "vols.txt"
#define FICHIER_RESULTAT     "resultat_recherche.txt"


#define COEF_ECONOMIQUE   1.0
#define COEF_AFFAIRES     1.8
#define COEF_PREMIERE     2.5
#define PRIX_REPAS        25.0   
#define PRIX_PMR          20.0   
#define PRIX_BAGAGE       60.0   
#define MAX_BAGAGES       3


typedef struct
{
    int jour;
    int mois;
    int annee;
} Date;

typedef struct
{
    int heure;
    int minute;
} Heure;

typedef struct
{
    char  id[20];            /* ex : RES001                      */
    char  id_passager[20];   /* login / CIN du passager           */
    char  id_vol[20];        /* ex : TU712                       */
    Date  date_vol;
    Heure heure_depart;
    Heure heure_arrivee;
    char  classe[20];        /* Economique / Affaires / Premiere */
    char  siege[6];          /* ex : 12A                         */
    int   repas;             /* 1 = oui, 0 = non                 */
    int   pmr;               /* 1 = oui, 0 = non                 */
    int   nb_bagages;        /* bagages supplementaires 0..3     */
    float prix_base;         /* prix du billet en economique     */
    float prix_total;        /* calcule automatiquement          */
    char  statut[20];        /* Confirmee / En_attente / Annulee */
} Reservation;

typedef struct
{
    char  id[20];
    char  ville_depart[30];
    char  destination[30];
    Date  date;
    Heure heure_depart;
    Heure heure_arrivee;
    float prix;
} Vol;
/*CRUD*/
int  ajouter_reservation  (char *fichier, Reservation r);
int  modifier_reservation (char *fichier, char *id, Reservation nouv);
int  supprimer_reservation(char *fichier, char *id);
int  chercher_reservation (char *fichier, char *id, Reservation *r);
int  afficher_reservations(char *fichier);         


int  filtrer_reservations (char *fichier_src, char *fichier_dest,
                           char *id_passager, char *id_vol,
                           char *classe, char *statut);


int  existe_reservation   (char *fichier, char *id);
int  doublon_passager_vol (char *fichier, char *id_passager, char *id_vol, char *id_exclu);
int  siege_disponible     (char *fichier, char *id_vol, char *siege, char *id_exclu);
int  controle_saisie      (Reservation r, char *message);
int  date_valide          (Date d);
int  siege_valide         (char *siege);
void generer_id           (char *fichier, char *id);

/*Fonction specifique : prix*/
float calculer_prix(float prix_base, char *classe, int repas, int pmr, int nb_bagages);

/*Lien avec le module "vols"*/
int  chercher_vol         (char *fichier_vols, char *id_vol, Vol *v);
int  charger_infos_vol    (char *fichier_vols, Reservation *r);

/*Espace passager*/
int  reserver_vol         (char *fichier, char *fichier_vols, Reservation *r, char *message);
int  mes_reservations     (char *fichier_src, char *fichier_dest, char *id_passager);
int  modifier_siege       (char *fichier, char *id, char *id_passager, char *nouveau_siege);

/*Option 1 : detection de conflits*/
int  detecter_conflit     (char *fichier, Reservation r, char *id_conflit);

/*Option 2 : annulation et remboursement*/
float calculer_remboursement(Reservation r, Date auj, Heure maintenant);
float annuler_reservation (char *fichier, char *id, char *id_passager);

#endif 
