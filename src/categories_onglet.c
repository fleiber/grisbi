/* fichier qui s'occupe de l'onglet de gestion des catégories */
/*           categories_onglet.c */

/*     Copyright (C) 2000-2003  Cédric Auger */
/* 			cedric@grisbi.org */
/* 			http://www.grisbi.org */

/*     This program is free software; you can redistribute it and/or modify */
/*     it under the terms of the GNU General Public License as published by */
/*     the Free Software Foundation; either version 2 of the License, or */
/*     (at your option) any later version. */

/*     This program is distributed in the hope that it will be useful, */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/*     GNU General Public License for more details. */

/*     You should have received a copy of the GNU General Public License */
/*     along with this program; if not, write to the Free Software */
/*     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include "include.h"
#include "echeancier_formulaire_constants.h"
#include "ventilation_constants.h"
#include "echeancier_ventilation_constants.h"
#include "categories_onglet_constants.h"
#include "operations_formulaire_constants.h"



/*START_INCLUDE*/
#include "categories_onglet.h"
#include "utils_devises.h"
#include "utils_categories.h"
#include "operations_comptes.h"
#include "tiers_onglet.h"
#include "fichiers_io.h"
#include "barre_outils.h"
#include "operations_liste.h"
#include "dialog.h"
#include "gtk_combofix.h"
#include "utils_str.h"
#include "traitement_variables.h"
#include "utils_operations.h"
#include "etats_config.h"
#include "affichage_formulaire.h"
#include "operations_formulaire.h"
#include "utils_file_selection.h"
#include "utils_files.h"
/*END_INCLUDE*/

/*START_STATIC*/
static void appui_sur_ajout_categorie ( void );
static void appui_sur_ajout_sous_categorie ( void );
static void calcule_total_montant_categ ( void );
static gchar *calcule_total_montant_categ_par_compte ( gint categ, gint sous_categ, gint no_compte );
static void clique_sur_annuler_categ ( void );
static void clique_sur_modifier_categ ( void );
static gboolean enleve_selection_ligne_categ ( void );
static void expand_selected_category ();
static void exporter_categ ( void );
static void importer_categ ( void );
static gboolean keypress_category ( GtkWidget *widget, GdkEventKey *ev, gint *no_origine );
static void merge_liste_categories ( void );
static void modification_du_texte_categ ( void );
static gboolean ouverture_node_categ ( GtkWidget *arbre, GtkCTreeNode *node, 
				gpointer null );
static gboolean selection_ligne_categ ( GtkCTree *arbre_categ, GtkCTreeNode *noeud,
				 gint colonne, gpointer null );
static void supprimer_categ ( void );
static void supprimer_sous_categ ( void );
static gboolean verifie_double_click_categ ( GtkWidget *liste, GdkEventButton *ev,
				      gpointer null );
/*END_STATIC*/





gint mise_a_jour_combofix_categ_necessaire;

GtkWidget *arbre_categ;
GtkWidget *entree_nom_categ;
GtkWidget *bouton_categ_debit;
GtkWidget *bouton_categ_credit;
GtkWidget *bouton_modif_categ_modifier;
GtkWidget *bouton_modif_categ_annuler;
GtkWidget *bouton_supprimer_categ;
GtkWidget *bouton_ajouter_categorie;
GtkWidget *bouton_ajouter_sous_categorie;

/* liste des structures de catég */

GSList *liste_struct_categories;

/*  liste des noms des categ et sous categ pour le combofix */

GSList *liste_categories_combofix;

/* nombre de catégories */

gint nb_enregistrements_categories;
gint no_derniere_categorie;

/* buffer */

gfloat *tab_montant_categ;

/* buffer */

gfloat **tab_montant_sous_categ;

/* buffer */

gint *nb_ecritures_par_categ;

/* buffer */

gint **nb_ecritures_par_sous_categ;




/*START_EXTERN*/
extern gint compte_courant;
extern gchar *dernier_chemin_de_travail;
extern struct struct_devise *devise_compte;
extern struct struct_devise *devise_operation;
extern struct struct_etat *etat_courant;
extern GtkWidget *formulaire;
extern GSList *liste_categories_ventilation_combofix;
extern GSList *liste_struct_echeances;
extern GdkBitmap *masque_ferme;
extern GdkBitmap *masque_ouvre;
extern gint modif_categ;
extern gint nb_comptes;
extern gint nb_ecritures_par_comptes;
extern gint no_derniere_operation;
extern gint no_devise_totaux_tiers;
extern gpointer **p_tab_nom_de_compte;
extern gpointer **p_tab_nom_de_compte_variable;
extern GdkPixmap *pixmap_ferme;
extern GdkPixmap *pixmap_ouvre;
extern GtkTreeSelection * selection;
extern GtkWidget *widget_formulaire_echeancier[SCHEDULER_FORM_TOTAL_WIDGET];
extern GtkWidget *widget_formulaire_ventilation[TRANSACTION_BREAKDOWN_FORM_TOTAL_WIDGET];
extern GtkWidget *widget_formulaire_ventilation_echeances[SCHEDULER_BREAKDOWN_FORM_TOTAL_WIDGET];
extern GtkWidget *window;
/*END_EXTERN*/




/* **************************************************************************************************** */
/* Fonction onglet_categories : */
/* crée et renvoie le widget contenu dans l'onglet */
/* **************************************************************************************************** */

