#include "Graphe_AS.h"

/// Constructeur
Graphe_AS::Graphe_AS()
{
    std::vector<std::pair < Coordonnee*,float>> m_inter_successeurs;

    for (float i=0; i<40; i++)
    {
        for(float j=0; j<22; j++)
        {
            m_coordonnees.push_back(new Coordonnee{i,j});
        }
    }

    for(int z=0; z<880; z++)
    {
        for(const auto& it: m_coordonnees)
        {
            //Case du haut
            if ((m_coordonnees[z]->getX()+1) == it->getX() && m_coordonnees[z]->getY()==it->getY())
            {
                m_coordonnees[z]->ajouterSucc(it,1);
            }
            //Case du bas
            if ((m_coordonnees[z]->getX()-1) == it->getX() && m_coordonnees[z]->getY()==it->getY())
            {
                m_coordonnees[z]->ajouterSucc(it,1);
            }
            //Case de droite
            if (m_coordonnees[z]->getX() == it->getX() && (m_coordonnees[z]->getY()+1)==it->getY())
            {
                m_coordonnees[z]->ajouterSucc(it,1);
            }
            //Case de gauche
            if (m_coordonnees[z]->getX() == it->getX() && (m_coordonnees[z]->getY()-1)==it->getY())
            {
                m_coordonnees[z]->ajouterSucc(it,1);
            }
            // Diagonale haut droite
            if ((m_coordonnees[z]->getX()+1) == it->getX() && (m_coordonnees[z]->getY()+1)==it->getY())
            {
                m_coordonnees[z]->ajouterSucc(it,1.4);
            }
            //Diagonale haut gauche
            if ((m_coordonnees[z]->getX()-1) == it->getX() && (m_coordonnees[z]->getY()+1)==it->getY())
            {
                m_coordonnees[z]->ajouterSucc(it,1.4);
            }
            //Diagonale bas gauche
            if ((m_coordonnees[z]->getX()-1) == it->getX() && (m_coordonnees[z]->getY()-1)==it->getY())
            {
                m_coordonnees[z]->ajouterSucc(it,1.4);
            }
            //Diagonale bas droite
            if ((m_coordonnees[z]->getX()+1) == it->getX() && (m_coordonnees[z]->getY()-1)==it->getY())
            {
                m_coordonnees[z]->ajouterSucc(it,1.4);
            }
        }
    }
}

/// Destructeur
Graphe_AS::~Graphe_AS()
{
    //dtor
}

/// Méthodes de conversion
Coordonnee* Graphe_AS::Astar_pixels(Coordonnee* debut)
{
    float x, y;

    x = (debut->getX()*1200)/40;
    y = (debut->getY()*650)/22;

    return (new Coordonnee{x, y});
}

Coordonnee* Graphe_AS::pixels_Astar(float x, float y)
{
    float x_inter = (x*40)/1200;
    float y_inter = (y*22)/650;

    return (new Coordonnee{x_inter, y_inter});
}

