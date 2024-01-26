#include "Monde.h"

/* PROJET REALISE EN INTEGRALITE PAR L'EQUIPE COMPOSE
DE CHARLOTTE BOCAHUT, MARINE CANNEVA, CHARLES COATMELLEC ET AGATHE VIGNANCOUR */


// sous-programmes extérieurs aux classes
void initiAllegro();
int affichage_menu();
int ecran_accueil();
void menu(int choix, Monde terre);


int main()
{
    initiAllegro();

    /// Récupération des informations dans les fichiers textes
    Monde terre("infos/Monde.txt");
    int debuter=0, choix;

    debuter=ecran_accueil();

    //terre.afficher();

    ///Affichage du menu avec toutes les options + choix
    while(debuter==1 && choix!=3)
    {
        choix = affichage_menu();
        if(choix==1)
        {
            terre.deplacement();
        }
        if(choix==2)
        {
            terre.afficher_court_chemin();
        }
    }
    //terre.afficher();

    // on quitte Allegro
    allegro_exit();

    return 0;
}
END_OF_MAIN()