GtkWidget *onglet_categories ( void )
{
    GtkWidget *onglet;
    GtkWidget *scroll_window;
    gchar *titres[] =
    {
	_("Categories list"),
	_("Amount per category"),
	_("Amount per subcategory"),
	_("Amount per account")
    };
    GtkWidget *vbox;
    GtkWidget *frame;
    GtkWidget *vbox_frame;
    GtkWidget *hbox;
    GtkWidget *label;
    GtkWidget *separateur;
    GtkWidget *bouton;


    /* création de la fenêtre qui sera renvoyée */

    onglet = gtk_hbox_new ( FALSE,
			    5 );
    gtk_container_set_border_width ( GTK_CONTAINER ( onglet ),
				     10 );
    gtk_widget_show ( onglet );


    /*   création de la frame de gauche */

    frame = gtk_frame_new ( NULL );
    gtk_frame_set_shadow_type ( GTK_FRAME ( frame ),
				GTK_SHADOW_IN );
    gtk_box_pack_start ( GTK_BOX ( onglet ),
			 frame,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show (frame );

    /* mise en place du gtk_text */

    vbox = gtk_vbox_new ( FALSE,
			  5 );
    gtk_container_set_border_width ( GTK_CONTAINER ( vbox ),
				     15 );
    gtk_container_add ( GTK_CONTAINER ( frame ),
			vbox );
    gtk_widget_show ( vbox );




    frame = gtk_frame_new ( SPACIFY(COLON(_("Information"))) );
    gtk_box_pack_start ( GTK_BOX ( vbox ),
			 frame,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( frame );

    vbox_frame = gtk_vbox_new ( FALSE,
				5 );
    gtk_container_set_border_width ( GTK_CONTAINER ( vbox_frame ),
				     5 );
    gtk_container_add ( GTK_CONTAINER ( frame ),
			vbox_frame );
    gtk_widget_show ( vbox_frame );

    entree_nom_categ = gtk_entry_new ();
    gtk_widget_set_sensitive ( entree_nom_categ,
			       FALSE );
    gtk_signal_connect ( GTK_OBJECT ( entree_nom_categ ),
			 "changed",
			 GTK_SIGNAL_FUNC ( modification_du_texte_categ),
			 NULL );
    gtk_box_pack_start ( GTK_BOX ( vbox_frame ),
			 entree_nom_categ,
			 FALSE,
			 FALSE,
			 10 );
    gtk_widget_show ( entree_nom_categ );


    /* création des radio bouton débit/crédit */

    separateur = gtk_hseparator_new ();
    gtk_box_pack_start ( GTK_BOX ( vbox_frame ),
			 separateur,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( separateur );


    label = gtk_label_new ( COLON(_("Sorting")) );
    gtk_box_pack_start ( GTK_BOX ( vbox_frame ),
			 label,
			 FALSE,
			 FALSE,
			 10 );
    gtk_widget_show ( label );

    bouton_categ_debit = gtk_radio_button_new_with_label ( NULL,
							   _("Debit") );
    gtk_widget_set_sensitive ( bouton_categ_debit,
			       FALSE );
    gtk_signal_connect ( GTK_OBJECT ( bouton_categ_debit ),
			 "toggled",
			 GTK_SIGNAL_FUNC ( modification_du_texte_categ),
			 NULL );
    gtk_box_pack_start ( GTK_BOX ( vbox_frame ),
			 bouton_categ_debit,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( bouton_categ_debit );


    bouton_categ_credit = gtk_radio_button_new_with_label_from_widget ( GTK_RADIO_BUTTON ( bouton_categ_debit ),
									_("Credit") );
    gtk_widget_set_sensitive ( bouton_categ_credit,
			       FALSE );
    gtk_box_pack_start ( GTK_BOX ( vbox_frame ),
			 bouton_categ_credit,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( bouton_categ_credit );


    separateur = gtk_hseparator_new ();
    gtk_box_pack_start ( GTK_BOX ( vbox_frame ),
			 separateur,
			 FALSE,
			 FALSE,
			 10 );
    gtk_widget_show ( separateur );




    /*   création des boutons modifier et annuler */

    hbox = gtk_hbox_new ( TRUE,
			  5 );
    gtk_box_pack_start ( GTK_BOX ( vbox_frame ),
			 hbox,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( hbox );

    bouton_modif_categ_modifier = gtk_button_new_from_stock (GTK_STOCK_APPLY);
    gtk_button_set_relief ( GTK_BUTTON ( bouton_modif_categ_modifier ),
			    GTK_RELIEF_NONE );
    gtk_widget_set_sensitive ( bouton_modif_categ_modifier,
			       FALSE );
    gtk_signal_connect ( GTK_OBJECT ( bouton_modif_categ_modifier ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( clique_sur_modifier_categ ),
			 NULL );
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton_modif_categ_modifier,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( bouton_modif_categ_modifier );

    bouton_modif_categ_annuler = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
    gtk_button_set_relief ( GTK_BUTTON ( bouton_modif_categ_annuler ),
			    GTK_RELIEF_NONE );
    gtk_signal_connect ( GTK_OBJECT ( bouton_modif_categ_annuler ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( clique_sur_annuler_categ ),
			 NULL );
    gtk_widget_set_sensitive ( bouton_modif_categ_annuler,
			       FALSE );
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton_modif_categ_annuler,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( bouton_modif_categ_annuler);

    bouton_supprimer_categ = gtk_button_new_from_stock (GTK_STOCK_REMOVE);
    gtk_button_set_relief ( GTK_BUTTON ( bouton_supprimer_categ ),
			    GTK_RELIEF_NONE );
    gtk_widget_set_sensitive ( bouton_supprimer_categ,
			       FALSE );
    gtk_signal_connect ( GTK_OBJECT ( bouton_supprimer_categ ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( supprimer_categ ),
			 NULL );
    gtk_box_pack_start ( GTK_BOX ( vbox_frame ),
			 bouton_supprimer_categ,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( bouton_supprimer_categ );


    /* mise en place des boutons ajout d'1 categ / sous-categ */

    bouton_ajouter_categorie = gtk_button_new_with_label ( _("Add a category") );
    gtk_button_set_relief ( GTK_BUTTON ( bouton_ajouter_categorie ),
			    GTK_RELIEF_NONE );
    gtk_signal_connect ( GTK_OBJECT ( bouton_ajouter_categorie ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( appui_sur_ajout_categorie ),
			 NULL );
    gtk_box_pack_start ( GTK_BOX ( vbox ),
			 bouton_ajouter_categorie,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( bouton_ajouter_categorie );

    bouton_ajouter_sous_categorie = gtk_button_new_with_label ( _("Add a subcategory") );
    gtk_button_set_relief ( GTK_BUTTON ( bouton_ajouter_sous_categorie ),
			    GTK_RELIEF_NONE );
    gtk_widget_set_sensitive ( bouton_ajouter_sous_categorie,
			       FALSE );
    gtk_signal_connect ( GTK_OBJECT ( bouton_ajouter_sous_categorie ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( appui_sur_ajout_sous_categorie ),
			 NULL );
    gtk_box_pack_start ( GTK_BOX ( vbox ),
			 bouton_ajouter_sous_categorie,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( bouton_ajouter_sous_categorie );

    separateur = gtk_hseparator_new ();
    gtk_box_pack_start ( GTK_BOX ( vbox ),
			 separateur,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( separateur );

    /* on met le bouton exporter */

    bouton = gtk_button_new_with_label ( _("Export") );
    gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			    GTK_RELIEF_NONE );
    gtk_signal_connect ( GTK_OBJECT ( bouton ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( exporter_categ ),
			 NULL );
    gtk_box_pack_start ( GTK_BOX ( vbox ),
			 bouton,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( bouton );

    /* on met le bouton importer */

    bouton = gtk_button_new_with_label ( _("Import") );
    gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			    GTK_RELIEF_NONE );
    gtk_signal_connect ( GTK_OBJECT ( bouton ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( importer_categ ),
			 NULL );
    gtk_box_pack_start ( GTK_BOX ( vbox ),
			 bouton,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( bouton );



    /*   création de la frame de droite */

    frame = gtk_frame_new ( NULL );
    gtk_frame_set_shadow_type ( GTK_FRAME ( frame ),
				GTK_SHADOW_IN );
    gtk_box_pack_start ( GTK_BOX ( onglet ),
			 frame,
			 TRUE,
			 TRUE,
			 5 );
    gtk_widget_show (frame );


    vbox = gtk_vbox_new ( FALSE,
			  5 );
    gtk_container_set_border_width ( GTK_CONTAINER ( vbox ),
				     10 );
    gtk_container_add ( GTK_CONTAINER ( frame ),
			vbox );
    gtk_widget_show ( vbox );

    /* on y ajoute la barre d'outils */

    gtk_box_pack_start ( GTK_BOX ( vbox ),
			 creation_barre_outils_categ(),
			 FALSE,
			 FALSE,
			 0 );

    /* création de l'arbre principal */

    scroll_window = gtk_scrolled_window_new ( NULL,
					      NULL );
    gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scroll_window ),
				     GTK_POLICY_AUTOMATIC,
				     GTK_POLICY_AUTOMATIC );
    gtk_box_pack_start ( GTK_BOX ( vbox ),
			 scroll_window,
			 TRUE,
			 TRUE,
			 0 );
    gtk_widget_show ( scroll_window );


    arbre_categ = gtk_ctree_new_with_titles ( 4,
					      0,
					      titres );
    gtk_ctree_set_line_style ( GTK_CTREE ( arbre_categ ),
			       GTK_CTREE_LINES_DOTTED );
    gtk_ctree_set_expander_style ( GTK_CTREE ( arbre_categ ),
				   GTK_CTREE_EXPANDER_CIRCULAR );
    gtk_clist_column_titles_passive ( GTK_CLIST ( arbre_categ ));

    gtk_clist_set_column_justification ( GTK_CLIST ( arbre_categ ),
					 0,
					 GTK_JUSTIFY_LEFT);
    gtk_clist_set_column_justification ( GTK_CLIST ( arbre_categ ),
					 1,
					 GTK_JUSTIFY_CENTER);
    gtk_clist_set_column_justification ( GTK_CLIST ( arbre_categ ),
					 2,
					 GTK_JUSTIFY_CENTER);
    gtk_clist_set_column_justification ( GTK_CLIST ( arbre_categ ),
					 3,
					 GTK_JUSTIFY_CENTER);

    gtk_clist_set_column_resizeable ( GTK_CLIST ( arbre_categ ),
				      0,
				      FALSE );
    gtk_clist_set_column_resizeable ( GTK_CLIST ( arbre_categ ),
				      1,
				      FALSE );
    gtk_clist_set_column_resizeable ( GTK_CLIST ( arbre_categ ),
				      2,
				      FALSE );
    gtk_clist_set_column_resizeable ( GTK_CLIST ( arbre_categ ),
				      3,
				      FALSE );

    gtk_signal_connect ( GTK_OBJECT ( arbre_categ ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( verifie_double_click_categ ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT ( arbre_categ ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( keypress_category ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT ( arbre_categ ),
			 "tree-select-row",
			 GTK_SIGNAL_FUNC ( selection_ligne_categ ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT ( arbre_categ ),
			 "tree-unselect-row",
			 GTK_SIGNAL_FUNC ( enleve_selection_ligne_categ ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT ( arbre_categ ),
			 "size-allocate",
			 GTK_SIGNAL_FUNC ( changement_taille_liste_tiers ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT ( arbre_categ ),
			 "tree-expand",
			 GTK_SIGNAL_FUNC ( ouverture_node_categ ),
			 NULL );
    gtk_container_add ( GTK_CONTAINER (  scroll_window ),
			arbre_categ );
    gtk_widget_show ( arbre_categ );


    /* on met la fontion de tri alphabétique en prenant en compte les accents */

    gtk_clist_set_compare_func ( GTK_CLIST ( arbre_categ ),
				 (GtkCListCompareFunc) classement_alphabetique_tree );

    /* la 1ère fois qu'on affichera les catég, il faudra remplir la liste */

    modif_categ = 1;


    return ( onglet );
}
/* **************************************************************************************************** */




/* **************************************************************************************************** */
/* Fonction remplit_arbre_categ */
/* prend en argument le clist arbre_categ, */
/* le vide et le remplit */
/* on y met les categ et sous categ et leur montant */
/* **************************************************************************************************** */

void remplit_arbre_categ ( void )
{
    gchar *text[4];
    GSList *liste_categ_tmp;
    gint place_categ;
    gint i;

    /* freeze le ctree */

    gtk_clist_freeze ( GTK_CLIST ( arbre_categ ));

    /*   efface l'ancien arbre */

    gtk_clist_clear ( GTK_CLIST ( arbre_categ ));


    /*   le devise est choisie dans les paramètres */

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

    if ( !devise_compte
	 ||
	 devise_compte -> no_devise != no_devise_totaux_tiers )
	devise_compte = devise_par_no ( no_devise_totaux_tiers );

    /* calcule les montants des catég et sous categ */

    calcule_total_montant_categ ();

    /* remplit l'arbre */

    liste_categ_tmp = liste_struct_categories;
    place_categ = 0;

    while ( liste_categ_tmp )
    {
	struct struct_categ *categ;
	GtkCTreeNode *ligne;
	GSList *liste_sous_categ_tmp;
	GtkCTreeNode *ligne_sous_categ;
	gint place_sous_categ;

	categ = liste_categ_tmp -> data;

	if ( etat.affiche_nb_ecritures_listes
	     &&
	     nb_ecritures_par_categ[place_categ+1] )
	    text[0] = g_strconcat ( categ -> nom_categ,
				    " (",
				    itoa ( nb_ecritures_par_categ[place_categ+1] ),
				    ")",
				    NULL );
	else
	    text[0] = categ -> nom_categ ;

	if ( tab_montant_categ[place_categ+1] )
	    text[1] = g_strdup_printf ( _("%4.2f %s"),
					tab_montant_categ[place_categ+1],
					devise_code ( devise_compte ) );
	else
	    text[1] = NULL;
	text[2] = NULL;
	text[3] = NULL;

	ligne = gtk_ctree_insert_node ( GTK_CTREE ( arbre_categ ),
					NULL,
					NULL,
					text,
					10,
					pixmap_ferme,
					masque_ferme,
					pixmap_ouvre,
					masque_ouvre,
					FALSE,
					FALSE );

	/* on associe à cette categ à l'adr de sa struct */

	gtk_ctree_node_set_row_data ( GTK_CTREE ( arbre_categ ),
				      ligne,
				      categ );

	/*       pour chaque categ, on met ses sous-categ */

	liste_sous_categ_tmp = categ -> liste_sous_categ;
	place_sous_categ = 0;

	while ( liste_sous_categ_tmp )
	{
	    struct struct_sous_categ *sous_categ;

	    sous_categ = liste_sous_categ_tmp -> data;

	    if ( tab_montant_sous_categ[place_categ]
		 &&
		 tab_montant_sous_categ[place_categ][place_sous_categ+1]
		 &&
		 etat.affiche_nb_ecritures_listes
		 &&
		 nb_ecritures_par_sous_categ[place_categ][place_sous_categ+1] )
		text[0] = g_strconcat ( sous_categ -> nom_sous_categ,
					" (",
					itoa ( nb_ecritures_par_sous_categ[place_categ][place_sous_categ+1] ),
					")",
					NULL );
	    else
		text[0] = sous_categ -> nom_sous_categ ;

	    text[1] = NULL;

	    if ( tab_montant_sous_categ[place_categ]
		 &&
		 tab_montant_sous_categ[place_categ][place_sous_categ+1] )
		text[2] = g_strdup_printf ( _("%4.2f %s"),
					    tab_montant_sous_categ[place_categ][place_sous_categ+1],
					    devise_code ( devise_compte ) );
	    else
		text[2] = NULL;

	    text[3] = NULL;

	    ligne_sous_categ = gtk_ctree_insert_node ( GTK_CTREE ( arbre_categ ),
						       ligne,
						       NULL,
						       text,
						       10,
						       NULL,
						       NULL,
						       NULL,
						       NULL,
						       FALSE,
						       FALSE );

	    /* on associe cette sous_categ à l'adr de sa struct */

	    gtk_ctree_node_set_row_data ( GTK_CTREE ( arbre_categ ),
					  ligne_sous_categ,
					  sous_categ );

	    /* pour chacun des sous categ, on met un fils bidon pour pouvoir l'ouvrir */

	    ligne_sous_categ = gtk_ctree_insert_node ( GTK_CTREE ( arbre_categ ),
						       ligne_sous_categ,
						       NULL,
						       text,
						       5,
						       NULL,
						       NULL,
						       NULL,
						       NULL,
						       FALSE,
						       FALSE );

	    /* on associe le fils bidon à -1 */

	    gtk_ctree_node_set_row_data ( GTK_CTREE ( arbre_categ ),
					  ligne_sous_categ,
					  GINT_TO_POINTER (-1));


	    place_sous_categ++;
	    liste_sous_categ_tmp = liste_sous_categ_tmp -> next;
	}

	/*       on a fini de saisir les sous catégories, s'il y avait des opés sans sous catég, on les mets ici */

	if ( tab_montant_sous_categ[place_categ]
	     &&
	     nb_ecritures_par_sous_categ[place_categ][0])
	{
	    if ( etat.affiche_nb_ecritures_listes )
		text[0] = g_strdup_printf ( _("No sub-category (%d)"),
					    nb_ecritures_par_sous_categ[place_categ][0] );
	    else
		text[0] = g_strdup ( _("No sub-category"));

	    text[1] = NULL;

	    if ( tab_montant_sous_categ[place_categ][0] )
		text[2] = g_strdup_printf ( _("%4.2f %s"),
					    tab_montant_sous_categ[place_categ][0],
					    devise_code ( devise_compte ) );
	    else
		text[2] = NULL;

	    text[3] = NULL;

	    ligne_sous_categ = gtk_ctree_insert_node ( GTK_CTREE ( arbre_categ ),
						       ligne,
						       NULL,
						       text,
						       10,
						       NULL,
						       NULL,
						       NULL,
						       NULL,
						       FALSE,
						       FALSE );

	    /* pour chacun des sous categ, on met un fils bidon pour pouvoir l'ouvrir */

	    ligne_sous_categ = gtk_ctree_insert_node ( GTK_CTREE ( arbre_categ ),
						       ligne_sous_categ,
						       NULL,
						       text,
						       5,
						       NULL,
						       NULL,
						       NULL,
						       NULL,
						       FALSE,
						       FALSE );

	    /* on associe le fils bidon à -1 */

	    gtk_ctree_node_set_row_data ( GTK_CTREE ( arbre_categ ),
					  ligne_sous_categ,
					  GINT_TO_POINTER (-1));

	}
	place_categ++;
	liste_categ_tmp = liste_categ_tmp -> next;
    }

    /*   on a fini de mettre les catégories, on met ici les opés sans catég */

    if ( tab_montant_categ[0] )
    {
	GtkCTreeNode *ligne;
	GtkCTreeNode *ligne_sous_categ;

	if ( etat.affiche_nb_ecritures_listes
	     &&
	     nb_ecritures_par_categ[0] )
	    text[0] = g_strdup_printf ( _("No category (%d)"),
					nb_ecritures_par_categ[0] );
	else
	    text[0] = _("No category");

	text[1] = g_strdup_printf ( _("%4.2f %s"),
				    tab_montant_categ[0],
				    devise_code ( devise_compte ) );
	text[2] = NULL;
	text[3] = NULL;

	ligne = gtk_ctree_insert_node ( GTK_CTREE ( arbre_categ ),
					NULL,
					NULL,
					text,
					10,
					pixmap_ferme,
					masque_ferme,
					pixmap_ouvre,
					masque_ouvre,
					FALSE,
					FALSE );


	/* on met aucune sous categ */

	if ( etat.affiche_nb_ecritures_listes
	     &&
	     nb_ecritures_par_categ[0] )
	    text[0] = g_strdup_printf ( _("No sub-category (%d)"),
					nb_ecritures_par_categ[0]);
	else
	    text[0] = _("No sub-category");

	text[1] = NULL;
	text[2] = g_strdup_printf ( _("%4.2f %s"),
				    tab_montant_categ[0],
				    devise_code ( devise_compte ) );
	text[3] = NULL;

	ligne_sous_categ = gtk_ctree_insert_node ( GTK_CTREE ( arbre_categ ),
						   ligne,
						   NULL,
						   text,
						   10,
						   NULL,
						   NULL,
						   NULL,
						   NULL,
						   FALSE,
						   FALSE );

	/* on met un fils bidon pour pouvoir l'ouvrir */

	ligne_sous_categ = gtk_ctree_insert_node ( GTK_CTREE ( arbre_categ ),
						   ligne_sous_categ,
						   NULL,
						   text,
						   5,
						   NULL,
						   NULL,
						   NULL,
						   NULL,
						   FALSE,
						   FALSE );

	/* on associe le fils bidon à -1 */

	gtk_ctree_node_set_row_data ( GTK_CTREE ( arbre_categ ),
				      ligne_sous_categ,
				      GINT_TO_POINTER (-1));
    }

    /*   on efface les variables */

    free ( tab_montant_categ );
    free ( nb_ecritures_par_categ );

    for ( i=0 ; i<nb_enregistrements_categories ; i++ )
	free ( tab_montant_sous_categ[i] );
    free ( tab_montant_sous_categ );

    for ( i=0 ; i<nb_enregistrements_categories ; i++ )
	free ( nb_ecritures_par_sous_categ[i] );
    free ( nb_ecritures_par_sous_categ );


    gtk_ctree_sort_recursive ( GTK_CTREE ( arbre_categ ),
			       NULL );

    /* defreeze le ctree */

    gtk_clist_thaw ( GTK_CLIST ( arbre_categ ));

    enleve_selection_ligne_categ ();
    modif_categ = 0;
}
/* **************************************************************************************************** */



/* **************************************************************************************************** */
/* Fonction ouverture_node_categ */
/* appeléé lorsqu'on ouvre une categ, sous categ ou le compte d'une categ */
/* remplit ce qui doit être affiché */
/* **************************************************************************************************** */

gboolean ouverture_node_categ ( GtkWidget *arbre, GtkCTreeNode *node, 
				gpointer null )
{			    
    GtkCTreeRow *row;
    gchar *text[4];
    GtkCTreeNode *node_insertion;

    row = GTK_CTREE_ROW ( node );

    /*   si on ouvre une categ, on fait rien */

    if ( !row || row->level == 1 )
	return(FALSE);

    /*   si le fiston = -1, c'est qu'il n'a pas encore été créé */
    /* dans le cas contraire, on vire */

    if ( GPOINTER_TO_INT ( gtk_ctree_node_get_row_data ( GTK_CTREE ( arbre_categ ),
							 row -> children )) != -1 )
	return(FALSE);

    /* freeze le ctree */

    gtk_clist_freeze ( GTK_CLIST ( arbre_categ ));


    /* on commence par virer la ligne bidon qui était attachée à ce noeud */

    gtk_ctree_remove_node ( GTK_CTREE ( arbre_categ ),
			    row -> children );

    /* séparation entre ouverture de sous-categ ( 2 ) et ouverture de compte ( 3 ) */

    if ( row -> level == 2 )
    {
	/* c'est une ouverture de sous categ, on récupère sa structure  */

	struct struct_categ *categ;
	struct struct_sous_categ *sous_categ;
	gint no_categ;
	gint no_sous_categ;
	gint i;

	/*       soit il y a une categ et une sous categ, soit c'est aucune categ (donc categ = 0) */

	if ( ( categ = gtk_ctree_node_get_row_data ( GTK_CTREE ( arbre_categ ),
						     row -> parent )) &&
	     categ != GINT_TO_POINTER(-1) && categ != NULL )
	{
	    no_categ = categ -> no_categ;
	    if ( ( sous_categ = gtk_ctree_node_get_row_data ( GTK_CTREE ( arbre_categ ),
							      node)) &&
		 sous_categ != GINT_TO_POINTER(-1) && sous_categ != NULL )
		no_sous_categ = sous_categ -> no_sous_categ;
	    else
		no_sous_categ = 0;
	}
	else
	{
	    no_categ = 0;
	    no_sous_categ = 0;
	}

	/* on va scanner tous les comptes, dès qu'un tiers correspondant au tiers sélectionné est trouvé */
	/* on affiche le nom du compte */

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

	for ( i = 0 ; i < nb_comptes ; i++ )
	{
	    GSList *pointeur_ope;

	    pointeur_ope = LISTE_OPERATIONS;

	    while ( pointeur_ope )
	    {
		struct structure_operation *operation;

		operation = pointeur_ope -> data;

		if ( operation &&
		     operation -> categorie == no_categ &&
		     operation -> sous_categorie == no_sous_categ &&
		     !operation -> relation_no_operation &&
		     !operation -> operation_ventilee )
		{
		    /* affiche le compte courant */

		    text[3] = calcule_total_montant_categ_par_compte ( operation -> categorie,
								       operation -> sous_categorie,
								       operation -> no_compte );

		    if ( etat.affiche_nb_ecritures_listes
			 &&
			 nb_ecritures_par_comptes )
			text[0] = g_strconcat ( NOM_DU_COMPTE,
						" (",
						itoa ( nb_ecritures_par_comptes ),
						")",
						NULL );
		    else
			text[0] = NOM_DU_COMPTE;

		    text[1] = NULL;
		    text[2] = NULL;

		    node_insertion = gtk_ctree_insert_node ( GTK_CTREE ( arbre_categ ),
							     node,
							     NULL,
							     text,
							     5,
							     NULL,
							     NULL,
							     NULL,
							     NULL,
							     FALSE,
							     FALSE );

		    /* associe le no de compte à la ligne du compte */

		    gtk_ctree_node_set_row_data ( GTK_CTREE ( arbre_categ ),
						  node_insertion,
						  GINT_TO_POINTER ( i ));

		    /* on met une ligne bidon pour pouvoir l'ouvrir */

		    node_insertion = gtk_ctree_insert_node ( GTK_CTREE ( arbre_categ ),
							     node_insertion,
							     NULL,
							     text,
							     5,
							     NULL,
							     NULL,
							     NULL,
							     NULL,
							     FALSE,
							     FALSE );

		    /* on associe le fils bidon à -1 */

		    gtk_ctree_node_set_row_data ( GTK_CTREE ( arbre_categ ),
						  node_insertion,
						  GINT_TO_POINTER (-1));

		    pointeur_ope = NULL;
		}
		else
		    pointeur_ope = pointeur_ope -> next;
	    }

	    p_tab_nom_de_compte_variable++;
	}
    }
    else
    {
	/* c'est une ouverture d'un compte */
	/*       cette fois, on fait le tour de toutes les opés du compte pour afficher celles qui correspondent à la categ */

	struct struct_categ *categ;
	struct struct_sous_categ *sous_categ;
	GSList *pointeur_ope;
	gint no_categ;
	gint no_sous_categ;

	if (( categ = gtk_ctree_node_get_row_data ( GTK_CTREE ( arbre_categ ),
						    GTK_CTREE_ROW ( row -> parent ) -> parent )) && 
	    categ != GINT_TO_POINTER(-1) && categ != NULL )
	{
	    no_categ = categ -> no_categ;

	    if ( (sous_categ = gtk_ctree_node_get_row_data ( GTK_CTREE ( arbre_categ ),
							     row -> parent ))
		 && sous_categ != GINT_TO_POINTER(-1) && sous_categ != NULL )
		no_sous_categ = sous_categ -> no_sous_categ;
	    else
		no_sous_categ = 0;
	}
	else
	{
	    no_categ = 0;
	    no_sous_categ = 0;
	}

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + GPOINTER_TO_INT ( gtk_ctree_node_get_row_data ( GTK_CTREE ( arbre_categ ),
													     node ));

	pointeur_ope = LISTE_OPERATIONS;

	while ( pointeur_ope )
	{
	    struct struct_devise *devise_operation;
	    struct structure_operation *operation;

	    operation = pointeur_ope -> data;

	    if ( operation )
		devise_operation = devise_par_no ( operation -> devise );

	    if ( operation &&
		 operation -> categorie == no_categ &&
		 operation -> sous_categorie == no_sous_categ &&
		 !operation -> relation_no_operation &&
		 !operation -> operation_ventilee )
	    {
		if ( operation -> notes )
		{
		    if ( operation -> no_operation_ventilee_associee )
			text[0] = g_strdup_printf ( _("%d/%d/%d : %4.2f %s (breakdown) [ %s ]"),
						    operation -> jour,
						    operation -> mois,
						    operation -> annee,
						    operation -> montant,
						    devise_operation -> code_devise,
						    operation -> notes );
		    else
			text[0] = g_strdup_printf ( "%d/%d/%d : %4.2f %s [ %s ]",
						    operation -> jour,
						    operation -> mois,
						    operation -> annee,
						    operation -> montant,
						    devise_operation -> code_devise,
						    operation -> notes );
		}
		else
		{
		    if ( operation -> no_operation_ventilee_associee )
			text[0] = g_strdup_printf ( _("%d/%d/%d : %4.2f %s (breakdown)"),
						    operation -> jour,
						    operation -> mois,
						    operation -> annee,
						    operation -> montant,
						    devise_operation -> code_devise );
		    else
			text[0] = g_strdup_printf ( "%d/%d/%d : %4.2f %s",
						    operation -> jour,
						    operation -> mois,
						    operation -> annee,
						    operation -> montant,
						    devise_operation -> code_devise );
		}

		text[1] = NULL;
		text[2] = NULL;
		text[3] = NULL;

		node_insertion = gtk_ctree_insert_node ( GTK_CTREE ( arbre_categ ),
							 node,
							 NULL,
							 text,
							 5,
							 NULL,
							 NULL,
							 NULL,
							 NULL,
							 FALSE,
							 FALSE );

		/* on associe à cette opé l'adr de sa struct */

		gtk_ctree_node_set_row_data ( GTK_CTREE ( arbre_categ ),
					      node_insertion,
					      operation );
	    }
	    pointeur_ope = pointeur_ope -> next;
	}
    }

    /* defreeze le ctree */

    gtk_clist_thaw ( GTK_CLIST ( arbre_categ ));

    return FALSE;
}
/* **************************************************************************************************** */




/* **************************************************************************************************** */
gboolean selection_ligne_categ ( GtkCTree *arbre_categ, GtkCTreeNode *noeud,
				 gint colonne, gpointer null )
{
    GtkCTreeNode *node_tmp;

    node_tmp = noeud;

    while ( GTK_CTREE_ROW ( node_tmp ) -> level  != 1 )
	node_tmp = GTK_CTREE_ROW ( node_tmp ) -> parent;

    if ( GTK_CTREE_ROW ( noeud ) -> level != 1 )
	gtk_widget_set_sensitive ( bouton_ajouter_sous_categorie,
				   FALSE );
    else
	gtk_widget_set_sensitive ( bouton_ajouter_sous_categorie,
				   TRUE );



    if ( GTK_CTREE_ROW ( noeud ) -> level  == 1 &&
	 gtk_ctree_node_get_row_data ( GTK_CTREE ( arbre_categ ),
				       noeud ) &&
	 gtk_ctree_node_get_row_data ( GTK_CTREE ( arbre_categ ),
				       noeud ) != GINT_TO_POINTER(-1) )
    {
	struct struct_categ *categ;

	categ = gtk_ctree_node_get_row_data ( GTK_CTREE ( arbre_categ ),
					      noeud );

	gtk_signal_handler_block_by_func ( GTK_OBJECT ( entree_nom_categ ),
					   GTK_SIGNAL_FUNC ( modification_du_texte_categ),
					   NULL );

	gtk_entry_set_text ( GTK_ENTRY ( entree_nom_categ ),
			     categ -> nom_categ );

	gtk_signal_handler_unblock_by_func ( GTK_OBJECT ( entree_nom_categ ),
					     GTK_SIGNAL_FUNC ( modification_du_texte_categ),
					     NULL );

	gtk_object_set_data ( GTK_OBJECT ( entree_nom_categ ),
			      "adr_struct_categ",
			      categ );
	gtk_object_set_data ( GTK_OBJECT ( entree_nom_categ ),
			      "adr_struct_sous_categ",
			      NULL );

	gtk_widget_set_sensitive ( bouton_supprimer_categ,
				   TRUE );
	gtk_widget_set_sensitive ( entree_nom_categ,
				   TRUE );
	gtk_widget_set_sensitive ( bouton_categ_debit,
				   TRUE );
	gtk_widget_set_sensitive ( bouton_categ_credit,
				   TRUE );

	gtk_signal_handler_block_by_func ( GTK_OBJECT ( bouton_categ_debit ),
					   GTK_SIGNAL_FUNC ( modification_du_texte_categ ),
					   NULL );
	if ( categ -> type_categ )
	    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_categ_debit ),
					   TRUE );
	else
	    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_categ_credit ),
					   TRUE );

	gtk_signal_handler_unblock_by_func ( GTK_OBJECT ( bouton_categ_debit ),
					     GTK_SIGNAL_FUNC ( modification_du_texte_categ ),
					     NULL );
    }
    else
	if ( GTK_CTREE_ROW ( noeud ) -> level  == 2
	     &&
	     gtk_ctree_node_get_row_data ( GTK_CTREE ( arbre_categ ),
					   noeud ) > 0
	     &&
	     gtk_ctree_node_get_row_data ( GTK_CTREE ( arbre_categ ),
					   GTK_CTREE_ROW ( noeud ) -> parent ) > 0 )
	{
	    struct struct_sous_categ *sous_categ;

	    sous_categ = gtk_ctree_node_get_row_data ( GTK_CTREE ( arbre_categ ),
						       noeud );
	    if ( sous_categ == GINT_TO_POINTER(-1) )
		return(FALSE);

	    gtk_signal_handler_block_by_func ( GTK_OBJECT ( entree_nom_categ ),
					       GTK_SIGNAL_FUNC ( modification_du_texte_categ),
					       NULL );

	    gtk_entry_set_text ( GTK_ENTRY ( entree_nom_categ ),
				 sous_categ -> nom_sous_categ );

	    gtk_signal_handler_unblock_by_func ( GTK_OBJECT ( entree_nom_categ ),
						 GTK_SIGNAL_FUNC ( modification_du_texte_categ),
						 NULL );

	    gtk_object_set_data ( GTK_OBJECT ( entree_nom_categ ),
				  "adr_struct_categ",
				  NULL );
	    gtk_object_set_data ( GTK_OBJECT ( entree_nom_categ ),
				  "adr_struct_sous_categ",
				  sous_categ );

	    gtk_widget_set_sensitive ( bouton_supprimer_categ,
				       TRUE );
	    gtk_widget_set_sensitive ( entree_nom_categ,
				       TRUE );
	}

    return FALSE;
}
/* **************************************************************************************************** */



gboolean verifie_double_click_categ ( GtkWidget *liste, GdkEventButton *ev,
				      gpointer null )
{
    if ( ev -> type == GDK_2BUTTON_PRESS )
    {
	expand_selected_category();
    }

    return FALSE;
}


gboolean keypress_category ( GtkWidget *widget, GdkEventKey *ev, gint *no_origine )
{
    GtkCTreeNode *node;

    if ( ev->keyval == GDK_Return || 
	 ev->keyval == GDK_KP_Enter )
    { 
	node = gtk_ctree_node_nth ( GTK_CTREE(arbre_categ), 
				    GTK_CLIST(arbre_categ) -> focus_row );
	gtk_ctree_select ( GTK_CTREE(arbre_categ), node );
	gtk_ctree_expand ( GTK_CTREE(arbre_categ), node );

	expand_selected_category ();
    }

    return FALSE;
}


void expand_selected_category ()
{
    struct structure_operation *operation;

    if ( GTK_CLIST ( arbre_categ ) -> selection
	 &&
	 ( GTK_CTREE_ROW ( ( GTK_CLIST ( arbre_categ ) -> selection ) -> data ) -> level == 4
	   ||
	   ( GTK_CTREE_ROW ( ( GTK_CLIST ( arbre_categ ) -> selection ) -> data ) -> level == 3
	     &&
	     gtk_ctree_node_get_row_data ( GTK_CTREE ( arbre_categ ),
					   GTK_CTREE_ROW ( GTK_CTREE_ROW (( GTK_CLIST ( arbre_categ ) -> selection ) -> data ) -> parent ) -> parent ) > 0)))
    {
	/* passage sur le compte concerné */

	operation = gtk_ctree_node_get_row_data ( GTK_CTREE ( arbre_categ ),
						  GTK_CTREE_NODE ( ( GTK_CLIST ( arbre_categ ) -> selection ) -> data ) );

	if (!operation || operation == GINT_TO_POINTER(-1))
	    return;

	/* si c'est une opé de ventilation, on se place sur l'opé
	   ventilée correspondante */ 

	operation = operation_par_no ( operation -> no_operation_ventilee_associee,
				       operation -> no_compte );

	if ( operation )
	{
	    changement_compte ( GINT_TO_POINTER ( operation -> no_compte ));

	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_courant;

	    if ( operation -> pointe == 3 && !AFFICHAGE_R )
	    {
		AFFICHAGE_R = 1;
		remplissage_liste_operations ( compte_courant );
	    }

	    selectionne_ligne ( operation );
	}
    }

}


/* **************************************************************************************************** */
gboolean enleve_selection_ligne_categ ( void )
{
    gtk_widget_set_sensitive ( bouton_supprimer_categ,
			       FALSE );
    gtk_widget_set_sensitive ( bouton_modif_categ_modifier,
			       FALSE );
    gtk_widget_set_sensitive ( bouton_modif_categ_annuler,
			       FALSE );
    gtk_widget_set_sensitive ( entree_nom_categ,
			       FALSE );
    gtk_widget_set_sensitive ( bouton_categ_debit,
			       FALSE );
    gtk_widget_set_sensitive ( bouton_categ_credit,
			       FALSE );
    gtk_widget_set_sensitive ( bouton_ajouter_sous_categorie,
			       FALSE );


    gtk_signal_handler_block_by_func ( GTK_OBJECT ( entree_nom_categ ),
				       GTK_SIGNAL_FUNC ( modification_du_texte_categ),
				       NULL );

    gtk_editable_delete_text ( GTK_EDITABLE ( entree_nom_categ ),
			       0,
			       -1 );

    gtk_signal_handler_unblock_by_func ( GTK_OBJECT ( entree_nom_categ ),
					 GTK_SIGNAL_FUNC ( modification_du_texte_categ),
					 NULL );

    return FALSE;
}
/* **************************************************************************************************** */






/* **************************************************************************************************** */
void modification_du_texte_categ ( void )
{
    gtk_widget_set_sensitive ( bouton_modif_categ_modifier,
			       TRUE );
    gtk_widget_set_sensitive ( bouton_modif_categ_annuler,
			       TRUE );
}
/* **************************************************************************************************** */





/* **************************************************************************************************** */
void clique_sur_modifier_categ ( void )
{
    GtkCTreeNode *node;


    /* si c'est une categ et si c'est une modif du nom, on doit réafficher la liste des categ et les listes des opés, sinon, on change juste la categ et */
    /* met à jour les listes de categories */
    /* si c'est une sous categ, c'est obligmy_atoirement un changement de nom */

    if ( gtk_object_get_data ( GTK_OBJECT (  entree_nom_categ ),
			       "adr_struct_categ" ) )
    {
	/* c'est une categorie */

	struct struct_categ *categ;


	categ =  gtk_object_get_data ( GTK_OBJECT (  entree_nom_categ ),
				       "adr_struct_categ" );


	if ( strcmp ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree_nom_categ ))),
		      categ -> nom_categ ))
	{
	    free ( categ -> nom_categ );

	    categ -> nom_categ = g_strdup ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree_nom_categ ))) );


	    node = GTK_CTREE_NODE ( ( GTK_CLIST ( arbre_categ ) -> selection ) -> data );

	    if ( GTK_CTREE_ROW ( node ) -> expanded )
		gtk_ctree_node_set_pixtext ( GTK_CTREE ( arbre_categ ),
					     node,
					     0,
					     categ -> nom_categ,
					     10,
					     pixmap_ouvre,
					     masque_ouvre );
	    else
		gtk_ctree_node_set_pixtext ( GTK_CTREE ( arbre_categ ),
					     node,
					     0,
					     categ -> nom_categ,
					     10,
					     pixmap_ferme,
					     masque_ferme );

	    demande_mise_a_jour_tous_comptes ();
	}
	else
	    categ -> type_categ = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_categ_debit ));
    }
    else
    {
	/* c'est une sous categorie */

	struct struct_sous_categ *sous_categ;

	sous_categ =  gtk_object_get_data ( GTK_OBJECT (  entree_nom_categ ),
					    "adr_struct_sous_categ" );

	free ( sous_categ -> nom_sous_categ );

	sous_categ -> nom_sous_categ = g_strdup ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree_nom_categ ))) );


	node = GTK_CTREE_NODE ( ( GTK_CLIST ( arbre_categ ) -> selection ) -> data );

	gtk_ctree_node_set_text ( GTK_CTREE ( arbre_categ ),
				  node,
				  0,
				  sous_categ -> nom_sous_categ );

	demande_mise_a_jour_tous_comptes ();
    }

    gtk_clist_unselect_all ( GTK_CLIST ( arbre_categ ));

    if ( mise_a_jour_combofix_categ_necessaire )
	mise_a_jour_combofix_categ ();

    gtk_widget_set_sensitive ( bouton_modif_categ_modifier,
			       FALSE );
    gtk_widget_set_sensitive ( bouton_modif_categ_annuler,
			       FALSE );

    modification_fichier(TRUE);
}
/* **************************************************************************************************** */


/* **************************************************************************************************** */
void clique_sur_annuler_categ ( void )
{
    if ( gtk_object_get_data ( GTK_OBJECT (  entree_nom_categ ),
			       "adr_struct_categ" ))
    {
	/* c'est une categorie */

	struct struct_categ *categ;

	categ =  gtk_object_get_data ( GTK_OBJECT (  entree_nom_categ ),
				       "adr_struct_categ" );

	gtk_entry_set_text ( GTK_ENTRY ( entree_nom_categ ),
			     categ -> nom_categ );

	if ( categ -> type_categ )
	    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_categ_debit ),
					   TRUE );
	else
	    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_categ_credit ),
					   TRUE );
    }
    else
    {
	/* c'est une sous categorie */

	struct struct_sous_categ *sous_categ;

	sous_categ =  gtk_object_get_data ( GTK_OBJECT (  entree_nom_categ ),
					    "adr_struct_sous_categ" );

	gtk_entry_set_text ( GTK_ENTRY ( entree_nom_categ ),
			     sous_categ -> nom_sous_categ );

    }

    gtk_widget_set_sensitive ( bouton_modif_categ_modifier,
			       FALSE );
    gtk_widget_set_sensitive ( bouton_modif_categ_annuler,
			       FALSE );

}
/* **************************************************************************************************** */





