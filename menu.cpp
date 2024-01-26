#include "Monde.h"

///Programme d'affichage de l'écran d'accueil du jeu
int ecran_accueil()
{
    // déclaration des variables
    BITMAP *bienvenu = NULL, *bienvenu2=NULL, *buffer = NULL;
    int play = 0;

    // initialisation du double buffer
    buffer = create_bitmap(1200,650);
    clear_bitmap(buffer);

    // initialisation des fonds d'écran
    bienvenu = load_bitmap("bmp/bienvenue.bmp",NULL);
    bienvenu2 = load_bitmap("bmp/bienvenue2.bmp", NULL);

    blit(buffer, screen, 0, 0, 0, 0, 1200, 650);
    draw_sprite(screen, buffer, 0,0);

    while(play!=1)
    {
        clear_bitmap(buffer);

        /// play
        if((mouse_x > 434) && (mouse_x <765) && (mouse_y > 568) && (mouse_y <632))
        {
            blit(bienvenu2, buffer, 0, 0, 0, 0, bienvenu2->w, bienvenu2->h);
            draw_sprite(screen, buffer, 0,0);

            if(mouse_b&1)
                play=1;
        }
        else
        {
            blit(bienvenu, buffer, 0, 0, 0, 0, bienvenu->w, bienvenu->h);
            draw_sprite(screen, buffer, 0,0);
        }
    }

    // destruction des BITMAP pour libérer la mémoire allouée
    destroy_bitmap(buffer);
    destroy_bitmap(bienvenu2);
    destroy_bitmap(bienvenu);

    return play;
}

int affichage_menu()
{
    BITMAP *buffer = NULL, *menu = NULL, *simu_alea = NULL, *trajet_court = NULL, *quitter = NULL;
    int choix = 0;

    // création du double buffer
    buffer = create_bitmap(1200, 650);
    clear_bitmap(buffer);

    // récupération des images
    menu = load_bitmap("bmp/menu.bmp", NULL);
    simu_alea = load_bitmap("bmp/simu.bmp", NULL);
    trajet_court = load_bitmap("bmp/trajet.bmp", NULL);
    quitter = load_bitmap("bmp/quitter.bmp", NULL);

    // copie du double buffer sur l'écran graphique
    blit(buffer, screen, 0, 0, 0, 0, 1200, 650);
    draw_sprite(screen, buffer, 0,0);


    while(choix!=1 && choix!=2 && choix!=3)
    {
        clear_bitmap(buffer);

        ///SIMULATION ALEATOIRE
        if(mouse_x>=435 && mouse_x<=765 && mouse_y>=184 && mouse_y<=249)
        {
            blit(simu_alea, buffer, 0, 0, 0, 0, simu_alea->w, simu_alea->h);
            draw_sprite(screen, buffer, 0,0);
            if(mouse_b&1)
                choix = 1;
        }
        ///TRAJET LE PLUS COURT
        else if(mouse_x>=435 && mouse_x<=765 && mouse_y>=291 && mouse_y<=356)
        {
            blit(trajet_court, buffer, 0, 0, 0, 0, trajet_court->w, trajet_court->h);
            draw_sprite(screen, buffer, 0,0);
            if(mouse_b&1)
                choix = 2;
        }
        ///QUITTER
        else if(mouse_x>=435 && mouse_x<=765 && mouse_y>=409 && mouse_y<=474)
        {
            blit(quitter, buffer, 0, 0, 0, 0, quitter->w, quitter->h);
            draw_sprite(screen, buffer, 0,0);
            if(mouse_b&1)
                choix = 3;
        }
        else
        {
            blit(menu, buffer,0, 0, 0, 0, menu->w, menu->h);
            draw_sprite(screen, buffer, 0,0);
        }
    }


    // nettoyage de l'écran
    clear_bitmap(screen);

    // destruction des bitmap
    destroy_bitmap(buffer);
    destroy_bitmap(menu);
    destroy_bitmap(trajet_court);
    destroy_bitmap(quitter);
    destroy_bitmap(simu_alea);

    return choix;
}
