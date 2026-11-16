#include <stdio.h>
#include <string.h>
#include "passager.h"

int main()
{
    passager p1 = {"11111111", "Zouari", "Sami", {5, 3, 2000}, "Masculin", "Tunis", "sami@mail.com", "20123456", 0, 1, 0};
    passager p2 = {"22222222", "Ben_Ali", "Amel", {12, 7, 1999}, "Feminin", "Sfax", "amel@mail.com", "98765432", 1, 0, 0};
    passager p3;
    char niveau[10];
    int x;

    remove("passagers.txt");

    x = ajouter("passagers.txt", p1);
    if (x == 1)
        printf("\najout de p1 avec succes");
    else
        printf("\nechec ajout de p1");

    x = ajouter("passagers.txt", p2);
    if (x == 1)
        printf("\najout de p2 avec succes");
    else
        printf("\nechec ajout de p2");

    x = ajouter("passagers.txt", p1);
    if (x == -1)
        printf("\nCIN deja existant (unicite respectee)");

    p2.points = 120;
    x = modifier("passagers.txt", "22222222", p2);
    if (x == 1)
        printf("\nmodification avec succes");
    else
        printf("\nechec modification");

    p3 = chercher("passagers.txt", "22222222");
    if (strcmp(p3.cin, "") != 0)
        printf("\ntrouve : %s %s", p3.nom, p3.prenom);
    else
        printf("\nintrouvable");

    x = trier("passagers.txt", 1);
    if (x == 1)
        printf("\ntri croissant fait (voir passagers.txt)");

    x = ajouter_points("passagers.txt", "11111111", 5200);
    niveau_fidelite(x, niveau);
    printf("\npoints : %d, niveau : %s, remise : %d%%", x, niveau, remise_fidelite(x));

    x = supprimer("passagers.txt", "11111111");
    if (x == 1)
        printf("\nsuppression avec succes");
    else
        printf("\nechec suppression");

    if (valider_cin("12345678") == 1)
        printf("\nCIN valide");
    if (valider_email("ab.com") == 0)
        printf("\nemail invalide detecte");

    x = rechercher("passagers.txt", "Sfax", "resultat.txt");
    if (x >= 0)
        printf("\nrecherche ville Sfax : %d trouve(s)", x);
    if (valider_date(p1.date_naissance) == 1)
        printf("\ndate valide");

    printf("\n");
    return 0;
}