/* **************************************************************************************************** */
void supprimer_categ ( void )
{
    struct struct_categ *categ;
    GtkCTreeNode *node;
    gint i;
    gint ope_trouvee;
    gint echeance_trouvee;

    /* ALAIN-FIXME il y a des GSList *liste_tmp un peu partout dans cette fonction,
       il faudrait voir si on ne peut pas se contenter d'une seule */
    GSList *liste_tmp2;


    if ( !gtk_object_get_data ( GTK_OBJECT (  entree_nom_categ ),
				"adr_struct_categ" ))
    {
	supprimer_sous_categ();
	return;
    }

    node = GTK_CTREE_NODE ( ( GTK_CLIST ( arbre_categ ) -> selection ) -> data );

    categ = gtk_ctree_node_get_row_data ( GTK_CTREE ( arbre_categ ),
					  node );
    if ( categ <= 0 )
	return;

    /* fait le tour des opés pour en trouver une qui a cette catégorie */

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte;
    ope_trouvee = 0;

    for ( i=0 ; i<nb_comptes ; i++ )
    {
	GSList *liste_tmp;

	liste_tmp = LISTE_OPERATIONS;

	while ( liste_tmp )
	{
	    struct structure_operation *operation;

	    operation = liste_tmp -> data;

	    if ( operation -> categorie == categ -> no_categ )
	    {
		ope_trouvee = 1;
		liste_tmp = NULL;
		i = nb_comptes;
	    }
	    else
		liste_tmp = liste_tmp -> next;
	}
	p_tab_nom_de_compte_variable++;
    }

    /* fait le tour des échéances pour en trouver une qui a cette catégorie  */

    liste_tmp2 = liste_struct_echeances;
    echeance_trouvee = 0;

    while ( liste_tmp2 )
    {
	struct operation_echeance *echeance;

	echeance = liste_tmp2 -> data;

	if ( echeance -> categorie == categ -> no_categ )
	{
	    echeance_trouvee = 1;
	    liste_tmp2 = NULL;
	}
	else
	    liste_tmp2 = liste_tmp2 -> next;
    }


    if ( ope_trouvee || echeance_trouvee )
    {
	GtkWidget *hbox, *bouton_categ_generique, *combofix, *dialog, *bouton_transfert;
	GSList *liste_combofix, *pointeur, *liste_tmp, *liste_categ_credit, *liste_categ_debit;
	gint resultat, i, nouveau_no_categ, nouveau_no_sous_categ;
	struct struct_categ *nouvelle_categ;
	struct struct_sous_categ *nouvelle_sous_categ;
	gchar **split_categ;

	dialog = dialogue_special_no_run ( GTK_MESSAGE_WARNING, GTK_BUTTONS_OK_CANCEL,
					   make_hint ( _("Selected category still contains transactions."),
						       _("If you want to remove this category but want to keep transactions, you can transfer them to another category.  Otherwise, transactions can be simply deleted along with their category.") ));

	/*       mise en place du choix tranfert vers un autre categ */

	hbox = gtk_hbox_new ( FALSE,
			      5 );
	gtk_box_pack_start ( GTK_BOX ( GTK_DIALOG ( dialog ) -> vbox ),
			     hbox,
			     FALSE,
			     FALSE,
			     0 );

	bouton_transfert = gtk_radio_button_new_with_label ( NULL,
							     COLON(_("Transfer transactions to category"))  );
	gtk_box_pack_start ( GTK_BOX ( hbox ),
			     bouton_transfert,
			     FALSE,
			     FALSE,
			     0 );


	pointeur = liste_struct_categories;
	liste_combofix = NULL;
	liste_categ_credit = NULL;
	liste_categ_debit = NULL;

	while ( pointeur )
	{
	    struct struct_categ *categorie;
	    GSList *sous_pointeur;

	    categorie = pointeur -> data;

	    if ( categorie-> no_categ != categ -> no_categ )
	    {
		if ( categorie -> type_categ )
		    liste_categ_debit = g_slist_append ( liste_categ_debit,
							 g_strdup ( categorie -> nom_categ ) );
		else
		    liste_categ_credit = g_slist_append ( liste_categ_credit,
							  g_strdup ( categorie -> nom_categ ) );


		sous_pointeur = categorie -> liste_sous_categ;

		while ( sous_pointeur )
		{
		    struct struct_sous_categ *sous_categ;

		    sous_categ = sous_pointeur -> data;

		    if ( categorie -> type_categ )
			liste_categ_debit = g_slist_append ( liste_categ_debit,
							     g_strconcat ( "\t",
									   sous_categ -> nom_sous_categ,
									   NULL ) );
		    else
			liste_categ_credit = g_slist_append ( liste_categ_credit,
							      g_strconcat ( "\t",
									    sous_categ -> nom_sous_categ,
									    NULL ) );

		    sous_pointeur = sous_pointeur -> next;
		}
	    }
	    pointeur = pointeur -> next;
	}

	/*   on ajoute les listes des crédits / débits à la liste du combofix du formulaire */

	liste_combofix = g_slist_append ( liste_combofix,
					  liste_categ_debit );
	liste_combofix = g_slist_append ( liste_combofix,
					  liste_categ_credit );


	combofix = gtk_combofix_new_complex ( liste_combofix,
					      TRUE,
					      TRUE,
					      TRUE,
					      0 );
	gtk_box_pack_start ( GTK_BOX ( hbox ),
			     combofix,
			     TRUE,
			     TRUE,
			     0 );

	/*       mise en place du choix supprimer le categ */

	hbox = gtk_hbox_new ( FALSE,
			      5 );
	gtk_box_pack_start ( GTK_BOX ( GTK_DIALOG ( dialog ) -> vbox ),
			     hbox,
			     FALSE,
			     FALSE,
			     0 );

	bouton_categ_generique = gtk_radio_button_new_with_label ( gtk_radio_button_group ( GTK_RADIO_BUTTON ( bouton_transfert )),
								   PRESPACIFY(_("Just remove this category.")) );
	gtk_box_pack_start ( GTK_BOX ( hbox ),
			     bouton_categ_generique,
			     FALSE,
			     FALSE,
			     0 );

	gtk_widget_show_all ( dialog );


retour_dialogue:
	resultat = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

	if ( resultat != GTK_RESPONSE_OK )
	{
	    gtk_widget_destroy ( GTK_WIDGET ( dialog ) );
	    return;
	}

	nouveau_no_categ = 0;
	nouveau_no_sous_categ = 0;

	if ( gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_transfert )) )
	{

	    if ( !strlen (gtk_combofix_get_text ( GTK_COMBOFIX ( combofix ))))
	    {
		dialogue_warning_hint ( _("It is compulsory to specify a destination category to move transactions but no category was entered."),
				      _("Please enter a category!"));
		goto retour_dialogue;
	    }

	    /* transfert des categ au niveau des listes */

	    split_categ = g_strsplit ( gtk_combofix_get_text ( GTK_COMBOFIX ( combofix )),
				       " : ",
				       2 );

	    nouvelle_categ = categ_par_nom ( split_categ[0],
					     0,
					     0,
					     0 );

	    if ( nouvelle_categ )
	    {
		nouveau_no_categ = nouvelle_categ -> no_categ;

		nouvelle_sous_categ = sous_categ_par_nom ( nouvelle_categ,
							    split_categ[1],
							    0 );

		if ( nouvelle_sous_categ )
		    nouveau_no_sous_categ = nouvelle_sous_categ -> no_sous_categ;
	    }

	    g_strfreev ( split_categ );
	}

	/* on fait le tour des opés pour mettre le nouveau numéro de categ et sous_categ */

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

	for ( i = 0 ; i < nb_comptes ; i++ )
	{
	    liste_tmp = LISTE_OPERATIONS;

	    while ( liste_tmp )
	    {
		struct structure_operation *operation;

		operation = liste_tmp -> data;

		if ( operation -> categorie == categ -> no_categ )
		{
		    operation -> categorie = nouveau_no_categ;
		    operation -> sous_categorie = nouveau_no_sous_categ;
		}

		liste_tmp = liste_tmp -> next;
	    }

	    p_tab_nom_de_compte_variable++;
	}

	/* fait le tour des échéances pour mettre le nouveau numéro de categ et sous_categ  */

	liste_tmp = liste_struct_echeances;

	while ( liste_tmp )
	{
	    struct operation_echeance *echeance;

	    echeance = liste_tmp -> data;

	    if ( echeance -> categorie == categ -> no_categ )
	    {
		echeance -> categorie = nouveau_no_categ;
		echeance -> sous_categorie = nouveau_no_sous_categ;

		if ( !echeance -> categorie )
		    echeance -> compte_virement = -1;
	    }

	    liste_tmp = liste_tmp -> next;
	}


	demande_mise_a_jour_tous_comptes ();

	gtk_widget_destroy ( GTK_WIDGET ( dialog ) );
    }


    /* supprime dans la liste des categ  */

    liste_struct_categories = g_slist_remove ( liste_struct_categories,
					       categ );
    nb_enregistrements_categories--;

    enleve_selection_ligne_categ();

    if ( mise_a_jour_combofix_categ_necessaire )
	mise_a_jour_combofix_categ  ();
    remplit_arbre_categ ();

    modification_fichier(TRUE);
}
/* **************************************************************************************************** */








