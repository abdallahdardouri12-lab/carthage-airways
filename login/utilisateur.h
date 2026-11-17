#ifndef UTILISATEUR_H_INCLUDED
#define UTILISATEUR_H_INCLUDED

typedef struct
{
    char entry_login_user[30];
    char entry_login_mdp[30];
    char role[20];
} Utilisateur;

int verifier_connexion(char *fichier, char *login, char *mdp, char *role);
int login_utilisateur_existe(char *fichier, char *login);
int controle_saisie_utilisateur(char *fichier, Utilisateur u);
int ajouter_utilisateur(char *fichier, Utilisateur u);
int supprimer_utilisateur(char *fichier, char *login);
int modifier_mdp_utilisateur(char *fichier, char *login, char *ancien_mdp, char *nouveau_mdp);

#endif
