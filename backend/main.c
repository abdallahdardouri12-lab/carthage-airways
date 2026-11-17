#include <stdio.h>
#include <string.h>
#include "reservation.h"

static void vider_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void saisir_reservation(Reservation *r)
{
    int choix;

    printf("ID passager : ");  
    scanf("%19s", r->id_passager);
    printf("Numero de vol : ");
    scanf("%19s", r->id_vol);

    if (charger_infos_vol(FICHIER_VOLS, r))
    {
        printf("  -> Vol trouve : %02d/%02d/%04d  %02d:%02d - %02d:%02d  prix de base %.2f DT\n",
               r->date_vol.jour, r->date_vol.mois, r->date_vol.annee,
               r->heure_depart.heure, r->heure_depart.minute,
               r->heure_arrivee.heure, r->heure_arrivee.minute, r->prix_base);
    }
    else
    {
        printf("  -> Vol absent dans %s, saisie manuelle.\n", FICHIER_VOLS);
        printf("Date du vol (jj mm aaaa) : ");
        scanf("%d %d %d", &r->date_vol.jour, &r->date_vol.mois, &r->date_vol.annee);
        printf("Heure de depart (hh mm) : ");
        scanf("%d %d", &r->heure_depart.heure, &r->heure_depart.minute);
        printf("Heure d'arrivee (hh mm) : ");
        scanf("%d %d", &r->heure_arrivee.heure, &r->heure_arrivee.minute);
        printf("Prix de base (DT) : ");
        scanf("%f", &r->prix_base);
    }

    printf("Classe (1 Economique, 2 Affaires, 3 Premiere) : ");
    scanf("%d", &choix);
    strcpy(r->classe, choix == 2 ? "Affaires" : choix == 3 ? "Premiere" : "Economique");

    printf("Siege (ex 12A) : ");                     scanf("%5s", r->siege);
    printf("Repas particulier (1 oui / 0 non) : ");  scanf("%d", &r->repas);
    printf("Assistance PMR (1 oui / 0 non) : ");     scanf("%d", &r->pmr);
    printf("Bagages supplementaires (0-3) : ");      scanf("%d", &r->nb_bagages);
    vider_buffer();
}

static void afficher_une(Reservation r)
{
    printf("\n  Reservation %s | passager %s | vol %s\n", r.id, r.id_passager, r.id_vol);
    printf("  Le %02d/%02d/%04d de %02d:%02d a %02d:%02d | %s | siege %s\n",
           r.date_vol.jour, r.date_vol.mois, r.date_vol.annee,
           r.heure_depart.heure, r.heure_depart.minute,
           r.heure_arrivee.heure, r.heure_arrivee.minute, r.classe, r.siege);
    printf("  Repas : %s | PMR : %s | Bagages supp. : %d\n",
           r.repas ? "oui" : "non", r.pmr ? "oui" : "non", r.nb_bagages);
    printf("  Prix total : %.2f DT | Statut : %s\n", r.prix_total, r.statut);
}

static void message_ajout(int res, char *quoi)
{
    switch (res)
    {
        case 1:  printf(">> %s avec succes.\n", quoi); break;
        case -1: printf(">> Erreur : identifiant deja existant.\n"); break;
        case -2: printf(">> Erreur : ce passager a deja une reservation sur ce vol.\n"); break;
        case -3: printf(">> Erreur : siege deja pris sur ce vol.\n"); break;
        default: printf(">> Erreur : reservation introuvable ou fichier inaccessible.\n");
    }
}