/* **************************************************************************************************** */
void supprimer_sous_categ ( void )
{
    struct struct_categ *categ;
    struct struct_sous_categ *sous_categ;
    GtkCTreeNode *node;
    gint i;
    gint ope_trouvee;
    gint echeance_trouvee;

    /* ALAIN-FIXME il y a des GSList *liste_tmp un peu partout dans cette fonction,
       il faudrait voir si on ne peut pas se contenter d'une seule */
    GSList *liste_tmp2;



    node = GTK_CTREE_NODE ( ( GTK_CLIST ( arbre_categ ) -> selection ) -> data );

    sous_categ = gtk_ctree_node_get_row_data ( GTK_CTREE ( arbre_categ ),
					       node );
    if ( sous_categ <= 0 )
	return;

    node = GTK_CTREE_ROW ( ( GTK_CLIST ( arbre_categ ) -> selection ) -> data ) -> parent;

    categ = gtk_ctree_node_get_row_data ( GTK_CTREE ( arbre_categ ),
					  node );
    if ( categ <= 0 )
	return;


    /* fait le tour des opés pour en trouver une qui a cette sous-catégorie */

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte;
    ope_trouvee = 0;

    for ( i=0 ; i<nb_comptes ; i++ )
    {
	GSList *liste_tmp;

	liste_tmp = LISTE_OPERATIONS;

	while ( liste_tmp )
	{
	    struct structure_operation *operation;

	    operation = liste_tmp -> data;

	    if ( operation -> categorie == categ -> no_categ
		 &&
		 operation -> sous_categorie == sous_categ -> no_sous_categ )
	    {
		ope_trouvee = 1;  modification_fichier(TRUE);

		liste_tmp = NULL;
		i = nb_comptes;
	    }
	    else
		liste_tmp = liste_tmp -> next;
	}
	p_tab_nom_de_compte_variable++;
    }


    /* fait le tour des échéances pour en trouver une qui a cette catégorie  */

    liste_tmp2 = liste_struct_echeances;
    echeance_trouvee = 0;

    while ( liste_tmp2 )
    {
	struct operation_echeance *echeance;

	echeance = liste_tmp2 -> data;

	if ( echeance -> categorie == categ -> no_categ && echeance -> sous_categorie == sous_categ -> no_sous_categ )
	{
	    echeance_trouvee = 1;
	    liste_tmp2 = NULL;
	}
	else
	    liste_tmp2 = liste_tmp2 -> next;
    }

    if ( ope_trouvee || echeance_trouvee )
    {
	GtkWidget *dialog, *hbox, *bouton_categ_generique, *combofix, *bouton_transfert;
	GSList *liste_combofix, *pointeur, *liste_tmp, *liste_categ_credit, *liste_categ_debit;
	gint i, resultat, nouveau_no_categ, nouveau_no_sous_categ;
	struct struct_categ *nouvelle_categ;
	struct struct_sous_categ *nouvelle_sous_categ;
	gchar **split_categ;

	dialog = dialogue_special_no_run ( GTK_MESSAGE_WARNING, GTK_BUTTONS_OK_CANCEL,
					   make_hint ( _("Selected sub-category still contains transactions."),
						       _("If you want to remove this sub-category but want to keep transactions, you can transfer them to another (sub-)category.  Otherwise, transactions can be simply deleted along with their category.") ));

	/*       mise en place du choix tranfert vers un autre categ */

	hbox = gtk_hbox_new ( FALSE,
			      5 );
	gtk_box_pack_start ( GTK_BOX ( GTK_DIALOG ( dialog ) -> vbox ),
			     hbox,
			     FALSE,
			     FALSE,
			     0 );

	bouton_transfert = gtk_radio_button_new_with_label ( NULL,
							     COLON(_("Transfer transactions to category"))  );
	gtk_box_pack_start ( GTK_BOX ( hbox ),
			     bouton_transfert,
			     FALSE,
			     FALSE,
			     0 );

	pointeur = liste_struct_categories;
	liste_combofix = NULL;
	liste_categ_credit = NULL;
	liste_categ_debit = NULL;

	while ( pointeur )
	{
	    struct struct_categ *categorie;
	    GSList *sous_pointeur;



	    categorie = pointeur -> data;

	    if ( categorie -> type_categ )
		liste_categ_debit = g_slist_append ( liste_categ_debit,
						     g_strdup ( categorie -> nom_categ ) );
	    else
		liste_categ_credit = g_slist_append ( liste_categ_credit,
						      g_strdup ( categorie -> nom_categ ) );


	    sous_pointeur = categorie -> liste_sous_categ;

	    while ( sous_pointeur )
	    {
		struct struct_sous_categ *sous_categorie;

		sous_categorie = sous_pointeur -> data;



		if ( sous_categorie -> no_sous_categ !=  sous_categ -> no_sous_categ )
		{
		    if ( categorie -> type_categ )
			liste_categ_debit = g_slist_append ( liste_categ_debit,
							     g_strconcat ( "\t",
									   sous_categorie -> nom_sous_categ,
									   NULL ) );
		    else
			liste_categ_credit = g_slist_append ( liste_categ_credit,
							      g_strconcat ( "\t",
									    sous_categorie -> nom_sous_categ,
									    NULL ) );
		}
		sous_pointeur = sous_pointeur -> next;
	    }
	    pointeur = pointeur -> next;
	}

	/*   on ajoute les listes des crÃ©dits / débits à la liste du combofix du formulaire */

	liste_combofix = g_slist_append ( liste_combofix,
					  liste_categ_debit );
	liste_combofix = g_slist_append ( liste_combofix,
					  liste_categ_credit );


	combofix = gtk_combofix_new_complex ( liste_combofix,
					      TRUE,
					      TRUE,
					      TRUE,
					      0 );

	gtk_box_pack_start ( GTK_BOX ( hbox ),
			     combofix,
			     TRUE,
			     TRUE,
			     0 );

	/*       mise en place du choix supprimer le categ */

	hbox = gtk_hbox_new ( FALSE,
			      5 );
	gtk_box_pack_start ( GTK_BOX ( GTK_DIALOG ( dialog ) -> vbox ),
			     hbox,
			     FALSE,
			     FALSE,
			     0 );

	bouton_categ_generique = gtk_radio_button_new_with_label ( gtk_radio_button_group ( GTK_RADIO_BUTTON ( bouton_transfert )),
								   PRESPACIFY(_("Just remove this sub-category.")) );
	gtk_box_pack_start ( GTK_BOX ( hbox ),
			     bouton_categ_generique,
			     FALSE,
			     FALSE,
			     0 );

	gtk_widget_show_all ( dialog );


retour_dialogue:
	resultat = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

	if ( resultat != GTK_RESPONSE_OK )
	{
	    gtk_widget_destroy ( GTK_WIDGET ( dialog ) );
	    return;
	}

	nouveau_no_categ = 0;
	nouveau_no_sous_categ = 0;

	if ( gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_transfert )) )
	{

	    if ( !strlen (gtk_combofix_get_text ( GTK_COMBOFIX ( combofix ))))
	    {
		dialogue_warning_hint ( _("It is compulsory to specify a destination category to move transactions but no category was entered."),
				      _("Please enter a category!"));

		goto retour_dialogue;
	    }

	    /* récupère les no de categ et sous categ */

	    split_categ = g_strsplit ( gtk_combofix_get_text ( GTK_COMBOFIX ( combofix )),
				       " : ",
				       2 );

	    nouvelle_categ = categ_par_nom ( split_categ[0],
					     0,
					     0,
					     0 );

	    if ( nouvelle_categ )
	    {
		nouveau_no_categ = nouvelle_categ -> no_categ;

		nouvelle_sous_categ =  sous_categ_par_nom ( nouvelle_categ,
							    split_categ[1],
							    0 );

		if ( nouvelle_sous_categ )
		    nouveau_no_sous_categ = nouvelle_sous_categ -> no_sous_categ;
	    }

	    g_strfreev ( split_categ );
	}
	else
	{
	    nouveau_no_categ = 0;
	    nouveau_no_sous_categ = 0;
	}


	/* on fait le tour des opés pour mettre le nouveau numéro de categ et sous_categ */

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

	for ( i = 0 ; i < nb_comptes ; i++ )
	{
	    liste_tmp = LISTE_OPERATIONS;

	    while ( liste_tmp )
	    {
		struct structure_operation *operation;

		operation = liste_tmp -> data;

		if ( operation -> categorie == categ -> no_categ
		     &&
		     operation -> sous_categorie == sous_categ -> no_sous_categ )
		{
		    operation -> categorie = nouveau_no_categ;
		    operation -> sous_categorie = nouveau_no_sous_categ;
		}

		liste_tmp = liste_tmp -> next;
	    }

	    p_tab_nom_de_compte_variable++;
	}

	/* fait le tour des échéances pour mettre le nouveau numéro de categ et sous_categ  */

	liste_tmp = liste_struct_echeances;

	while ( liste_tmp )
	{
	    struct operation_echeance *echeance;

	    echeance = liste_tmp -> data;

	    if ( echeance -> categorie == categ -> no_categ
		 &&
		 echeance -> sous_categorie == sous_categ -> no_sous_categ )
	    {
		echeance -> categorie = nouveau_no_categ;
		echeance -> sous_categorie = nouveau_no_sous_categ;

		if ( !echeance -> categorie )
		    echeance -> compte_virement = -1;
	    }

	    liste_tmp = liste_tmp -> next;
	}

	modification_fichier(TRUE);

	demande_mise_a_jour_tous_comptes ();

	gtk_widget_destroy ( GTK_WIDGET ( dialog ) );

    }


    /* supprime dans la liste des categ  */

    categ -> liste_sous_categ = g_slist_remove ( categ -> liste_sous_categ,
						 sous_categ );


    enleve_selection_ligne_categ();

    if ( mise_a_jour_combofix_categ_necessaire )
	mise_a_jour_combofix_categ  ();

    remplit_arbre_categ ();

    modification_fichier(TRUE);
}
/* **************************************************************************************************** */




