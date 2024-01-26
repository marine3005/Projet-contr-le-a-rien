#ifndef GRAPHE_AS_H
#define GRAPHE_AS_H

#include"Coordonnee.h"

class Graphe_AS
{
private:
    std::vector<Coordonnee*> m_coordonnees;

public:
    Graphe_AS();
    ~Graphe_AS();

    void Zone_turbulence(int num_zone);
    std::vector<Coordonnee*> Astar(float x_dep,float y_dep,float x_arrivee1,float y_arrivee1);

    Coordonnee* Astar_pixels(Coordonnee* debut);
    Coordonnee* pixels_Astar(float x, float y);
};

#endif // GRAPHE_AS_H
