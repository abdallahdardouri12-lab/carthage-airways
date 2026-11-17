#include <stdio.h>
#include <string.h>
#include "utilisateur.h"

static int lire_utilisateur(FILE *f, Utilisateur *u)
{
    return fscanf(f, " %29[^;];%29[^;];%19s", u->entry_login_user, u->entry_login_mdp, u->role) == 3;
}

static void ecrire_utilisateur(FILE *f, Utilisateur u)
{
    fprintf(f, "%s;%s;%s\n", u.entry_login_user, u.entry_login_mdp, u.role);
}

int verifier_connexion(char *fichier, char *login, char *mdp, char *role)
{
    Utilisateur u;
    FILE *f = fopen(fichier, "r");
    if (f == NULL)
        return 0;
    while (lire_utilisateur(f, &u))
    {
        if (strcmp(u.entry_login_user, login) == 0 && strcmp(u.entry_login_mdp, mdp) == 0)
        {
            strcpy(role, u.role);
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int login_utilisateur_existe(char *fichier, char *login)
{
    Utilisateur u;
    FILE *f = fopen(fichier, "r");
    if (f == NULL)
        return 0;
    while (lire_utilisateur(f, &u))
    {
        if (strcmp(u.entry_login_user, login) == 0)
        {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

static int texte_valide(char *t)
{
    return strlen(t) > 0 && strchr(t, ';') == NULL && strchr(t, ' ') == NULL;
}

int controle_saisie_utilisateur(char *fichier, Utilisateur u)
{
    if (!texte_valide(u.entry_login_user)) return 1;
    if (strlen(u.entry_login_mdp) < 4 || !texte_valide(u.entry_login_mdp)) return 2;
    if (strcmp(u.role, "admin") != 0 && strcmp(u.role, "personnel") != 0
        && strcmp(u.role, "passager") != 0) return 3;
    if (login_utilisateur_existe(fichier, u.entry_login_user)) return 4;
    return 0;
}

int ajouter_utilisateur(char *fichier, Utilisateur u)
{
    FILE *f;
    if (controle_saisie_utilisateur(fichier, u) != 0)
        return 0;
    f = fopen(fichier, "a");
    if (f == NULL)
        return 0;
    ecrire_utilisateur(f, u);
    fclose(f);
    return 1;
}

int supprimer_utilisateur(char *fichier, char *login)
{
    Utilisateur u;
    int trouve = 0;
    FILE *f = fopen(fichier, "r");
    FILE *aux = fopen("aux_utilisateurs.txt", "w");
    if (f == NULL || aux == NULL)
    {
        if (f) fclose(f);
        if (aux) fclose(aux);
        return 0;
    }
    while (lire_utilisateur(f, &u))
    {
        if (strcmp(u.entry_login_user, login) == 0)
            trouve = 1;
        else
            ecrire_utilisateur(aux, u);
    }
    fclose(f);
    fclose(aux);
    remove(fichier);
    rename("aux_utilisateurs.txt", fichier);
    return trouve;
}

int modifier_mdp_utilisateur(char *fichier, char *login, char *ancien_mdp, char *nouveau_mdp)
{
    Utilisateur u;
    int trouve = 0;
    FILE *f, *aux;
    if (strlen(nouveau_mdp) < 4 || !texte_valide(nouveau_mdp))
        return 0;
    f = fopen(fichier, "r");
    aux = fopen("aux_utilisateurs.txt", "w");
    if (f == NULL || aux == NULL)
    {
        if (f) fclose(f);
        if (aux) fclose(aux);
        return 0;
    }
    while (lire_utilisateur(f, &u))
    {
        if (strcmp(u.entry_login_user, login) == 0 && strcmp(u.entry_login_mdp, ancien_mdp) == 0)
        {
            strcpy(u.entry_login_mdp, nouveau_mdp);
            trouve = 1;
        }
        ecrire_utilisateur(aux, u);
    }
    fclose(f);
    fclose(aux);
    remove(fichier);
    rename("aux_utilisateurs.txt", fichier);
    return trouve;
}
