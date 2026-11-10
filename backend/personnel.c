#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "personnel.h"


static int lire_personnel(FILE *f, Personnel *p)
{
    return fscanf(f, " %19[^;];%29[^;];%29[^;];%29[^;];%8[^;];%d;%d;%d;%19[^;];%39[^;];%39[^;];%d;%d;%d;%8[^;];%49[^;];%d;%f",
        p->entry_id, p->entry_login, p->entry_pers_nom, p->entry_pers_prenom, p->entry_pers_cin,
        &p->date_naissance.spin_pers_jour, &p->date_naissance.spin_pers_mois, &p->date_naissance.spin_pers_annee,
        p->combo_fonction, p->label_pers_grade, p->combo_base,
        &p->date_embauche.spin_pers_jour, &p->date_embauche.spin_pers_mois, &p->date_embauche.spin_pers_annee,
        p->entry_pers_telephone, p->entry_pers_mail, &p->check_disponible, &p->heures_vol) == 18;
}

static void ecrire_personnel(FILE *f, Personnel p)
{
    fprintf(f, "%s;%s;%s;%s;%s;%d;%d;%d;%s;%s;%s;%d;%d;%d;%s;%s;%d;%.1f\n",
        p.entry_id, p.entry_login, p.entry_pers_nom, p.entry_pers_prenom, p.entry_pers_cin,
        p.date_naissance.spin_pers_jour, p.date_naissance.spin_pers_mois, p.date_naissance.spin_pers_annee,
        p.combo_fonction, p.label_pers_grade, p.combo_base,
        p.date_embauche.spin_pers_jour, p.date_embauche.spin_pers_mois, p.date_embauche.spin_pers_annee,
        p.entry_pers_telephone, p.entry_pers_mail, p.check_disponible, p.heures_vol);
}


