#include "Avion.h"

/// Initialisation d'Allegro
void initiAllegro()
{
    allegro_init();             // lancement d'Allegro

    set_color_depth(desktop_color_depth());                         // initialisation du mode graphique
    if((set_gfx_mode(GFX_AUTODETECT_WINDOWED,1200,650,0,0))!=0)     // et de la taille de l'écran
    {
        allegro_message("Pb de mode graphique") ;
        allegro_exit();
        exit(EXIT_FAILURE);
    }

    install_keyboard();         // installation du clavier
    install_mouse();            // et de la souris

    show_mouse(screen);         // affichage de la souris sur l'écran graphique Allegro
}
