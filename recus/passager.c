#include <string.h>
#include "passager.h"

passager chercher(char *filename, char *cin)
{
    passager p;
    int tr = 0;
    FILE *f = fopen(filename, "r");
    if (f != NULL)
    {
        while (tr == 0 && fscanf(f, "%s %s %s %d %d %d %s %s %s %s %d %d %d\n",
               p.cin, p.nom, p.prenom, &p.jour, &p.mois, &p.annee, p.sexe,
               p.ville, p.email, p.telephone, &p.pmr, &p.newsletter, &p.points) != EOF)
        {
            if (strcmp(p.cin, cin) == 0)
                tr = 1;
        }
        fclose(f);
    }
    if (tr == 0)
        strcpy(p.cin, "");
    return p;
}

int ajouter(char *filename, passager p)
{
    passager q = chercher(filename, p.cin);
    if (strcmp(q.cin, "") != 0)
        return -1;
    FILE *f = fopen(filename, "a");
    if (f != NULL)
    {
        fprintf(f, "%s %s %s %d %d %d %s %s %s %s %d %d %d\n",
                p.cin, p.nom, p.prenom, p.jour, p.mois, p.annee, p.sexe,
                p.ville, p.email, p.telephone, p.pmr, p.newsletter, p.points);
        fclose(f);
        return 1;
    }
    return 0;
}

int modifier(char *filename, char *cin, passager nouv)
{
    int tr = 0;
    passager p;
    FILE *f = fopen(filename, "r");
    FILE *f2 = fopen("nouv.txt", "w");
    if (f != NULL && f2 != NULL)
    {
        while (fscanf(f, "%s %s %s %d %d %d %s %s %s %s %d %d %d\n",
               p.cin, p.nom, p.prenom, &p.jour, &p.mois, &p.annee, p.sexe,
               p.ville, p.email, p.telephone, &p.pmr, &p.newsletter, &p.points) != EOF)
        {
            if (strcmp(p.cin, cin) == 0)
            {
                fprintf(f2, "%s %s %s %d %d %d %s %s %s %s %d %d %d\n",
                        nouv.cin, nouv.nom, nouv.prenom, nouv.jour, nouv.mois, nouv.annee,
                        nouv.sexe, nouv.ville, nouv.email, nouv.telephone,
                        nouv.pmr, nouv.newsletter, nouv.points);
                tr = 1;
            }
            else
                fprintf(f2, "%s %s %s %d %d %d %s %s %s %s %d %d %d\n",
                        p.cin, p.nom, p.prenom, p.jour, p.mois, p.annee, p.sexe,
                        p.ville, p.email, p.telephone, p.pmr, p.newsletter, p.points);
        }
        fclose(f);
        fclose(f2);
        remove(filename);
        rename("nouv.txt", filename);
    }
    return tr;
}

int supprimer(char *filename, char *cin)
{
    int tr = 0;
    passager p;
    FILE *f = fopen(filename, "r");
    FILE *f2 = fopen("nouv.txt", "w");
    if (f != NULL && f2 != NULL)
    {
        while (fscanf(f, "%s %s %s %d %d %d %s %s %s %s %d %d %d\n",
               p.cin, p.nom, p.prenom, &p.jour, &p.mois, &p.annee, p.sexe,
               p.ville, p.email, p.telephone, &p.pmr, &p.newsletter, &p.points) != EOF)
        {
            if (strcmp(p.cin, cin) == 0)
                tr = 1;
            else
                fprintf(f2, "%s %s %s %d %d %d %s %s %s %s %d %d %d\n",
                        p.cin, p.nom, p.prenom, p.jour, p.mois, p.annee, p.sexe,
                        p.ville, p.email, p.telephone, p.pmr, p.newsletter, p.points);
        }
        fclose(f);
        fclose(f2);
        remove(filename);
        rename("nouv.txt", filename);
    }
    return tr;
}

int trier(char *filename, int ordre)
{
    passager t[200], temp;
    int n = 0, i, j;
    FILE *f = fopen(filename, "r");
    if (f == NULL)
        return 0;
    while (n < 200 && fscanf(f, "%s %s %s %d %d %d %s %s %s %s %d %d %d\n",
           t[n].cin, t[n].nom, t[n].prenom, &t[n].jour, &t[n].mois, &t[n].annee,
           t[n].sexe, t[n].ville, t[n].email, t[n].telephone,
           &t[n].pmr, &t[n].newsletter, &t[n].points) != EOF)
        n++;
    fclose(f);

    for (i = 0; i < n - 1; i++)
        for (j = 0; j < n - 1 - i; j++)
            if ((ordre == 1 && strcmp(t[j].nom, t[j + 1].nom) > 0) ||
                (ordre == 0 && strcmp(t[j].nom, t[j + 1].nom) < 0))
            {
                temp = t[j];
                t[j] = t[j + 1];
                t[j + 1] = temp;
            }

    f = fopen(filename, "w");
    if (f == NULL)
        return 0;
    for (i = 0; i < n; i++)
        fprintf(f, "%s %s %s %d %d %d %s %s %s %s %d %d %d\n",
                t[i].cin, t[i].nom, t[i].prenom, t[i].jour, t[i].mois, t[i].annee,
                t[i].sexe, t[i].ville, t[i].email, t[i].telephone,
                t[i].pmr, t[i].newsletter, t[i].points);
    fclose(f);
    return 1;
}

void niveau_fidelite(int points, char *niveau)
{
    if (points >= 1000)
        strcpy(niveau, "Platinum");
    else if (points >= 500)
        strcpy(niveau, "Gold");
    else if (points >= 100)
        strcpy(niveau, "Silver");
    else
        strcpy(niveau, "Standard");
}

int remise_fidelite(int points)
{
    if (points >= 1000)
        return 15;
    if (points >= 500)
        return 10;
    if (points >= 100)
        return 5;
    return 0;
}

int ajouter_points(char *filename, char *cin, float montant)
{
    passager p = chercher(filename, cin);
    if (strcmp(p.cin, "") == 0)
        return -1;
    p.points = p.points + (int)(montant / 10);
    modifier(filename, cin, p);
    return p.points;
}

int n_chiffres(char *s, int n)
{
    int i = 0;
    while (s[i] != '\0')
    {
        if (s[i] < '0' || s[i] > '9')
            return 0;
        i++;
    }
    return i == n;
}

int valider_cin(char *cin)
{
    return n_chiffres(cin, 8);
}

int valider_telephone(char *tel)
{
    return n_chiffres(tel, 8);
}

int valider_email(char *email)
{
    int i, arobase = -1, point = -1;
    for (i = 0; email[i] != '\0'; i++)
    {
        if (email[i] == '@' && arobase == -1)
            arobase = i;
        if (email[i] == '.' && arobase != -1)
            point = i;
    }
    if (arobase < 1 || point == -1)
        return 0;
    if (point == arobase + 1 || email[point + 1] == '\0')
        return 0;
    return 1;
}

int valider_date(int jour, int mois, int annee)
{
    int jmax;
    if (annee < 1900 || annee > 2026 || mois < 1 || mois > 12 || jour < 1)
        return 0;
    if (mois == 2)
    {
        if ((annee % 4 == 0 && annee % 100 != 0) || annee % 400 == 0)
            jmax = 29;
        else
            jmax = 28;
    }
    else if (mois == 4 || mois == 6 || mois == 9 || mois == 11)
        jmax = 30;
    else
        jmax = 31;
    return jour <= jmax;
}