/***********************************************************************************************************/
/* Routine creation_liste_categories */
/* appelée lors d'un nouveau fichier */
/* crée la liste des catégories à partir de la liste ci dessus */
/* en fait, merge la liste de base avec rien */
/***********************************************************************************************************/

void creation_liste_categories ( void )
{

    liste_struct_categories = NULL;
    nb_enregistrements_categories = 0;
    no_derniere_categorie = 0;

    merge_liste_categories ();
}
/***********************************************************************************************************/





/***********************************************************************************************************/
/* Routine merge_liste_categories */
/*   merge la liste de categories existante ( aucune quand vient de creation_liste_categories ) */
/* avec la liste de base du début du fichier */
/***********************************************************************************************************/

void merge_liste_categories ( void )
{
    gint i;
    gchar **categ;
    gint debit;
    struct struct_categ *categorie;

    debit = 0;

    /* récupération des crédits puis des débits*/

retour_recuperation :

    if ( debit )
	categ = categories_de_base_debit;
    else
	categ = categories_de_base_credit;

    i = 0;

    while ( categ[i] )
    {
	gchar **split_categ;
	struct struct_sous_categ *sous_categ;

	split_categ = g_strsplit ( categ[i],
				   " : ",
				   2 );

	categorie = categ_par_nom( split_categ[0],
				   1,
				   debit,
				   0 );

	sous_categ = sous_categ_par_nom ( categorie,
					  split_categ[1],
					  1 );

	/* libère le tableau créé */

	g_strfreev ( split_categ );

	i++;
    }

    /*       si on a fait les crédits, on fait les débits */

    if ( !debit )
    {
	debit = 1;
	goto retour_recuperation;
    }

}
/***********************************************************************************************************/


