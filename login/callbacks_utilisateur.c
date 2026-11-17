#include <string.h>
#include <gtk/gtk.h>
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "utilisateur.h"

char utilisateur_connecte[30] = "";

void
on_btn_login_connexion_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *login_window = lookup_widget(GTK_WIDGET(button), "login_window");
    GtkWidget *message = lookup_widget(GTK_WIDGET(button), "label_login_message");
    GtkWidget *fenetre = NULL;
    char login[30], mdp[30], role[20];

    strncpy(login, gtk_entry_get_text(GTK_ENTRY(lookup_widget(GTK_WIDGET(button), "entry_login_user"))), 29);
    login[29] = '\0';
    strncpy(mdp, gtk_entry_get_text(GTK_ENTRY(lookup_widget(GTK_WIDGET(button), "entry_login_mdp"))), 29);
    mdp[29] = '\0';

    if (strlen(login) == 0 || strlen(mdp) == 0)
    {
        gtk_label_set_markup(GTK_LABEL(message), "<span foreground=\"red\">Saisissez votre login et votre mot de passe.</span>");
        return;
    }
    if (!verifier_connexion("utilisateurs.txt", login, mdp, role))
    {
        gtk_label_set_markup(GTK_LABEL(message), "<span foreground=\"red\">Login ou mot de passe incorrect.</span>");
        return;
    }

    strcpy(utilisateur_connecte, login);
    if (strcmp(role, "admin") == 0)
        fenetre = create_window_admin();
    else if (strcmp(role, "personnel") == 0)
        fenetre = create_win_pers_espace();
    else if (strcmp(role, "passager") == 0)
        fenetre = create_window_espace_passager();

    if (fenetre == NULL)
    {
        gtk_label_set_markup(GTK_LABEL(message), "<span foreground=\"red\">Rôle inconnu dans utilisateurs.txt.</span>");
        return;
    }
    gtk_widget_show(fenetre);
    gtk_widget_hide(login_window);
}


void
on_btn_login_quitter_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_main_quit();
}
