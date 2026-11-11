#include <stdio.h>
#include <string.h>
#include "personnel.h"

int main(void)
{
    char fichier[] = "personnel.txt";
    char fichier_aff[] = "affectations.txt";
    Personnel p, trouve;
    AffectationPersonnel a;
    DatePersonnel aujourdhui;
    char id[20], fonction[20], base[40];
    int choix, dispo, code, n, statut;
    int total, pil, copil, cab, nb_dispo;
    float temps, restant;

    printf("Date du jour (jj mm aaaa) : ");
    scanf("%d %d %d", &aujourdhui.spin_pers_jour, &aujourdhui.spin_pers_mois, &aujourdhui.spin_pers_annee);

    do
    {
        printf("\n===== GESTION DU PERSONNEL NAVIGANT =====\n");
        printf("1. Ajouter un membre\n");
        printf("2. Modifier un membre\n");
        printf("3. Supprimer un membre\n");
        printf("4. Rechercher un membre\n");
        printf("5. Filtrer les membres\n");
        printf("6. Affecter un membre a un vol\n");
        printf("7. Changer le statut d'une affectation\n");
        printf("8. Planning et temps de vol d'un membre\n");
        printf("9. Statistiques\n");
        printf("0. Quitter\n");
        printf("Votre choix : ");
        if (scanf("%d", &choix) != 1) break;

        switch (choix)
        {
        case 1:
        case 2:
            if (choix == 2)
            {
                printf("ID du membre a modifier : ");
                scanf(" %19s", id);
                if (!chercher_personnel(fichier, id, &trouve))
                {
                    printf("Membre introuvable.\n");
                    break;
                }
                strcpy(p.entry_id, id);
            }
            else
            {
                printf("ID : ");
                scanf(" %19s", p.entry_id);
            }
            printf("Login : ");
            scanf(" %29s", p.entry_login);
            printf("Nom : ");
            scanf(" %29[^\n]", p.entry_pers_nom);
            printf("Prenom : ");
            scanf(" %29[^\n]", p.entry_pers_prenom);
            printf("CIN (8 chiffres) : ");
            scanf(" %8s", p.entry_pers_cin);
            printf("Date de naissance (jj mm aaaa) : ");
            scanf("%d %d %d", &p.date_naissance.spin_pers_jour, &p.date_naissance.spin_pers_mois, &p.date_naissance.spin_pers_annee);
            printf("Fonction (Pilote, Copilote, Hotesse, Steward) : ");
            scanf(" %19s", p.combo_fonction);
            printf("Grade : ");
            scanf(" %39[^\n]", p.label_pers_grade);
            printf("Base d'attache : ");
            scanf(" %39[^\n]", p.combo_base);
            printf("Date d'embauche (jj mm aaaa) : ");
            scanf("%d %d %d", &p.date_embauche.spin_pers_jour, &p.date_embauche.spin_pers_mois, &p.date_embauche.spin_pers_annee);
            printf("Telephone (8 chiffres) : ");
            scanf(" %8s", p.entry_pers_telephone);
            printf("Email : ");
            scanf(" %49s", p.entry_pers_mail);
            printf("Disponible (1 = oui, 0 = non) : ");
            scanf("%d", &p.check_disponible);
            printf("Heures de vol : ");
            scanf("%f", &p.heures_vol);

            code = controle_saisie_personnel(fichier, p);
            if (choix == 2 && (code == 6 || code == 7))
                code = 0;
            switch (code)
            {
            case 1: printf("Erreur : un champ obligatoire est vide.\n"); break;
            case 2: printf("Erreur : le telephone doit contenir 8 chiffres.\n"); break;
            case 3: printf("Erreur : email invalide.\n"); break;
            case 4: printf("Erreur : le CIN doit contenir 8 chiffres.\n"); break;
            case 5: printf("Erreur : date invalide.\n"); break;
            case 6: printf("Erreur : cet ID existe deja.\n"); break;
            case 7: printf("Erreur : ce login existe deja.\n"); break;
            }
            if (code != 0) break;

            if (choix == 1)
            {
                if (ajouter_personnel(fichier, p)) printf("Membre ajoute.\n");
                else printf("Ajout impossible.\n");
            }
            else
            {
                if (modifier_personnel(fichier, id, p)) printf("Membre modifie.\n");
                else printf("Modification impossible (login deja utilise ?).\n");
            }
            break;

        case 3:
            printf("ID du membre a supprimer : ");
            scanf(" %19s", id);
            if (supprimer_personnel(fichier, id)) printf("Membre supprime.\n");
            else printf("Membre introuvable.\n");
            break;

        case 4:
            printf("ID du membre a rechercher : ");
            scanf(" %19s", id);
            if (chercher_personnel(fichier, id, &trouve))
            {
                printf("Trouve : %s %s, %s, base %s, experience %d an(s)\n",
                       trouve.entry_pers_prenom, trouve.entry_pers_nom, trouve.combo_fonction,
                       trouve.combo_base, duree_experience_personnel(trouve.date_embauche, aujourdhui));
            }
            else
                printf("Membre introuvable.\n");
            break;

        case 5:
            printf("Fonction (ou - pour toutes) : ");
            scanf(" %19s", fonction);
            if (strcmp(fonction, "-") == 0) strcpy(fonction, "");
            printf("Base (ou - pour toutes) : ");
            scanf(" %39[^\n]", base);
            if (strcmp(base, "-") == 0) strcpy(base, "");
            printf("Disponibilite (1 = disponibles, 0 = indisponibles, -1 = tous) : ");
            scanf("%d", &dispo);
            n = filtrer_personnel(fichier, fonction, base, dispo, "recherche.txt");
            printf("%d membre(s) trouve(s), resultat dans recherche.txt\n", n);
            break;

        case 6:
            printf("ID du membre : ");
            scanf(" %19s", a.id_personnel);
            if (!chercher_personnel(fichier, a.id_personnel, &trouve))
            {
                printf("Membre introuvable.\n");
                break;
            }
            printf("Numero du vol : ");
            scanf(" %19s", a.numero_vol);
            printf("Date du vol (jj mm aaaa) : ");
            scanf("%d %d %d", &a.date_vol.spin_pers_jour, &a.date_vol.spin_pers_mois, &a.date_vol.spin_pers_annee);
            printf("Duree du vol (heures) : ");
            scanf("%f", &a.duree);
            a.statut = 0;
            if (ajouter_affectation_personnel(fichier_aff, a)) printf("Affectation enregistree (non confirmee).\n");
            else printf("Ce membre est deja affecte a ce vol.\n");
            break;

        case 7:
            printf("ID du membre : ");
            scanf(" %19s", id);
            printf("Numero du vol : ");
            scanf(" %19s", a.numero_vol);
            printf("Nouveau statut (0 non confirme, 1 confirme, 2 retarde, 3 annule) : ");
            scanf("%d", &statut);
            if (modifier_statut_affectation_personnel(fichier_aff, id, a.numero_vol, statut))
                printf("Statut modifie.\n");
            else
                printf("Affectation introuvable.\n");
            break;

        case 8:
            printf("ID du membre : ");
            scanf(" %19s", id);
            n = planning_personnel(fichier_aff, id, "planning.txt");
            temps_de_vol_personnel(fichier_aff, id, aujourdhui, &temps, &restant);
            printf("%d vol(s) dans planning.txt\n", n);
            printf("Temps de vol : %.1f h, temps de vol restant : %.1f h\n", temps, restant);
            break;

        case 9:
            statistiques_personnel(fichier, &total, &pil, &copil, &cab, &nb_dispo);
            printf("Total : %d | Pilotes : %d | Copilotes : %d | Cabine : %d | Disponibles : %d\n",
                   total, pil, copil, cab, nb_dispo);
            break;

        case 0:
            printf("Au revoir.\n");
            break;

        default:
            printf("Choix invalide.\n");
        }
    } while (choix != 0);

    return 0;
}