/***********************************************************************************************************/
/*  Routine qui crée la liste des catégories pour les combofix du formulaire et de la ventilation */
/* c'est à dire 3 listes dans 1 liste : */
/* la première contient les catégories de débit */
/* la seconde contient les catégories de crédit */
/* la troisième contient les catégories spéciales ( virement, retrait, ventilation ) */
/* la ventilation n'apparait pas dans les ventilations */
/***********************************************************************************************************/

void creation_liste_categ_combofix ( void )
{
    GSList *pointeur;
    GSList *liste_categ_credit;
    GSList *liste_categ_debit;
    GSList *liste_categ_special;
    gint i;

    if ( DEBUG )
	printf ( "creation_liste_categ_combofix\n" );

    liste_categories_combofix = NULL;
    liste_categories_ventilation_combofix = NULL;
    liste_categ_credit = NULL;
    liste_categ_debit = NULL;


    pointeur = liste_struct_categories;

    while ( pointeur )
    {
	struct struct_categ *categorie;
	GSList *sous_pointeur;

	categorie = pointeur -> data;

	if ( categorie -> type_categ )
	    liste_categ_debit = g_slist_append ( liste_categ_debit,
						 g_strdup ( categorie -> nom_categ ) );
	else
	    liste_categ_credit = g_slist_append ( liste_categ_credit,
						  g_strdup ( categorie -> nom_categ ) );


	sous_pointeur = categorie -> liste_sous_categ;

	while ( sous_pointeur )
	{
	    struct struct_sous_categ *sous_categ;

	    sous_categ = sous_pointeur -> data;

	    if ( categorie -> type_categ )
		liste_categ_debit = g_slist_append ( liste_categ_debit,
						     g_strconcat ( "\t",
								   sous_categ -> nom_sous_categ,
								   NULL ) );
	    else
		liste_categ_credit = g_slist_append ( liste_categ_credit,
						      g_strconcat ( "\t",
								    sous_categ -> nom_sous_categ,
								    NULL ) );

	    sous_pointeur = sous_pointeur -> next;
	}
	pointeur = pointeur -> next;
    }


    /*   on ajoute les listes des crédits / débits à la liste du combofix du formulaire */

    liste_categories_combofix = g_slist_append ( liste_categories_combofix,
						 liste_categ_debit );
    liste_categories_combofix = g_slist_append ( liste_categories_combofix,
						 liste_categ_credit );




    /*   on ajoute les listes des crédits / débits à la liste du combofix des  échéances  */

    liste_categories_ventilation_combofix = g_slist_append ( liste_categories_ventilation_combofix,
							     liste_categ_debit );
    liste_categories_ventilation_combofix = g_slist_append ( liste_categories_ventilation_combofix,
							     liste_categ_credit );


    /* création des catégories spéciales : les virements et la ventilation pour le formulaire */

    liste_categ_special = NULL;

    liste_categ_special = g_slist_append ( liste_categ_special,
					   _("Breakdown of transaction") );

    liste_categ_special = g_slist_append ( liste_categ_special,
					   _("Transfer") );

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

    for ( i = 0 ; i < nb_comptes ; i++ )
    {
	if ( ! COMPTE_CLOTURE )
	    liste_categ_special = g_slist_append ( liste_categ_special,
						   g_strconcat ( "\t",
								 NOM_DU_COMPTE ,
								 NULL ));
	p_tab_nom_de_compte_variable++;
    }

    liste_categories_combofix = g_slist_append ( liste_categories_combofix,
						 liste_categ_special );

    /* on saute le texte Opération ventilée */

    liste_categ_special = liste_categ_special -> next;

    liste_categories_ventilation_combofix = g_slist_append ( liste_categories_ventilation_combofix,
							     liste_categ_special );

}
/***********************************************************************************************************/