int main(void)
{
    int choix, res, n, statut_choix;
    Reservation r;
    char id[20], id_passager[20], id_vol[20], classe[20], statut[20], siege[6];
    char message[200];
    float montant;

    do
    {
        printf("\n========== CARTHAGE AIRWAYS - RESERVATIONS ==========\n");
        printf(" --- Administrateur ---\n");
        printf("  1. Ajouter une reservation\n");
        printf("  2. Afficher toutes les reservations\n");
        printf("  3. Modifier une reservation\n");
        printf("  4. Supprimer une reservation\n");
        printf("  5. Rechercher une reservation (par ID)\n");
        printf("  6. Filtrer les reservations (multi-criteres)\n");
        printf(" --- Passager ---\n");
        printf("  7. Reserver un vol (prix + detection de conflit)\n");
        printf("  8. Mes reservations\n");
        printf("  9. Modifier mon siege\n");
        printf(" 10. Annuler une reservation (remboursement)\n");
        printf("  0. Quitter\n");
        printf("Votre choix : ");
        if (scanf("%d", &choix) != 1) { vider_buffer(); choix = -1; continue; }
        vider_buffer();

        switch (choix)
        {
        case 1:
            memset(&r, 0, sizeof(r));
            generer_id(FICHIER_RESERVATIONS, r.id);
            printf("Identifiant genere : %s\n", r.id);
            saisir_reservation(&r);
            strcpy(r.statut, "Confirmee");
            if (!controle_saisie(r, message))
                printf(">> Saisie refusee : %s\n", message);
            else
            {
                res = ajouter_reservation(FICHIER_RESERVATIONS, r);
                message_ajout(res, "Reservation ajoutee");
                if (res == 1)
                    printf("   Prix total calcule : %.2f DT\n",
                           calculer_prix(r.prix_base, r.classe, r.repas, r.pmr, r.nb_bagages));
            }
            break;

        case 2:
            afficher_reservations(FICHIER_RESERVATIONS);
            break;

        case 3:
            printf("ID de la reservation a modifier : ");
            scanf("%19s", id);
            vider_buffer();
            if (!chercher_reservation(FICHIER_RESERVATIONS, id, &r))
            {
                printf(">> Reservation introuvable.\n");
                break;
            }
            afficher_une(r);
            printf("\nNouvelles valeurs :\n");
            saisir_reservation(&r);
            printf("Statut (1 Confirmee, 2 En_attente, 3 Annulee) : ");
            scanf("%d", &statut_choix);
            vider_buffer();
            strcpy(r.statut, statut_choix == 2 ? "En_attente" :
                             statut_choix == 3 ? "Annulee" : "Confirmee");
            if (!controle_saisie(r, message))
                printf(">> Saisie refusee : %s\n", message);
            else
                message_ajout(modifier_reservation(FICHIER_RESERVATIONS, id, r),
                              "Reservation modifiee");
            break;

        case 4:
            printf("ID de la reservation a supprimer : ");
            scanf("%19s", id);
            vider_buffer();
            if (supprimer_reservation(FICHIER_RESERVATIONS, id))
                printf(">> Reservation supprimee.\n");
            else
                printf(">> Reservation introuvable.\n");
            break;

        case 5:
            printf("ID de la reservation : ");
            scanf("%19s", id);
            vider_buffer();
            if (chercher_reservation(FICHIER_RESERVATIONS, id, &r))
                afficher_une(r);
            else
                printf(">> Reservation introuvable.\n");
            break;

        case 6:
            printf("(tapez - pour ignorer un critere)\n");
            printf("ID passager : ");  scanf("%19s", id_passager);
            printf("Numero de vol : "); scanf("%19s", id_vol);
            printf("Classe (Economique/Affaires/Premiere) : "); scanf("%19s", classe);
            printf("Statut (Confirmee/En_attente/Annulee) : "); scanf("%19s", statut);
            vider_buffer();
            if (strcmp(id_passager, "-") == 0) strcpy(id_passager, "");
            if (strcmp(id_vol, "-") == 0)      strcpy(id_vol, "");
            if (strcmp(classe, "-") == 0)      strcpy(classe, "");
            if (strcmp(statut, "-") == 0)      strcpy(statut, "");
            n = filtrer_reservations(FICHIER_RESERVATIONS, FICHIER_RESULTAT,
                                     id_passager, id_vol, classe, statut);
            printf(">> %d resultat(s) :\n", n);
            if (n > 0) afficher_reservations(FICHIER_RESULTAT);
            break;

        case 7:
            memset(&r, 0, sizeof(r));
            printf("Votre identifiant (passager connecte) : "); scanf("%19s", r.id_passager);
            printf("Numero du vol choisi : ");                 scanf("%19s", r.id_vol);
            printf("Classe (1 Economique, 2 Affaires, 3 Premiere) : ");
            scanf("%d", &statut_choix);
            strcpy(r.classe, statut_choix == 2 ? "Affaires" :
                             statut_choix == 3 ? "Premiere" : "Economique");
            printf("Siege (ex 12A) : ");                     scanf("%5s", r.siege);
            printf("Repas particulier (1/0) : ");            scanf("%d", &r.repas);
            printf("Assistance PMR (1/0) : ");               scanf("%d", &r.pmr);
            printf("Bagages supplementaires (0-3) : ");      scanf("%d", &r.nb_bagages);
            vider_buffer();
            if (reserver_vol(FICHIER_RESERVATIONS, FICHIER_VOLS, &r, message))
                printf(">> %s\n", message);
            else
                printf(">> Reservation refusee : %s\n", message);
            break;

        case 8:
            printf("Votre identifiant : ");
            scanf("%19s", id_passager);
            vider_buffer();
            n = mes_reservations(FICHIER_RESERVATIONS, FICHIER_RESULTAT, id_passager);
            if (n == 0) printf(">> Aucune reservation.\n");
            else        afficher_reservations(FICHIER_RESULTAT);
            break;

        case 9:
            printf("Votre identifiant : ");          scanf("%19s", id_passager);
            printf("ID de la reservation : ");       scanf("%19s", id);
            printf("Nouveau siege (ex 14C) : ");     scanf("%5s", siege);
            vider_buffer();
            res = modifier_siege(FICHIER_RESERVATIONS, id, id_passager, siege);
            if (res == 1)       printf(">> Siege modifie.\n");
            else if (res == -1) printf(">> Cette reservation ne vous appartient pas.\n");
            else if (res == -2) printf(">> Siege invalide (ex : 12A).\n");
            else if (res == -3) printf(">> Ce siege est deja pris.\n");
            else if (res == -4) printf(">> Reservation annulee : modification impossible.\n");
            else                printf(">> Reservation introuvable.\n");
            break;

        case 10:
            printf("Votre identifiant : ");     scanf("%19s", id_passager);
            printf("ID de la reservation : ");  scanf("%19s", id);
            vider_buffer();
            montant = annuler_reservation(FICHIER_RESERVATIONS, id, id_passager);
            if (montant == -1)      printf(">> Reservation introuvable ou ne vous appartient pas.\n");
            else if (montant == -2) printf(">> Reservation deja annulee.\n");
            else printf(">> Reservation annulee. Montant rembourse : %.2f DT\n", montant);
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
