#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "personnel.h"

static int lire_personnel(FILE *f, Personnel *p)
{
    return fscanf(f, " %19[^;];%29[^;];%29[^;];%29[^;];%8[^;];%d;%d;%d;%19[^;];%39[^;];%39[^;];%d;%d;%d;%8[^;];%49[^;];%d;%f",
        p->id, p->login, p->nom, p->prenom, p->cin,
        &p->date_naissance.pers_jour, &p->date_naissance.pers_mois, &p->date_naissance.pers_annee,
        p->fonction, p->grade, p->base,
        &p->date_embauche.pers_jour, &p->date_embauche.pers_mois, &p->date_embauche.pers_annee,
        p->telephone, p->mail, &p->check_disponible, &p->heures_vol) == 18;
}

static void ecrire_personnel(FILE *f, Personnel p)
{
    fprintf(f, "%s;%s;%s;%s;%s;%d;%d;%d;%s;%s;%s;%d;%d;%d;%s;%s;%d;%.1f\n",
        p.id, p.login, p.nom, p.prenom, p.cin,
        p.date_naissance.pers_jour, p.date_naissance.pers_mois, p.date_naissance.pers_annee,
        p.fonction, p.grade, p.base,
        p.date_embauche.pers_jour, p.date_embauche.pers_mois, p.date_embauche.pers_annee,
        p.telephone, p.mail, p.check_disponible, p.heures_vol);
}

