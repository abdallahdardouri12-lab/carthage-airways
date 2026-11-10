#ifndef RESERVATION_H_INCLUDED
#define RESERVATION_H_INCLUDED


#define FICHIER_RESERVATIONS  "reservations.txt"
#define FICHIER_TEMPORAIRE    "reservations_tmp.txt"
#define MAX_RESERVATIONS      500
#define TAXES_AEROPORT        35.0f     
#define RANG_ISSUE_SECOURS    10        
#define PRIX_ISSUE_SECOURS    35.0f


#define CLASSE_ECO       0
#define CLASSE_AFFAIRES  1
#define CLASSE_PREMIERE  2
#define PASSAGER_ADULTE  0
#define PASSAGER_ENFANT  1
#define PASSAGER_BEBE    2

#define STATUT_CONFIRMEE      0
#define STATUT_EN_ATTENTE     1
#define STATUT_LISTE_ATTENTE  2
#define STATUT_ANNULEE        3

#define NIVEAU_STANDARD  0
#define NIVEAU_SILVER    1
#define NIVEAU_GOLD      2
#define NIVEAU_PLATINUM  3

#define PAIEMENT_CARTE     0
#define PAIEMENT_ESPECES   1
#define PAIEMENT_VIREMENT  2
#define PAIEMENT_POINTS    3


#define PREF_INDIFFERENT 0
#define PREF_HUBLOT      1
#define PREF_COULOIR     2
#define PREF_MILIEU      3


#define TRI_DATE_RECENT   0
#define TRI_DATE_ANCIEN   1
#define TRI_PRIX_CROISS   2
#define TRI_PRIX_DECROISS 3
#define TRI_NOM_AZ        4
#define TRI_NOM_ZA        5
#define TRI_REFERENCE     6


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
    
    int  repas;              
    int  type_repas;         
    int  boisson;            
    int  dessert;            
    char allergies[50];

    
    int  bagage_sup;         
    int  nb_bagages;         
    int  kg_excedent;        
    int  bagage_special;     

    int  pmr;                
    int  type_pmr;           
    int  fauteuil_perso;
    int  chien_assistance;
    int  umnr;               /* mineur                      */

    
    int  priorite;
    int  salon;
    int  wifi;
    int  assurance;
    int  sms;
} Services;

typedef struct
{
    char  id[20];            
    char  cin[9];            
    char  nom[50];
    char  tel[9];
    char  email[50];

    char  vol[10];           
    char  depart[30];        
    char  arrivee[30];
    Date  date_vol;
    Heure h_depart;
    Heure h_arrivee;

    int   classe;            
    int   type_passager;     
    char  siege[5];          
    Services services;

    float prix_base;        
    char  code_promo[20];
    int   niveau_fidelite;   
    int   mode_paiement;
    int   paye;              
    int   statut;            

    float prix_total;        
    int   points;            
    float montant_rembourse; 
} Reservation;


typedef struct
{
    float billet;
    float reduction_type;    
    float siege;
    float restauration;
    float bagages;
    float assistance;
    float extras;
    float sous_total;
    float remise_fidelite;
    float remise_promo;
    float taxes;
    float total;
    int   points;
} DetailPrix;


typedef struct
{
    char  mot_cle[50];       
    int   classe;           
    int   statut;            
    char  vol[10];           
    int   paiement;          
    int   filtre_periode;                    
    Date  du;
    Date  au;
    float prix_max;          
} Filtre;


typedef struct
{
    int   total;
    int   confirmees;
    int   en_attente;        
    int   annulees;
    float chiffre_affaires;  
    float total_rembourse;
} Statistiques;

int  ajouter_reservation(char *fichier, Reservation r);
int  modifier_reservation(char *fichier, char *id, Reservation nouv);
int  supprimer_reservation(char *fichier, char *id);
int  rechercher_reservation(char *fichier, char *id, Reservation *r);
int  existe_reservation(char *fichier, char *id);
int  charger_reservations(char *fichier, Reservation tab[], int max);
void afficher_reservations(char *fichier);
void afficher_reservation(Reservation r);

int  filtrer_reservations(char *fichier, Filtre f, Reservation tab[], int max);
int  reservations_passager(char *fichier, char *cin, Reservation tab[], int max);
void trier_reservations(Reservation tab[], int n, int critere);
void filtre_par_defaut(Filtre *f);

int  controle_saisie(Reservation r, char *message);
int  siege_valide(int classe, char *siege);
int  siege_occupe(char *fichier, char *vol, Date d, char *siege, char *id_exclu);
int  attribuer_siege_auto(char *fichier, Reservation r, int preference, char *siege);
int  verifier_conflit(char *fichier, Reservation nouv, Reservation *conflit);


DetailPrix calculer_prix(Reservation *r);
int  appliquer_code_promo(char *code, int classe, float *taux, float *montant_fixe);
float franchise_bagage_kg(int classe, int *nb_pieces);


double heures_avant_depart(Reservation r);
float  calculer_remboursement(Reservation r, int *taux);
int    annuler_reservation(char *fichier, char *id, float *montant);
int    promouvoir_liste_attente(char *fichier, char *vol, Date d);


void generer_id(char *fichier, char *id);
int  nb_places_reservees(char *fichier, char *vol, Date d);
void calculer_statistiques(char *fichier, Statistiques *s);
int  exporter_reservations(char *fichier, char *fichier_export);
int  generer_carte_embarquement(Reservation r, char *fichier_carte);
int  calculer_zone(Reservation r);
void calculer_porte(char *vol, char *porte);
void services_en_texte(Reservation r, char *texte);


const char *libelle_classe(int classe);
const char *libelle_type_passager(int type);
const char *libelle_statut(int statut);
const char *libelle_repas(int type);
const char *libelle_boisson(int b);
const char *libelle_bagage_special(int b);
const char *libelle_pmr(int t);
const char *libelle_paiement(int m);
const char *libelle_niveau(int n);

#endif 