/// Algorithme A*
std::vector<Coordonnee*> Graphe_AS::Astar(float x_dep, float y_dep, float x_arrivee1, float y_arrivee1)
{
    //std::vector <Aeroport *> parcours_avion_inter;
    //std::vector <Aeroport *> parcours_avion;
    std::map<Coordonnee*,float> dists;
    std::map<Coordonnee*, Coordonnee *> pred ;
    std::map<Coordonnee*,float> couleurs ;
    std::pair<Coordonnee*,float>  p;
    Coordonnee * coordonnee_dep;
    Coordonnee * coordonnee_arrivee;
    std::vector <Coordonnee *> parcours_avion;
    std::vector <Coordonnee *> parcours_avion_inter;

    int x_avion, y_avion;
    Coordonnee* inter = pixels_Astar(x_dep, y_dep);
    x_avion = inter->getX();
    y_avion = inter->getY();

    int x_arrivee, y_arrivee;
    Coordonnee* inter1 = pixels_Astar(x_arrivee1, y_arrivee1);
    x_arrivee = inter1->getX();
    y_arrivee = inter1->getY();

    //m_sucesseur = m_distance aeroport
    ///définition de la fonction de comparaison cmp
    auto cmp = [](std::pair< Coordonnee*,float> p1, std::pair< Coordonnee*,float> p2)
    {
        return p2.second<p1.second;
    };

    /// déclaration de la file de priorité

    std::priority_queue<std::pair< Coordonnee*,float>,std::vector<std::pair< Coordonnee*,float>>,decltype(cmp)> file(cmp);

    std::cout<<"hello1"<<std::endl;

    ///Initialisation de map distance qui associe un aeroport a une distance, de map couleur qui permet de savoir si un aeroport est parcouru, map predecesseur qui permet de savoir la coordonnée precedente
    for (size_t i=0; i<m_coordonnees.size(); i++)
    {
        dists.insert({m_coordonnees[i],-1});
        couleurs.insert({m_coordonnees[i],0});
        pred.insert({m_coordonnees[i],0});
    }

    std::cout<<"hello2"<<std::endl;

    ///On cherche le sommet correspondant aux coordonnées d'arrivée

    for(const auto& it: m_coordonnees)
    {
        if (it->getX() == x_arrivee && it->getY() == y_arrivee)
        {
            coordonnee_arrivee=it;
        }
    }

    std::cout<<"hello3"<<std::endl;

    ///On rentre coordonnee depart djikstra
    for(const auto& it: m_coordonnees)
    {
        std::cout<<"Coordonnee avion"<< x_avion<< "  "<< y_avion << std::endl;
        std::cout<<"Coordonnee recherche"<< it->getX() << "  "<< it->getY() << std::endl;
        if (it->getX() == x_avion && it->getY() == y_avion)
        {

            coordonnee_dep=it;
            ///On insere le sommet et sa distance au sommet de depart dans la file

            auto it2 = dists.find(it);
            it2->second=0;
            ///Calcul de l'heuristique du sommet de depart
            int heuristique = sqrt((it->getX()- x_arrivee)*(it->getX() - x_arrivee)+((it->getX()- x_arrivee)*(it->getX()- x_arrivee)));
            file.push({it,heuristique});
        }
    }

    std::cout<<"hello4"<<std::endl;

    //La condition de sortie est que la file soit vide
    while(p.first!=coordonnee_arrivee)
    {
        std::cout<<"BJ0"<<std::endl;
        if (file.empty())
        {
            std::cout<<"file vide"<<std::endl;
        }
        else
        {
            std::cout<<"file non vide"<<std::endl;
        }
        p=file.top();
        std::cout<<"BJ1"<<std::endl;
        //Certains sommets ont déja étaient marqués car on a utilisé un chemin plus court, on les enlève donc de la file
        auto it3 = couleurs.find(p.first);
        if(it3->second==1)
        {
            std::cout<<"BJ2"<<std::endl;
            file.pop();
        }
        else
        {
            std::cout<<"BJ3"<<std::endl;
            file.pop();
            //On marque le sommet que l'on traite
            it3->second=1;

            //Pour tous les sucesseurs
            for(const auto& it: (p.first)->getSuccesseur())
            {
                /*Si ils sont non marqués et que le chemin
                qui passe par le sommet en cours de traitement est plus court*/

                //it parcourt le vecteur map
                //auto it2 = dists.find(it->getPremiereLettre())
                std::cout<<"BJ4"<<std::endl;
                //On cherche si dans la map couleur coordonnee a deja été parcouru et quelle distance on lui avait attribué
                auto it4 = couleurs.find(it.first);
                std::cout<<"BJ5"<<std::endl;
                auto it5 = dists.find(it.first);

                ///it parcourt de la map des successeur sommet actuel
                ///p.second = distance du sommet traité
                ///it.second = distance du successeur du sommet traite
                ///it5->second = distance sommet initial/ sommet successeur
                std::cout<<"BJ6"<<std::endl;

                float distance_g = p.second+it.second;
                float distance_h = sqrt((it.first->getX()- x_arrivee)*(it.first->getX()- x_arrivee)+((it.first->getY()- y_arrivee)*(it.first->getY()- y_arrivee)));

                if( it4->second == 0 && (distance_g + distance_h < it5->second || (it5->second == -1) ))
                {
                    std::cout<<"BJ7"<<std::endl;
                    //On leur attribue une distance estimée la plus courte

                    it5->second = distance_g+distance_h ;
                    //On note le prédecesseur qui permet d'arriver au sommet par le chemin le plus court
                    auto it6 = pred.find (it.first);
                    //
                    it6->second = p.first;
                    //On l'ajoute a la file
                    std::cout<<"BJ8"<<std::endl;
                    file.push({it.first,distance_g+distance_h});
                    std::cout<<"BJ9"<<std::endl;
                }
            }
        }
    }

    //std::cout<<"hello5"<<std::endl;

    ///Affichage du Dijkstra
    //std::cout<< std::endl;
    //std::cout<< "Parcours Djiska a partir de  " << depart <<std::endl;
    //char interm;

    ///On parcourt
    //std::cout<<coordonnee_arrivee->getX()<<"  "<<coordonnee_arrivee->getY();
    Coordonnee * intermed;
    intermed = coordonnee_arrivee;
    parcours_avion_inter.push_back(intermed);
    while(intermed!=coordonnee_dep)
    {
        //std::cout << "<--" ;
        auto it13 = pred.find(intermed);
        //std::cout << it13->second->getX() << "   " << it13->second->getY();
        ///it14 parcourt vecteur d'aeroport
        ///it13 nom predecesseur
        ///it15 iterateur parcours_avion

        parcours_avion_inter.push_back(it13->first);
        intermed=it13->second;
    }

    for(const auto& it16: parcours_avion_inter)
    {
        //parcours_avion_inter.back() = Astar_pixels(parcours_avion_inter.back());
        //std::cout<<"x = "<<parcours_avion_inter.back()->getX()<<" y = "<<parcours_avion_inter.back()->getY()<<std::endl;
        parcours_avion.push_back(parcours_avion_inter.back());

        parcours_avion_inter.pop_back();
    }

    //std::cout<<"hello6"<<std::endl;

    return(parcours_avion);
}