int chercher_personnel(char *fichier, char *id, Personnel *resultat)
{
    Personnel p;
    FILE *f = fopen(fichier, "r");
    if (f == NULL) return 0;
    while (lire_personnel(f, &p))
    {
        if (strcmp(p.id, id) == 0)
        {
            *resultat = p;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int chercher_personnel_par_login(char *fichier, char *login, Personnel *resultat)
{
    Personnel p;
    FILE *f = fopen(fichier, "r");
    if (f == NULL) return 0;
    while (lire_personnel(f, &p))
    {
        if (strcmp(p.login, login) == 0)
        {
            *resultat = p;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int id_personnel_existe(char *fichier, char *id)
{
    Personnel p;
    return chercher_personnel(fichier, id, &p);
}

int login_personnel_existe(char *fichier, char *login)
{
    Personnel p;
    return chercher_personnel_par_login(fichier, login, &p);
}

int ajouter_personnel(char *fichier, Personnel p)
{
    FILE *f;
    if (id_personnel_existe(fichier, p.id) || login_personnel_existe(fichier, p.login))
        return 0;
    f = fopen(fichier, "a");
    if (f == NULL) return 0;
    ecrire_personnel(f, p);
    fclose(f);
    return 1;
}

int modifier_personnel(char *fichier, char *id, Personnel nouveau)
{
    Personnel p, autre;
    int trouve = 0;
    FILE *f, *aux;

    strcpy(nouveau.id, id);
    if (chercher_personnel_par_login(fichier, nouveau.login, &autre)
        && strcmp(autre.id, id) != 0)
        return 0;

    f = fopen(fichier, "r");
    aux = fopen("aux_personnel.txt", "w");
    if (f == NULL || aux == NULL)
    {
        if (f) fclose(f);
        if (aux) fclose(aux);
        return 0;
    }
    while (lire_personnel(f, &p))
    {
        if (strcmp(p.id, id) == 0)
        {
            ecrire_personnel(aux, nouveau);
            trouve = 1;
        }
        else
            ecrire_personnel(aux, p);
    }
    fclose(f);
    fclose(aux);
    remove(fichier);
    rename("aux_personnel.txt", fichier);
    return trouve;
}

int supprimer_personnel(char *fichier, char *id)
{
    Personnel p;
    int trouve = 0;
    FILE *f = fopen(fichier, "r");
    FILE *aux = fopen("aux_personnel.txt", "w");
    if (f == NULL || aux == NULL)
    {
        if (f) fclose(f);
        if (aux) fclose(aux);
        return 0;
    }
    while (lire_personnel(f, &p))
    {
        if (strcmp(p.id, id) == 0)
            trouve = 1;
        else
            ecrire_personnel(aux, p);
    }
    fclose(f);
    fclose(aux);
    remove(fichier);
    rename("aux_personnel.txt", fichier);
    return trouve;
}

int filtrer_personnel(char *fichier, char *fonction, char *base,
                      int disponible, char *fichier_resultat)
{
    Personnel p;
    int n = 0;
    FILE *f = fopen(fichier, "r");
    FILE *res = fopen(fichier_resultat, "w");
    if (f == NULL || res == NULL)
    {
        if (f) fclose(f);
        if (res) fclose(res);
        return 0;
    }
    while (lire_personnel(f, &p))
    {
        if (strcmp(fonction, "") != 0 && strcmp(p.fonction, fonction) != 0) continue;
        if (strcmp(base, "") != 0 && strcmp(p.base, base) != 0) continue;
        if (disponible != -1 && p.check_disponible != disponible) continue;
        ecrire_personnel(res, p);
        n++;
    }
    fclose(f);
    fclose(res);
    return n;
}

static int que_des_chiffres(char *ch, int longueur)
{
    int i;
    if ((int)strlen(ch) != longueur) return 0;
    for (i = 0; i < longueur; i++)
        if (!isdigit((unsigned char)ch[i])) return 0;
    return 1;
}

static int email_valide(char *mail)
{
    char *arobase = strchr(mail, '@');
    char *point;
    if (arobase == NULL || arobase == mail) return 0;
    point = strrchr(arobase, '.');
    return point != NULL && point > arobase + 1 && point[1] != '\0';
}

static int date_valide(DatePersonnel d)
{
    int jours[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (d.pers_annee < 1900 || d.pers_mois < 1 || d.pers_mois > 12) return 0;
    if ((d.pers_annee % 4 == 0 && d.pers_annee % 100 != 0) || d.pers_annee % 400 == 0)
        jours[1] = 29;
    return d.pers_jour >= 1 && d.pers_jour <= jours[d.pers_mois - 1];
}

int controle_saisie_personnel(char *fichier, Personnel p)
{
    if (strlen(p.id) == 0 || strlen(p.login) == 0 || strlen(p.nom) == 0
        || strlen(p.prenom) == 0 || strlen(p.fonction) == 0 || strlen(p.base) == 0)
        return 1;
    if (!que_des_chiffres(p.telephone, 8)) return 2;
    if (!email_valide(p.mail)) return 3;
    if (!que_des_chiffres(p.cin, 8)) return 4;
    if (!date_valide(p.date_naissance) || !date_valide(p.date_embauche)) return 5;
    if (id_personnel_existe(fichier, p.id)) return 6;
    if (login_personnel_existe(fichier, p.login)) return 7;
    return 0;
}

int duree_experience_personnel(DatePersonnel embauche, DatePersonnel aujourdhui)
{
    int ans = aujourdhui.pers_annee - embauche.pers_annee;
    if (aujourdhui.pers_mois < embauche.pers_mois
        || (aujourdhui.pers_mois == embauche.pers_mois
            && aujourdhui.pers_jour < embauche.pers_jour))
        ans--;
    if (ans < 0) ans = 0;
    return ans;
}

static int comparer_dates(DatePersonnel a, DatePersonnel b)
{
    long x = a.pers_annee * 10000L + a.pers_mois * 100 + a.pers_jour;
    long y = b.pers_annee * 10000L + b.pers_mois * 100 + b.pers_jour;
    return (x < y) ? -1 : (x > y);
}

static int lire_affectation_personnel(FILE *f, AffectationPersonnel *a)
{
    return fscanf(f, " %19[^;];%19[^;];%d;%d;%d;%f;%d", a->id_personnel, a->numero_vol,
                  &a->date_vol.pers_jour, &a->date_vol.pers_mois,
                  &a->date_vol.pers_annee, &a->duree, &a->statut) == 7;
}

static void ecrire_affectation_personnel(FILE *f, AffectationPersonnel a)
{
    fprintf(f, "%s;%s;%d;%d;%d;%.2f;%d\n", a.id_personnel, a.numero_vol,
            a.date_vol.pers_jour, a.date_vol.pers_mois, a.date_vol.pers_annee,
            a.duree, a.statut);
}

int ajouter_affectation_personnel(char *fichier, AffectationPersonnel a)
{
    AffectationPersonnel x;
    FILE *f = fopen(fichier, "r");
    if (f != NULL)
    {
        while (lire_affectation_personnel(f, &x))
            if (strcmp(x.id_personnel, a.id_personnel) == 0
                && strcmp(x.numero_vol, a.numero_vol) == 0 && x.statut != 3)
            {
                fclose(f);
                return 0;
            }
        fclose(f);
    }
    f = fopen(fichier, "a");
    if (f == NULL) return 0;
    ecrire_affectation_personnel(f, a);
    fclose(f);
    return 1;
}

int modifier_statut_affectation_personnel(char *fichier, char *id_personnel, char *numero_vol, int statut)
{
    AffectationPersonnel a;
    int trouve = 0;
    FILE *f = fopen(fichier, "r");
    FILE *aux = fopen("aux_affectations.txt", "w");
    if (f == NULL || aux == NULL)
    {
        if (f) fclose(f);
        if (aux) fclose(aux);
        return 0;
    }
    while (lire_affectation_personnel(f, &a))
    {
        if (strcmp(a.id_personnel, id_personnel) == 0 && strcmp(a.numero_vol, numero_vol) == 0)
        {
            a.statut = statut;
            trouve = 1;
        }
        ecrire_affectation_personnel(aux, a);
    }
    fclose(f);
    fclose(aux);
    remove(fichier);
    rename("aux_affectations.txt", fichier);
    return trouve;
}

int planning_personnel(char *fichier, char *id_personnel, char *fichier_planning)
{
    AffectationPersonnel a;
    int n = 0;
    FILE *f = fopen(fichier, "r");
    FILE *res = fopen(fichier_planning, "w");
    if (f == NULL || res == NULL)
    {
        if (f) fclose(f);
        if (res) fclose(res);
        return 0;
    }
    while (lire_affectation_personnel(f, &a))
        if (strcmp(a.id_personnel, id_personnel) == 0)
        {
            ecrire_affectation_personnel(res, a);
            n++;
        }
    fclose(f);
    fclose(res);
    return n;
}

void temps_de_vol_personnel(char *fichier, char *id_personnel, DatePersonnel aujourdhui,
                  float *temps_vol, float *temps_restant)
{
    AffectationPersonnel a;
    FILE *f = fopen(fichier, "r");
    *temps_vol = 0;
    *temps_restant = 0;
    if (f == NULL) return;
    while (lire_affectation_personnel(f, &a))
    {
        if (strcmp(a.id_personnel, id_personnel) != 0 || a.statut == 3)
            continue;
        if (comparer_dates(a.date_vol, aujourdhui) < 0)
        {
            if (a.statut == 1) *temps_vol += a.duree;
        }
        else
            *temps_restant += a.duree;
    }
    fclose(f);
}

void statistiques_personnel(char *fichier, int *total, int *pilotes,
                            int *copilotes, int *cabine, int *disponibles)
{
    Personnel p;
    FILE *f = fopen(fichier, "r");
    *total = *pilotes = *copilotes = *cabine = *disponibles = 0;
    if (f == NULL) return;
    while (lire_personnel(f, &p))
    {
        (*total)++;
        if (strcmp(p.fonction, "Pilote") == 0) (*pilotes)++;
        else if (strcmp(p.fonction, "Copilote") == 0) (*copilotes)++;
        else (*cabine)++;
        if (p.check_disponible == 1) (*disponibles)++;
    }
    fclose(f);
}
