#include <stdio.h>
#include <string.h>
#include "utilisateur.h"

#define VIDER_CLAVIER while (getchar() != '\n')

int main(void)
{
    char login[30], mdp[30], role[20];
    int choix;

    do
    {
        printf("\n===== CARTHAGE AIRWAYS - CONNEXION =====\n");
        printf("1. Se connecter\n");
        printf("0. Quitter\n");
        printf("Votre choix : ");
        if (scanf("%d", &choix) != 1)
            choix = -1;
        VIDER_CLAVIER;

        if (choix == 1)
        {
            printf("Login : ");
            scanf(" %29s", login);
            printf("Mot de passe : ");
            scanf(" %29s", mdp);
            VIDER_CLAVIER;

            if (verifier_connexion("utilisateurs.txt", login, mdp, role))
                printf("Connexion reussie.\n");
            else
                printf("Login ou mot de passe incorrect.\n");
        }
        else if (choix == 0)
            printf("Au revoir.\n");
        else
            printf("Choix invalide.\n");
    } while (choix != 0);

    return 0;
}