/***********************************************************************************************************/
/* Fonction mise_a_jour_combofix_categ */
/* recrée les listes de catégories des combofix */
/* et remet les combofix à jour */
/***********************************************************************************************************/

void mise_a_jour_combofix_categ ( void )
{
    if ( DEBUG )
	printf ( "mise_a_jour_combofix_categ\n" );


    creation_liste_categ_combofix ();

    if ( verifie_element_formulaire_existe ( TRANSACTION_FORM_CATEGORY )
	 &&
	 GTK_IS_COMBOFIX ( widget_formulaire_par_element (TRANSACTION_FORM_CATEGORY)))
	gtk_combofix_set_list ( GTK_COMBOFIX ( widget_formulaire_par_element (TRANSACTION_FORM_CATEGORY) ),
				liste_categories_combofix,
				TRUE,
				TRUE );

    if ( GTK_IS_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] ))
	gtk_combofix_set_list ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] ),
				liste_categories_combofix,
				TRUE,
				TRUE );

    if ( GTK_IS_COMBOFIX ( widget_formulaire_echeancier[TRANSACTION_BREAKDOWN_FORM_CATEGORY] ))
	gtk_combofix_set_list ( GTK_COMBOFIX ( widget_formulaire_ventilation[TRANSACTION_BREAKDOWN_FORM_CATEGORY] ),
				liste_categories_ventilation_combofix,
				TRUE,
				TRUE );

    if ( GTK_IS_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_BREAKDOWN_FORM_CATEGORY] ))
	gtk_combofix_set_list ( GTK_COMBOFIX ( widget_formulaire_ventilation_echeances[SCHEDULER_BREAKDOWN_FORM_CATEGORY] ),
				liste_categories_ventilation_combofix,
				TRUE,
				TRUE );

    /* FIXME : ça ne devrait pas se trouver dans cette fonction */

    if ( etat_courant )
    {
	remplissage_liste_categ_etats ();
	selectionne_liste_categ_etat_courant ();
    }

    mise_a_jour_combofix_categ_necessaire = 0;
    modif_categ = 1;
}
/***********************************************************************************************************/






/* **************************************************************************************************** */
/* crée un tableau de categ et de sous categ aussi gds que le nb de tiers */
/* et les renvoie dans un tab de 2 pointeurs */
/* **************************************************************************************************** */

void calcule_total_montant_categ ( void )
{
    gint i;

    /* on crée les tableaux de montant */

    /* le +1 pour reserver le [0] pour aucune catégorie */

    tab_montant_categ = calloc ( nb_enregistrements_categories + 1,
				 sizeof ( gfloat ));
    nb_ecritures_par_categ = calloc ( nb_enregistrements_categories + 1,
				      sizeof ( gint ));

    tab_montant_sous_categ = calloc ( nb_enregistrements_categories,
				      sizeof ( gpointer ));
    nb_ecritures_par_sous_categ = calloc ( nb_enregistrements_categories,
					   sizeof ( gpointer ));

    for ( i=0 ; i<nb_comptes ; i++ )
    {
	GSList *liste_tmp;

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;

	liste_tmp = LISTE_OPERATIONS;

	while ( liste_tmp )
	{
	    struct structure_operation *operation;
	    gdouble montant;
	    gint place_categ;


	    operation = liste_tmp -> data;

	    /* on commence par calculer le montant dans la devise choisie dans les paramètres */

	    montant = calcule_montant_devise_renvoi ( operation -> montant,
						      no_devise_totaux_tiers,
						      operation -> devise,
						      operation -> une_devise_compte_egale_x_devise_ope,
						      operation -> taux_change,
						      operation -> frais_change );

	    /* on traite ensuite l'opération */

	    if ( operation -> categorie )
	    {
		struct struct_categ *categorie;

		/* il y a une catégorie */

		categorie = categ_par_no ( operation -> categorie );

		/* recherche la place du tiers dans la liste */

		place_categ = g_slist_index ( liste_struct_categories,
					      categorie );

		/* crée la place pour les sous catég de cette categ si ce n'est déjà fait */

		if ( !tab_montant_sous_categ[place_categ] )
		{
		    gint nb_sous_categ;

		    nb_sous_categ = g_slist_length ( categorie -> liste_sous_categ );

		    /* on réserve nb_sous_categ + 1 pour aucune sous categ qui sera en [0] */

		    tab_montant_sous_categ[place_categ] = calloc ( nb_sous_categ + 1,
								   sizeof ( float ));
		    nb_ecritures_par_sous_categ[place_categ] = calloc ( nb_sous_categ + 1,
									sizeof ( gint ));
		}

		tab_montant_categ[place_categ+1] = tab_montant_categ[place_categ+1] + montant;
		nb_ecritures_par_categ[place_categ+1]++;

		/* on ajoute maintenant le montant à la sous categ si elle existe */

		if ( operation -> sous_categorie )
		{
		    gint place_sous_categ;

		    place_sous_categ = g_slist_index ( categorie -> liste_sous_categ,
						       sous_categ_par_no ( operation -> categorie,
									   operation -> sous_categorie ));

		    tab_montant_sous_categ[place_categ][place_sous_categ+1] = tab_montant_sous_categ[place_categ][place_sous_categ+1] + montant;
		    nb_ecritures_par_sous_categ[place_categ][place_sous_categ+1]++;
		}
		else
		{
		    if ( tab_montant_sous_categ[place_categ] )
		    {
			tab_montant_sous_categ[place_categ][0] = tab_montant_sous_categ[place_categ][0] + montant;
			nb_ecritures_par_sous_categ[place_categ][0]++;
		    }
		}
	    }
	    else
		/* il n'y a pas de catégorie */
		/* on met le montant dans tab_montant_categ[0} si e n'est ni un virement ni une ventil */
		if ( !operation -> relation_no_operation
		     &&
		     !operation -> operation_ventilee )
		{
		    tab_montant_categ[0] = tab_montant_categ[0] + montant;
		    nb_ecritures_par_categ[0]++;
		}
	    liste_tmp = liste_tmp -> next;
    }
}
}
/* **************************************************************************************************** */