void Graphe_AS::Zone_turbulence(int num_zone)
{
    for(int z=0; z<880; z++)
    {
        ///Atlanta -Londres
        if (num_zone==0)
        {
            if ( m_coordonnees[z]->getX() == 14 && m_coordonnees[z]->getY()== 8)
            {
                m_coordonnees.erase(m_coordonnees.begin()+ z);
            }
            if ( m_coordonnees[z]->getX() == 15 && m_coordonnees[z]->getY()== 8)
            {
                m_coordonnees.erase(m_coordonnees.begin()+ z);
            }
            if ( m_coordonnees[z]->getX() == 16 && m_coordonnees[z]->getY()== 8)
            {
                m_coordonnees.erase(m_coordonnees.begin()+ z);
            }
            if ( m_coordonnees[z]->getX() == 14 && m_coordonnees[z]->getY()== 9)
            {
                m_coordonnees.erase(m_coordonnees.begin()+ z);
            }
            if ( m_coordonnees[z]->getX() == 15 && m_coordonnees[z]->getY()== 9)
            {
                m_coordonnees.erase(m_coordonnees.begin()+ z);
            }
            if ( m_coordonnees[z]->getX() == 16 && m_coordonnees[z]->getY()== 9)
            {
                m_coordonnees.erase(m_coordonnees.begin()+ z);
            }
            if ( m_coordonnees[z]->getX() == 14 && m_coordonnees[z]->getY()== 10)
            {
                m_coordonnees.erase(m_coordonnees.begin()+ z);
            }
            if ( m_coordonnees[z]->getX() == 15 && m_coordonnees[z]->getY()== 10)
            {
                m_coordonnees.erase(m_coordonnees.begin()+ z);
            }
            if ( m_coordonnees[z]->getX() == 16 && m_coordonnees[z]->getY()== 10)
            {
                m_coordonnees.erase(m_coordonnees.begin()+ z);
            }
        }
        ///Deviation  Captown-Dubai
        else if (num_zone==1)
        {

            if ( m_coordonnees[z]->getX() == 23 && m_coordonnees[z]->getY()== 13)
            {
                m_coordonnees.erase(m_coordonnees.begin()+ z);
            }
            if ( m_coordonnees[z]->getX() == 23 && m_coordonnees[z]->getY()== 14)
            {
                m_coordonnees.erase(m_coordonnees.begin()+ z);
            }
            /*if ( m_coordonnees[z]->getX() == 24 && m_coordonnees[z]->getY()== 13)
            {
                m_coordonnees.erase(m_coordonnees.begin()+ z);
            }*/
            if ( m_coordonnees[z]->getX() == 24 && m_coordonnees[z]->getY()== 14)
            {
                m_coordonnees.erase(m_coordonnees.begin()+ z);
            }
        }
        /// Deviation Melbourne

        else if (num_zone==2)
        {
            if ( m_coordonnees[z]->getX() == 33 && m_coordonnees[z]->getY()== 13)
            {
                m_coordonnees.erase(m_coordonnees.begin()+ z);
            }
            if ( m_coordonnees[z]->getX() == 34 && m_coordonnees[z]->getY()== 13)
            {
                m_coordonnees.erase(m_coordonnees.begin()+ z);
            }
            /*if ( m_coordonnees[z]->getX() == 35 && m_coordonnees[z]->getY()== 13)
            {
                m_coordonnees.erase(m_coordonnees.begin()+ z);
            }*/
            if ( m_coordonnees[z]->getX() == 33 && m_coordonnees[z]->getY()== 14)
            {
                m_coordonnees.erase(m_coordonnees.begin()+ z);
            }
            /*if ( m_coordonnees[z]->getX() == 35 && m_coordonnees[z]->getY()== 14)
            {
                m_coordonnees.erase(m_coordonnees.begin()+ z);
            }*/

        }
    }
}
