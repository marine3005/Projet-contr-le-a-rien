#ifndef MONDE_H
#define MONDE_H

#include "Trajet.h"

class Monde
{
private:
    std::vector<Aeroport*> m_aeroports;
    std::vector<Trajet*> m_trajets;
    std::vector<Avion*> m_avions;
    BITMAP *m_carte;

public:
    Monde(std::string nomFichier);
    ~Monde();

    void afficher();

    void deplacement();

    void afficher_court_chemin ();

    std::vector <Aeroport *> Dijkstra(char depart, char arrive,char type,std::vector <float> & distance);
    std::vector <Aeroport *> Dijkstra_affichage(char depart, char arrive, char type,std::vector <float> & distance,std::map<char,char> &pred, std::map<char,float> &couleurs );
    std::map<std::pair<float, float>, std::pair<float, float>> initialisation_equivalence_Astar_pixels();
    void Welsh_et_Powell();
};

#endif // MONDE_H