/* **************************************************************************************************** */
gchar *calcule_total_montant_categ_par_compte ( gint categ, gint sous_categ, gint no_compte )
{
    gdouble retour_int;
    GSList *liste_tmp;

    retour_int = 0;
    nb_ecritures_par_comptes = 0;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + no_compte;

    liste_tmp = LISTE_OPERATIONS;

    while ( liste_tmp )
    {
	struct structure_operation *operation;

	operation = liste_tmp -> data;

	if ( operation -> categorie == categ
	     &&
	     operation -> sous_categorie == sous_categ
	     &&
	     !operation -> relation_no_operation
	     &&
	     !operation -> operation_ventilee   )
	{
	    gdouble montant;

	    montant = calcule_montant_devise_renvoi ( operation -> montant,
						      no_devise_totaux_tiers,
						      operation -> devise,
						      operation -> une_devise_compte_egale_x_devise_ope,
						      operation -> taux_change,
						      operation -> frais_change );

	    retour_int = retour_int + montant;
	    nb_ecritures_par_comptes++;
	}
	liste_tmp = liste_tmp -> next;
    }

    if ( retour_int )
	return ( g_strdup_printf ( _("%4.2f %s"),
				   retour_int,
				   devise_code_by_no ( no_devise_totaux_tiers ) ));
    else
	return ( NULL );
}
/* **************************************************************************************************** */


/* **************************************************************************************************** */
void appui_sur_ajout_categorie ( void )
{
    gchar *nom_categorie;
    struct struct_categ *nouvelle_categorie;
    gchar *text[4];
    GtkCTreeNode *ligne;

    if ( !( nom_categorie = demande_texte ( _("New category"),
					    COLON(_("Enter name for new category")) )))
	return;


    /* On vérifie si l'opération existe. */
    if ( categ_par_nom ( nom_categorie, 0, 0, 0 ))
    {
	dialogue_warning_hint ( _("Category must be both unique and not empty.  Please use another name for this category."),
				g_strdup_printf ( _("Category '%s' already exists."),
						  nom_categorie ) );
	return;
    }

    /* on l'ajoute à la liste des opés */

    nouvelle_categorie = categ_par_nom ( nom_categorie,
					 1,
					 0,
					 0 );


    /* on l'ajoute directement au ctree et on fait le tri pour éviter de toute la réafficher */

    text[0] = nouvelle_categorie -> nom_categ;
    text[1] = NULL;
    text[2] = NULL;
    text[3] = NULL;

    ligne = gtk_ctree_insert_node ( GTK_CTREE ( arbre_categ ),
				    NULL,
				    NULL,
				    text,
				    10,
				    pixmap_ferme,
				    masque_ferme,
				    pixmap_ouvre,
				    masque_ouvre,
				    FALSE,
				    FALSE );

    /* on associe à ce categorie à l'adr de sa struct */

    gtk_ctree_node_set_row_data ( GTK_CTREE ( arbre_categ ),
				  ligne,
				  nouvelle_categorie );

    gtk_ctree_sort_recursive ( GTK_CTREE ( arbre_categ ),
			       NULL );

    if ( mise_a_jour_combofix_categ_necessaire )
	mise_a_jour_combofix_categ();
    modif_categ = 0;
    modification_fichier(TRUE);
}
/* **************************************************************************************************** */



/* **************************************************************************************************** */
void appui_sur_ajout_sous_categorie ( void )
{
    gchar *nom_sous_categorie;
    struct struct_sous_categ *nouvelle_sous_categorie;
    struct struct_categ *categorie;
    gchar *text[4];
    GtkCTreeNode *ligne;
    GtkCTreeNode *node_parent;

    if ( !( nom_sous_categorie = demande_texte ( _("New sub-category"),
						 COLON(_("Enter name for new sub-category")) )))
	return;

    /* récupère le node parent */

    node_parent = GTK_CLIST ( arbre_categ ) -> selection -> data;

    while ( GTK_CTREE_ROW ( node_parent ) -> level != 1 )
	node_parent = GTK_CTREE_ROW ( node_parent ) -> parent;

    /* on récupère l'categorie parente */

    categorie = gtk_ctree_node_get_row_data ( GTK_CTREE ( arbre_categ ),
					      node_parent );
    if ( categorie <= 0 )
	return;

    /* On vérifie si l'opération existe. */
    if ( sous_categ_par_nom ( categorie, nom_sous_categorie, 0 ))
    {
	dialogue_warning_hint ( _("Sub-category must be both unique and not empty.  Please use another name for this sub-category."),
				g_strdup_printf ( _("Sub-category '%s' already exists."),
						  nom_sous_categorie ) );
	return;
    }

    /* on l'ajoute à la liste des opés */

    nouvelle_sous_categorie = sous_categ_par_nom ( categorie,
						   nom_sous_categorie,
						   1 );

    /* on l'ajoute directement au ctree et on fait le tri pour éviter de toute la réafficher */

    text[0] = nouvelle_sous_categorie -> nom_sous_categ;
    text[1] = NULL;
    text[2] = NULL;
    text[3] = NULL;

    ligne = gtk_ctree_insert_node ( GTK_CTREE ( arbre_categ ),
				    node_parent,
				    NULL,
				    text,
				    10,
				    NULL,
				    NULL,
				    NULL,
				    NULL,
				    FALSE,
				    FALSE );

    /* on associe à ce categorie à l'adr de sa struct */

    gtk_ctree_node_set_row_data ( GTK_CTREE ( arbre_categ ),
				  ligne,
				  nouvelle_sous_categorie );

    gtk_ctree_sort_recursive ( GTK_CTREE ( arbre_categ ),
			       NULL );


    if ( mise_a_jour_combofix_categ_necessaire )
	mise_a_jour_combofix_categ();
    modif_categ = 0;
    modification_fichier(TRUE);
}
/* **************************************************************************************************** */



/* **************************************************************************************************** */
void exporter_categ ( void )
{
    GtkWidget *fenetre_nom;
    gint resultat;
    struct stat test_fichier;
    gchar *nom_categ;

    fenetre_nom = gtk_file_selection_new ( _("Export categories"));
    file_selection_set_filename ( GTK_FILE_SELECTION ( fenetre_nom ),
				      dernier_chemin_de_travail );
    file_selection_set_entry ( GTK_FILE_SELECTION ( fenetre_nom ),
			 ".cgsb" );

    resultat = gtk_dialog_run ( GTK_DIALOG ( fenetre_nom ));

    switch ( resultat )
    {
	case GTK_RESPONSE_OK :
	    nom_categ =file_selection_get_filename ( GTK_FILE_SELECTION ( fenetre_nom ));

	    gtk_widget_destroy ( GTK_WIDGET ( fenetre_nom ));

	    /* vérification que c'est possible */

	    if ( !strlen ( nom_categ ))
		return;

	    if ( utf8_stat ( nom_categ,
			&test_fichier ) != -1 )
	    {
		if ( S_ISREG ( test_fichier.st_mode ) )
		{
		    if ( !question_yes_no_hint ( g_strdup_printf ( _("File '%s' already exists."), nom_categ ),
						 _("Do you want to overwrite it?")))
			return;
		}
		else
		{
		    dialogue_error_hint ( g_strdup_printf ( _("File \"%s\" exists and is not a regular file."),
							    nom_categ),
					  g_strdup_printf ( _("Error saving file '%s'." ), nom_categ ) );
		    return;
		}
	    }

	    if ( !enregistre_categ ( nom_categ ))
	    {
		return;
	    }

	    break;

	default :
	    gtk_widget_destroy ( GTK_WIDGET ( fenetre_nom ));
	    return;
    }
}
/* **************************************************************************************************** */



/* **************************************************************************************************** */
void importer_categ ( void )
{
    GtkWidget *dialog, *fenetre_nom;
    gint resultat;
    gchar *nom_categ;

    fenetre_nom = gtk_file_selection_new ( _("Import a category list"));
    file_selection_set_filename ( GTK_FILE_SELECTION ( fenetre_nom ),
				      dernier_chemin_de_travail );
    file_selection_set_entry ( GTK_FILE_SELECTION ( fenetre_nom ),
			 ".cgsb" );

    resultat = gtk_dialog_run ( GTK_DIALOG ( fenetre_nom ));

    if ( resultat != GTK_RESPONSE_OK )
    {
	gtk_widget_destroy ( fenetre_nom );
	return;
    }

    nom_categ = file_selection_get_filename ( GTK_FILE_SELECTION ( fenetre_nom ));

    gtk_widget_destroy ( GTK_WIDGET (fenetre_nom  ));

    /* vérification que c'est possible */

    if ( !strlen ( nom_categ ))
	return;

    /* on permet de remplacer/fusionner la liste */

    dialog = dialogue_special_no_run ( GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE,
				       make_hint ( _("Merge category list with existing?"),
						   ( no_derniere_operation ?
						     _("Imported category list will be merged with existing.  There is no undo for this.") :
						     _("Imported category list can either be merged with existing or replace it.  Once performed, there is no undo for this.") ) ) );

    if ( !no_derniere_operation )
      gtk_dialog_add_buttons ( GTK_DIALOG(dialog),
			       _("Replace existing"), 0,
			       NULL );

    gtk_dialog_add_buttons ( GTK_DIALOG(dialog),
			     GTK_STOCK_CANCEL, 1,
			     _("Merge categories"), 2,
			     NULL );

    resultat = gtk_dialog_run ( GTK_DIALOG ( dialog ));

    switch ( resultat )
    {
	case 0 :
	    /* si on a choisi de remplacer l'ancienne liste, on la vire ici */

	    if ( !no_derniere_operation )
	    {
		g_slist_free ( liste_struct_categories );
		liste_struct_categories = NULL;
		no_derniere_categorie = 0;
		nb_enregistrements_categories = 0;
	    }

        case 2 :
	    if ( !charge_categ ( nom_categ ))
	    {
		gtk_widget_destroy ( GTK_WIDGET ( dialog ));
		return;
	    }
	    gtk_widget_destroy ( GTK_WIDGET ( dialog ));
	    break;

	default :
	    gtk_widget_destroy ( GTK_WIDGET ( dialog ));
	    return;
    }
}
/* **************************************************************************************************** */


/* Local Variables: */
/* c-basic-offset: 4 */
/* End: */