int chercher_personnel(char *fichier, char *id, Personnel *resultat)
{
    Personnel p;
    FILE *f = fopen(fichier, "r");
    if (f == NULL) return 0;
    while (lire_personnel(f, &p))
    {
        if (strcmp(p.entry_id, id) == 0)
        {
            *resultat = p;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int chercher_par_login(char *fichier, char *login, Personnel *resultat)
{
    Personnel p;
    FILE *f = fopen(fichier, "r");
    if (f == NULL) return 0;
    while (lire_personnel(f, &p))
    {
        if (strcmp(p.entry_login, login) == 0)
        {
            *resultat = p;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int id_existe(char *fichier, char *id)
{
    Personnel p;
    return chercher_personnel(fichier, id, &p);
}

int login_existe(char *fichier, char *login)
{
    Personnel p;
    return chercher_par_login(fichier, login, &p);
}

int ajouter_personnel(char *fichier, Personnel p)
{
    FILE *f;
    if (id_existe(fichier, p.entry_id) || login_existe(fichier, p.entry_login))
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

    strcpy(nouveau.entry_id, id);                  
    if (chercher_par_login(fichier, nouveau.entry_login, &autre)
        && strcmp(autre.entry_id, id) != 0)
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
        if (strcmp(p.entry_id, id) == 0)
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
        if (strcmp(p.entry_id, id) == 0)
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
        if (strcmp(fonction, "") != 0 && strcmp(p.combo_fonction, fonction) != 0) continue;
        if (strcmp(base, "") != 0 && strcmp(p.combo_base, base) != 0) continue;
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

static int date_valide(Date d)
{
    int jours[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (d.spin_pers_annee < 1900 || d.spin_pers_mois < 1 || d.spin_pers_mois > 12) return 0;
    if ((d.spin_pers_annee % 4 == 0 && d.spin_pers_annee % 100 != 0) || d.spin_pers_annee % 400 == 0)
        jours[1] = 29;
    return d.spin_pers_jour >= 1 && d.spin_pers_jour <= jours[d.spin_pers_mois - 1];
}

int controle_saisie(char *fichier, Personnel p)
{
    if (strlen(p.entry_id) == 0 || strlen(p.entry_login) == 0 || strlen(p.entry_pers_nom) == 0
        || strlen(p.entry_pers_prenom) == 0 || strlen(p.combo_fonction) == 0 || strlen(p.combo_base) == 0)
        return 1;
    if (!que_des_chiffres(p.entry_pers_telephone, 8)) return 2;
    if (!email_valide(p.entry_pers_mail)) return 3;
    if (!que_des_chiffres(p.entry_pers_cin, 8)) return 4;
    if (!date_valide(p.date_naissance) || !date_valide(p.date_embauche)) return 5;
    if (id_existe(fichier, p.entry_id)) return 6;
    if (login_existe(fichier, p.entry_login)) return 7;
    return 0;
}


int duree_experience(Date embauche, Date aujourdhui)
{
    int ans = aujourdhui.spin_pers_annee - embauche.spin_pers_annee;
    if (aujourdhui.spin_pers_mois < embauche.spin_pers_mois
        || (aujourdhui.spin_pers_mois == embauche.spin_pers_mois
            && aujourdhui.spin_pers_jour < embauche.spin_pers_jour))
        ans--;
    if (ans < 0) ans = 0;
    return ans;
}


static int comparer_dates(Date a, Date b)
{
    long x = a.spin_pers_annee * 10000L + a.spin_pers_mois * 100 + a.spin_pers_jour;
    long y = b.spin_pers_annee * 10000L + b.spin_pers_mois * 100 + b.spin_pers_jour;
    return (x < y) ? -1 : (x > y);
}


static int lire_affectation(FILE *f, Affectation *a)
{
    return fscanf(f, " %19[^;];%19[^;];%d;%d;%d;%f;%d", a->id_personnel, a->numero_vol,
                  &a->date_vol.spin_pers_jour, &a->date_vol.spin_pers_mois,
                  &a->date_vol.spin_pers_annee, &a->duree, &a->statut) == 7;
}

static void ecrire_affectation(FILE *f, Affectation a)
{
    fprintf(f, "%s;%s;%d;%d;%d;%.2f;%d\n", a.id_personnel, a.numero_vol,
            a.date_vol.spin_pers_jour, a.date_vol.spin_pers_mois, a.date_vol.spin_pers_annee,
            a.duree, a.statut);
}

int ajouter_affectation(char *fichier, Affectation a)
{
    Affectation x;
    FILE *f = fopen(fichier, "r");
    if (f != NULL)
    {
        while (lire_affectation(f, &x))
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
    ecrire_affectation(f, a);
    fclose(f);
    return 1;
}

int modifier_statut_affectation(char *fichier, char *id_personnel, char *numero_vol, int statut)
{
    Affectation a;
    int trouve = 0;
    FILE *f = fopen(fichier, "r");
    FILE *aux = fopen("aux_affectations.txt", "w");
    if (f == NULL || aux == NULL)
    {
        if (f) fclose(f);
        if (aux) fclose(aux);
        return 0;
    }
    while (lire_affectation(f, &a))
    {
        if (strcmp(a.id_personnel, id_personnel) == 0 && strcmp(a.numero_vol, numero_vol) == 0)
        {
            a.statut = statut;
            trouve = 1;
        }
        ecrire_affectation(aux, a);
    }
    fclose(f);
    fclose(aux);
    remove(fichier);
    rename("aux_affectations.txt", fichier);
    return trouve;
}

int planning_personnel(char *fichier, char *id_personnel, char *fichier_planning)
{
    Affectation a;
    int n = 0;
    FILE *f = fopen(fichier, "r");
    FILE *res = fopen(fichier_planning, "w");
    if (f == NULL || res == NULL)
    {
        if (f) fclose(f);
        if (res) fclose(res);
        return 0;
    }
    while (lire_affectation(f, &a))
        if (strcmp(a.id_personnel, id_personnel) == 0)
        {
            ecrire_affectation(res, a);
            n++;
        }
    fclose(f);
    fclose(res);
    return n;
}

void temps_de_vol(char *fichier, char *id_personnel, Date aujourdhui,
                  float *temps_vol, float *temps_restant)
{
    Affectation a;
    FILE *f = fopen(fichier, "r");
    *temps_vol = 0;
    *temps_restant = 0;
    if (f == NULL) return;
    while (lire_affectation(f, &a))
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
        if (strcmp(p.combo_fonction, "Pilote") == 0) (*pilotes)++;
        else if (strcmp(p.combo_fonction, "Copilote") == 0) (*copilotes)++;
        else (*cabine)++;
        if (p.check_disponible == 1) (*disponibles)++;
    }
    fclose(f);
}
