#include <stdio.h>
#include <string.h>
#include "personnel.h"

int main(void)
{
    Personnel p1, p2, p3, modif, trouve;
    AffectationPersonnel a1, a2;
    DatePersonnel aujourdhui = {10, 11, 2026};
    float temps, restant;
    int total, pil, copil, cab, dispo;

    strcpy(p1.entry_id, "P001");          strcpy(p1.entry_login, "ahmed.bs");
    strcpy(p1.entry_pers_nom, "Ben Salah"); strcpy(p1.entry_pers_prenom, "Ahmed");
    strcpy(p1.entry_pers_cin, "09876543");
    p1.date_naissance.spin_pers_jour = 12; p1.date_naissance.spin_pers_mois = 4; p1.date_naissance.spin_pers_annee = 1985;
    strcpy(p1.combo_fonction, "Pilote");  strcpy(p1.label_pers_grade, "Commandant de bord");
    strcpy(p1.combo_base, "Tunis-Carthage (TUN)");
    p1.date_embauche.spin_pers_jour = 1; p1.date_embauche.spin_pers_mois = 9; p1.date_embauche.spin_pers_annee = 2012;
    strcpy(p1.entry_pers_telephone, "22111222"); strcpy(p1.entry_pers_mail, "ahmed@carthage.tn");
    p1.check_disponible = 1; p1.heures_vol = 8500;

    p2 = p1;
    strcpy(p2.entry_id, "P002");          strcpy(p2.entry_login, "sana.t");
    strcpy(p2.entry_pers_nom, "Trabelsi"); strcpy(p2.entry_pers_prenom, "Sana");
    strcpy(p2.entry_pers_cin, "11223344"); strcpy(p2.combo_fonction, "Hotesse");
    strcpy(p2.label_pers_grade, "Chef de cabine"); strcpy(p2.combo_base, "Monastir (MIR)");
    strcpy(p2.entry_pers_telephone, "55443322"); strcpy(p2.entry_pers_mail, "sana@carthage.tn");
    p2.heures_vol = 3200;

    p3 = p1;
    strcpy(p3.entry_id, "P003");          strcpy(p3.entry_login, "karim.m");
    strcpy(p3.entry_pers_nom, "Mansour"); strcpy(p3.entry_pers_prenom, "Karim");
    strcpy(p3.entry_pers_cin, "55667788"); strcpy(p3.combo_fonction, "Copilote");
    strcpy(p3.label_pers_grade, "Officier pilote de ligne");
    strcpy(p3.entry_pers_telephone, "98765432"); strcpy(p3.entry_pers_mail, "karim@carthage.tn");
    p3.check_disponible = 0; p3.heures_vol = 1500;

    remove("personnel.txt");
    remove("affectations.txt");

    printf("Controle de saisie P001 : %d (0 = correct)\n", controle_saisie_personnel("personnel.txt", p1));
    printf("Ajout P001 : %d\n", ajouter_personnel("personnel.txt", p1));
    printf("Ajout P002 : %d\n", ajouter_personnel("personnel.txt", p2));
    printf("Ajout P003 : %d\n", ajouter_personnel("personnel.txt", p3));
    printf("Ajout P001 une 2e fois (doublon refuse) : %d\n", ajouter_personnel("personnel.txt", p1));

    modif = p2;
    strcpy(modif.entry_pers_telephone, "20202020");
    printf("Modification P002 : %d\n", modifier_personnel("personnel.txt", "P002", modif));

    printf("Recherche P003 : %d\n", chercher_personnel("personnel.txt", "P003", &trouve));
    printf("Filtre Pilote + disponibles : %d membre(s) -> recherche.txt\n",
           filtrer_personnel("personnel.txt", "Pilote", "", 1, "recherche.txt"));

    printf("Experience de P001 : %d ans\n", duree_experience_personnel(p1.date_embauche, aujourdhui));

    strcpy(a1.id_personnel, "P001"); strcpy(a1.numero_vol, "TU712");
    a1.date_vol.spin_pers_jour = 2; a1.date_vol.spin_pers_mois = 11; a1.date_vol.spin_pers_annee = 2026;
    a1.duree = 2.5; a1.statut = 1;
    a2 = a1;
    strcpy(a2.numero_vol, "TU214");
    a2.date_vol.spin_pers_jour = 20; a2.duree = 3; a2.statut = 0;
    printf("Affectation P001 -> TU712 : %d\n", ajouter_affectation_personnel("affectations.txt", a1));
    printf("Affectation P001 -> TU214 : %d\n", ajouter_affectation_personnel("affectations.txt", a2));
    printf("Confirmation TU214 : %d\n", modifier_statut_affectation_personnel("affectations.txt", "P001", "TU214", 1));
    printf("Planning de P001 : %d vol(s) -> planning.txt\n",
           planning_personnel("affectations.txt", "P001", "planning.txt"));
    temps_de_vol_personnel("affectations.txt", "P001", aujourdhui, &temps, &restant);
    printf("Temps de vol : %.1f h, restant : %.1f h\n", temps, restant);

    statistiques_personnel("personnel.txt", &total, &pil, &copil, &cab, &dispo);
    printf("Total %d, pilotes %d, copilotes %d, cabine %d, disponibles %d\n", total, pil, copil, cab, dispo);

    printf("Suppression P003 : %d\n", supprimer_personnel("personnel.txt", "P003"));
    return 0;
}
