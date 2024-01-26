#include "Monde.h"

/// Constructeur
Monde::Monde(std::string nomFichier)
{
    std::string string_ligne;

    // ouverture du fichier texte
    std::ifstream ifs{nomFichier};
    if (!ifs)
        throw std::runtime_error( "Impossible d'ouvrir en lecture " + nomFichier );

    // récupération de l'ordre = nombre d'aéroports
    int ordre;
    ifs >> ordre;
    if ( ifs.fail() )
        throw std::runtime_error("Probleme lecture ordre du graphe");

    getline(ifs,string_ligne);

    // création des sommets = les aéroports + les avions dans le monde
    for (int i=0; i<ordre; ++i)
    {
        getline(ifs, string_ligne);
        m_aeroports.push_back(new Aeroport(string_ligne));

        for(int l=0; l<m_aeroports[i]->getNbStations(); l++)
        {
            m_avions.push_back(m_aeroports[i]->getStations()[l]);
        }
    }

    // récupération de la taille = nombre de trajets possibles
    int taille;
    ifs >> taille;
    if ( ifs.fail() )
        throw std::runtime_error("Probleme lecture taille du graphe");

    // récupération des données des arêtes
    char lettre1,lettre2;
    int num;
    for (int i=0; i<taille; ++i)
    {
        ifs>>lettre1>>lettre2>>num;
        if ( ifs.fail() )
            throw std::runtime_error("Probleme lecture arc");
    }

    // chargement de la carte en bitmap
    m_carte = load_bitmap("bmp/monde.bmp", NULL);
}

/// Destructeur
Monde::~Monde()
{
    // désallocation du vecteur de pointeurs sur sommets
    for (auto s : m_aeroports)
        delete s;

    // désallocation du vecteur de pointeurs sur arêtes
    for (auto s : m_trajets)
        delete s;

    // désallocation du vecteur de pointeurs sur avions
    for (auto s : m_avions)
        delete s;

}

/// Méthode d'affichage des aéroports, avions et trajets actuels
void Monde::afficher()
{
    std::cout<<"---------- AEROPORTS ----------"<<std::endl;
    for(auto it : m_aeroports)
    {
        it->afficher();
    }

    std::cout<<"----- TRAJETS ACTUELS -----"<<std::endl;
    for(auto it : m_trajets)
    {
        it->afficher();
    }

    int cpt_court=0, cpt_moyen=0, cpt_long=0;
    std::cout<<std::endl<<"---------- AVIONS ----------"<<std::endl;
    for(auto it : m_avions)
    {
        if(it->getType()=="C")
            cpt_court++;
        else if(it->getType()=="M")
            cpt_moyen++;
        else if(it->getType()=="L")
            cpt_long++;
    }

    std::cout<<"Court = "<<cpt_court<<std::endl;
    std::cout<<"Moyen = "<<cpt_moyen<<std::endl;
    std::cout<<"Long = "<<cpt_long<<std::endl<<std::endl;
}

/// Initialisation d'une map pour trouver les équivalences des pixels et carreaux entre A* et Dijkstra
std::map<std::pair<float, float>, std::pair<float, float>> Monde::initialisation_equivalence_Astar_pixels()
{
    int x=0, y=0;
    std::map<std::pair<float, float>, std::pair<float, float>> equivalence;

    // exemple : on lie la case {0,0} d'A* aux coordonnées {0,0} sur la partie graphique Allegro etc...
    for(int i=0; i<40; i++)
    {
        for(int j=0; j<22; j++)
        {
            equivalence.insert({std::make_pair(i,j),std::make_pair(x,y)});
            y=y+30;
        }
        x=x+30;
        y=0;
    }

    // on retourne la map afin de l'utiliser à chaque fois qu'on souhaite passer d'A* aux coordonnées
    return equivalence;
}

/// Deplacement des avions : partie calcul + partie graphique
void Monde::deplacement()
{
    // variables pour Dijkstra
    std::vector<Aeroport*> chemin;
    std::vector<float> nb_km;

    // variable pour A*
    std::map<std::pair<float, float>, std::pair<float, float>> equivalence = initialisation_equivalence_Astar_pixels();

    int depart_int, arrivee_int, cpt_ut=0, taille_trajet=0, cpt=0;
    char depart, arrivee, type;
    bool verif=true, verif2=true, verif3=true, suppr=false;
    bool verif_station = false, verif_piste = false, perturbation=false;
    int activation_perturbation=14, type_perturbation;


    // création des bitmaps
    BITMAP *buffer=NULL;
    BITMAP *info_avions=NULL, *graphe=NULL;
    BITMAP *nuage=NULL, *nuage2=NULL, *explosion=NULL;

    buffer = create_bitmap(1200, 650);
    clear_bitmap(buffer);

    info_avions=load_bitmap("bmp/info_avions.bmp", NULL);
    graphe=load_bitmap("bmp/graphe.bmp", NULL);

    BITMAP *atlanta=NULL, *rio=NULL, *londres=NULL, *shanghai=NULL, *bangkok=NULL, *captown=NULL, *dubai=NULL, *melbourne=NULL;

    atlanta=load_bitmap("aeroport_bmp/atlanta.bmp", NULL);
    rio=load_bitmap("aeroport_bmp/rio.bmp", NULL);
    londres=load_bitmap("aeroport_bmp/londres.bmp", NULL);
    shanghai=load_bitmap("aeroport_bmp/shanghai.bmp", NULL);
    bangkok=load_bitmap("aeroport_bmp/bangkok.bmp", NULL);
    captown=load_bitmap("aeroport_bmp/captown.bmp", NULL);
    dubai=load_bitmap("aeroport_bmp/dubai.bmp", NULL);
    melbourne=load_bitmap("aeroport_bmp/melbourne.bmp", NULL);

    nuage=load_bitmap("nuage.bmp", NULL);
    nuage2=load_bitmap("nuage2.bmp",NULL);
    explosion=load_bitmap("explosion.bmp", NULL);

    FONT *font3 = load_font("police.pcx", NULL, NULL);
    FONT *font2 = load_font("police2.pcx", NULL, NULL);

    SAMPLE *son;
    install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,"A");
    son = load_sample("son.wav");

    Graphe_AS perturb;


    /// Boucle de simulation : 1 boucle = 1 UT
    while(!key[KEY_ESC])
    {
        // lancement du compteur d'UT pour savoir combien de temps a duré notre simulation
        cpt_ut++;


        /// CREATION D UNE PERTURBATION METEOROLOGIQUE
        // toutes les 'activation_perturbation' UT
        if(cpt_ut%activation_perturbation==0)
        {
            perturbation=true;

            // choix aléatoire de la perturbation
            type_perturbation = rand()%(2+1);
            if (type_perturbation==0)
            {
                activation_perturbation=20;
            }
            else if (type_perturbation==1)
            {
                activation_perturbation=12;
            }
            else if (type_perturbation==2)
            {
                activation_perturbation=14;
            }

            // création de la perturbation dans le monde
            perturb.Zone_turbulence(type_perturbation);
        }

        // affichage de la carte sur le buffer
        blit(m_carte, buffer, 0, 0, 0, 0, 1200, 650);


        /// AFFICHER LE GRAPHE DE PONDÉRATION
        if(key[KEY_G])
        {
            blit(graphe, buffer,0,0,0,0, 1200, 650);
        }

         ///Calcul altitude de l'avion
         Welsh_et_Powell();


        /// BOUCLE DE TRAITEMENT DE CHAQUE TRAJET ACTUEL
        for(size_t i=0; i<m_trajets.size(); i++)
        {
            /// Affichage de l'avion sur le buffer
            //1/8
            if(m_trajets[i]->getArriveeX()>=m_trajets[i]->getAvionX() && m_trajets[i]->getArriveeY()<=m_trajets[i]->getAvionY()&& m_trajets[i]->getArriveeY()<m_trajets[i]->getArriveeX())
            {
                rotate_sprite(buffer, m_trajets[i]->getBmp(), m_trajets[i]->getAvionX()-15, m_trajets[i]->getAvionY()-15, 1500000);
            }
            //2/8
            else if(m_trajets[i]->getArriveeX()>=m_trajets[i]->getAvionX() && m_trajets[i]->getArriveeY()<=m_trajets[i]->getAvionY()&& m_trajets[i]->getArriveeY()>m_trajets[i]->getArriveeX())
            {
                rotate_sprite(buffer, m_trajets[i]->getBmp(), m_trajets[i]->getAvionX()-15, m_trajets[i]->getAvionY()-15, 3000000);
            }
            //3/8
            else if(m_trajets[i]->getArriveeX()>=m_trajets[i]->getAvionX() && m_trajets[i]->getArriveeY()>=m_trajets[i]->getAvionY()&& m_trajets[i]->getArriveeY()<m_trajets[i]->getArriveeX())
            {
                rotate_sprite(buffer, m_trajets[i]->getBmp(), m_trajets[i]->getAvionX()-15, m_trajets[i]->getAvionY()-15, 5000000);
            }
            //4/8
            else if(m_trajets[i]->getArriveeX()>=m_trajets[i]->getAvionX() && m_trajets[i]->getArriveeY()>=m_trajets[i]->getAvionY()&& m_trajets[i]->getArriveeY()>m_trajets[i]->getArriveeX())
            {
                rotate_sprite(buffer, m_trajets[i]->getBmp(), m_trajets[i]->getAvionX()-15, m_trajets[i]->getAvionY()-15, 7000000);
            }
            //5/8
            else if(m_trajets[i]->getArriveeX()<=m_trajets[i]->getAvionX() && m_trajets[i]->getArriveeY()>=m_trajets[i]->getAvionY()&& m_trajets[i]->getArriveeY()>m_trajets[i]->getArriveeX())
            {
                rotate_sprite(buffer, m_trajets[i]->getBmp(), m_trajets[i]->getAvionX()-15, m_trajets[i]->getAvionY()-15, 11250000);
            }
            //6/8
            else if(m_trajets[i]->getArriveeX()<=m_trajets[i]->getAvionX() && m_trajets[i]->getArriveeY()>=m_trajets[i]->getAvionY()&& m_trajets[i]->getArriveeY()<m_trajets[i]->getArriveeX())
            {
                rotate_sprite(buffer, m_trajets[i]->getBmp(), m_trajets[i]->getAvionX()-15, m_trajets[i]->getAvionY()-15, 11800000);
            }
            //7/8
            else if(m_trajets[i]->getArriveeX()<=m_trajets[i]->getAvionX() && m_trajets[i]->getArriveeY()<=m_trajets[i]->getAvionY()&& m_trajets[i]->getArriveeY()>m_trajets[i]->getArriveeX())
            {
                rotate_sprite(buffer, m_trajets[i]->getBmp(), m_trajets[i]->getAvionX()-15, m_trajets[i]->getAvionY()-15, 14600000);
            }
            //8/8
            else if(m_trajets[i]->getArriveeX()<=m_trajets[i]->getAvionX() && m_trajets[i]->getArriveeY()<=m_trajets[i]->getAvionY()&& m_trajets[i]->getArriveeY()<m_trajets[i]->getArriveeX())
            {
                rotate_sprite(buffer, m_trajets[i]->getBmp(), m_trajets[i]->getAvionX()-15, m_trajets[i]->getAvionY()-15, 14800000);
            }

            // si une perturbation est en cours
            if(perturbation && !m_trajets[i]->getAvionAtterrissage() && !m_trajets[i]->getAvionBoucleAttente())
            {
                // en fonction du type de la perturbation, on n'agit pas sur les mêmes trajets
                switch(type_perturbation)
                {
                /// Perturbation Atlanta Londres
                case 0:
                {
                    // si le trajet traité est un Atlanta Londres ou un Londres Atlanta
                    if((m_trajets[i]->getDepart1()=="Atlanta" && m_trajets[i]->getArrivee1()=="Londres") || (m_trajets[i]->getArrivee1()=="Atlanta" && m_trajets[i]->getDepart1()=="Londres"))
                    {
                        //std::cout<<"perturbation Atlanta Londres"<<std::endl;
                        //
                        if(m_trajets[i]->getAvionX()<426 || m_trajets[i]->getAvionX()>526 || m_trajets[i]->getAvionY()<230 ||  m_trajets[i]->getAvionY()>328 )
                        {
                            // on envoie le trajet pour calculer ses coordonnées de déplacement en A*
                            //m_trajets[i]->afficher();
                            m_trajets[i]->setCoordAstar(perturb.Astar(m_trajets[i]->getAvionX(), m_trajets[i]->getAvionY(), m_trajets[i]->getArriveeX(), m_trajets[i]->getArriveeY()));
                        }
                    }
                }
                break;

                /// Perturbation Captown Dubai
                case 1:
                {
                    if((m_trajets[i]->getDepart1()=="Captown" && m_trajets[i]->getArrivee1()=="Dubai") || (m_trajets[i]->getArrivee1()=="Captown" && m_trajets[i]->getDepart1()=="Dubai"))
                    {
                        //std::cout<<"perturbation Captown Dubai"<<std::endl;
                        if(m_trajets[i]->getAvionX()<729 || m_trajets[i]->getAvionX()>792 || m_trajets[i]->getAvionY()<396 ||  m_trajets[i]->getAvionY()>461 )
                        {
                            //m_trajets[i]->afficher();
                            m_trajets[i]->setCoordAstar(perturb.Astar(m_trajets[i]->getAvionX(), m_trajets[i]->getAvionY(), m_trajets[i]->getArriveeX(), m_trajets[i]->getArriveeY()));
                        }
                    }
                }
                break;

                /// Perturbation Melbourne Bangkok
                case 2:
                {
                    if((m_trajets[i]->getDepart1()=="Melbourne" && m_trajets[i]->getArrivee1()=="Shanghai") || (m_trajets[i]->getArrivee1()=="Melbourne" && m_trajets[i]->getDepart1()=="Shanghai"))
                    {
                        //std::cout<<"perturbation Melbourne Shangai"<<std::endl;
                        if(m_trajets[i]->getAvionX()<1054 || m_trajets[i]->getAvionX()>1119 || m_trajets[i]->getAvionY()<396 ||  m_trajets[i]->getAvionY()>461)
                        {
                            //m_trajets[i]->afficher();
                            m_trajets[i]->setCoordAstar(perturb.Astar(m_trajets[i]->getAvionX(), m_trajets[i]->getAvionY(), m_trajets[i]->getArriveeX(), m_trajets[i]->getArriveeY()));
                        }
                    }
                }
                break;
                }
            }


            /// CHOIX DU TYPE DE DEPLACEMENT DE L'AVION
            // si l'avion vole car il décolle
            if(m_trajets[i]->getVoler() && m_trajets[i]->getAvionDecollage())
            {
                m_trajets[i]->decollage();
            }
            // si l'avion vole car il est en boucle d'attente
            else if(m_trajets[i]->getVoler() && m_trajets[i]->getAvionBoucleAttente())
            {
                m_trajets[i]->boucle();
            }
            // si l'avion vole car il atterrit
            else if(m_trajets[i]->getVoler() && m_trajets[i]->getAvionAtterrissage())
            {
                m_trajets[i]->atterrissage(suppr);

                // si l'avion est arrivé à sa destination finale, on supprime son trajet
                if(suppr)
                {
                    m_trajets.erase(m_trajets.begin()+i);
                }
            }
            // si l'avion est en train de voler
            else if(m_trajets[i]->getVoler() && !m_trajets[i]->getAvionDecollage() && !m_trajets[i]->getAvionAtterrissage() && !m_trajets[i]->getAvionBoucleAttente())
            {
                /// Calcul du déplacement de l'avion
                // si le trajet n'est pas impacté par la perturbation
                if(m_trajets[i]->getCoordAstar().empty())
                {
                    m_trajets[i]->deplacement();
                }
                else
                {
                    m_trajets[i]->deplacement_astar(equivalence);

                    /// ici pour rajouter un nuage au niveau de la perturbation
                    if(type_perturbation==0)
                    {
                        draw_sprite(buffer,nuage2,375,230);
                        play_sample(son,255,0,1000,0);
                    }
                    else if(type_perturbation==1)
                    {
                        draw_sprite(buffer,explosion,665,370);
                        play_sample(son,255,0,1000,0);
                    }
                    else if(type_perturbation==2)
                    {
                        draw_sprite(buffer,nuage,985,400);
                        play_sample(son,255,0,1000,0);
                    }
                    else if(type_perturbation!=0 && type_perturbation!=1 && type_perturbation!=2)
                    {
                        stop_sample(son);
                    }
                }
            }
            // si l'avion doit encore attendre des unités de tps à l'aéroport
            else if(!m_trajets[i]->getVoler())
            {
                m_trajets[i]->attente(verif_station, verif_piste);
            }

            // si le trajet n'a pas été supprimé car l'avion a atterri, on affiche ses infos
            if(!suppr)
            {
                ///AFFICHAGES DES INFOS DE L'AVION
                if((mouse_x>=m_trajets[i]->getAvionX()-15) && (mouse_x<=m_trajets[i]->getAvionX()+15) && (mouse_y>=m_trajets[i]->getAvionY()-15) && (mouse_y<=m_trajets[i]->getAvionY()+15))
                {
                    draw_sprite(buffer,info_avions,10,490);
                    textprintf_ex(buffer, font3, 14, 515, makecol(0,0,0), -1, "%c", m_trajets[i]->getDepart()->getPremiereLettre());
                    textprintf_ex(buffer, font3, 137, 515, makecol(0,0,0), -1, "%c", m_trajets[i]->getArrivee()->getPremiereLettre());

                    if(m_trajets[i]->getVoler())
                    {
                        textprintf_ex(buffer, font2, 20, 542, makecol(0,0,0), -1, "%d:%d:%d", m_trajets[i]->getHeureDepart(), m_trajets[i]->getMinuteDepart(), m_trajets[i]->getSecondeDepart());
                        textprintf_ex(buffer, font2, 110, 542, makecol(0,0,0), -1, "%d:%d:%d", m_trajets[i]->getHeureArrivee(), m_trajets[i]->getMinuteArrivee(), m_trajets[i]->getSecondeArrivee());
                    }

                    // on récupère le nombre d'aéroports visités par l'avion pour savoir s'il fait des escales
                    taille_trajet= m_trajets[i]->getTailleTrajet();
                    switch(taille_trajet)
                    {
                    // 1 escale
                    case 3:
                    {
                        textprintf_ex(buffer, font2, 65, 573, makecol(0,0,0), -1, "%s", m_trajets[i]->getNomEscale(1).c_str());
                    }
                    break;

                    // 2 escales
                    case 4:
                    {
                        textprintf_ex(buffer, font2, 65, 573, makecol(0,0,0), -1, "%s", m_trajets[i]->getNomEscale(1).c_str());
                        textprintf_ex(buffer, font2, 65, 593, makecol(0,0,0), -1, "%s", m_trajets[i]->getNomEscale(2).c_str());
                    }
                    break;

                    // 3 escales
                    case 5:
                    {
                        textprintf_ex(buffer, font2, 65, 573, makecol(0,0,0), -1, "%s", m_trajets[i]->getNomEscale(1).c_str());
                        textprintf_ex(buffer, font2, 65, 593, makecol(0,0,0), -1, "%s", m_trajets[i]->getNomEscale(2).c_str());
                        textprintf_ex(buffer, font2, 65, 613, makecol(0,0,0), -1, "%s", m_trajets[i]->getNomEscale(3).c_str());
                    }
                    break;
                    }

                    textprintf_ex(buffer, font2, 29, 622, makecol(0,0,0), -1, "Carburant : %.0f", m_trajets[i]->getAvionCapaCarburant());
                }

                //m_trajets[i]->setCptUt(true);
            }
            // sinon
            else
            {
                // pour traiter tous les trajets, on régresse d'une place
                i=i-1;
                suppr = false;
            }
        }

        if(perturbation)
        {
            switch(type_perturbation)
            {
            /// Perturbation Atlanta Londres
            case 0:
                textprintf_centre_ex(buffer, font3, 600, 600, makecol(255,0,0), -1, "PERTURBATION   Atlanta Londres");
                break;

            /// Perturbation Captown Dubai
            case 1:
                textprintf_centre_ex(buffer, font3, 600, 600, makecol(255,0,0), -1, "PERTURBATION   Captown Dubai");
                break;

            /// Perturbation Shanghai Bangkok
            case 2:
                textprintf_centre_ex(buffer, font3, 600, 600, makecol(255,0,0), -1, "PERTURBATION   Shanghai Melbourne");
                break;
            }
        }

        perturbation = false;

        ///AFFICHAGE DES INFOS AÉROPORTS
        //ATLANTA
        if(mouse_x>=283-20 && mouse_x<=283+20 && mouse_y>=279-20 && mouse_y<=279+20)
        {
            draw_sprite(buffer,atlanta,10,490);
            if(mouse_b&1)
            {
                textprintf_centre_ex(buffer, font2, 1075, 10, makecol(255,255,255), -1, "%s", m_aeroports[3]->getNom().c_str());

                textprintf_centre_ex(buffer, font, 1148, 30, makecol(255,255,255), -1, "BOUCLE");
                if(!m_aeroports[3]->getBoucleAttente().empty())
                {
                    for(auto it : m_aeroports[3]->getBoucleAttente())
                    {
                        textprintf_centre_ex(buffer, font, 1123, 40+cpt, makecol(255,255,255), -1, "%s", it->getMatricule().c_str());
                        textprintf_ex(buffer, font, 1143, 40+cpt, makecol(255,255,255), -1, "%.0f", it->getCapaciteCarburant());
                        cpt = cpt+10;
                    }
                }
                cpt=0;
                textprintf_centre_ex(buffer, font, 1075, 30, makecol(255,255,255), -1, "STATIONS");
                if(!m_aeroports[3]->getStations().empty())
                {
                    for(auto it : m_aeroports[3]->getStations())
                    {
                        textprintf_centre_ex(buffer, font, 1075, 40+cpt, makecol(255,255,255), -1, "%s", it->getMatricule().c_str());
                        cpt = cpt+10;
                    }
                }
                cpt=0;
                textprintf_centre_ex(buffer, font, 1000, 30, makecol(255,255,255), -1, "PISTES");
                for(auto it : m_aeroports[3]->getPistes())
                {
                    if(!it.empty())
                    {
                        textprintf_ex(buffer, font, 972, 40+cpt, makecol(255,255,255), -1, "p%d : %s", (cpt/10)+1, it.front()->getMatricule().c_str());
                    }
                    else
                    {
                        textprintf_ex(buffer, font, 972, 40+cpt, makecol(255,255,255), -1, "p%d :", (cpt/10)+1);
                    }
                    cpt = cpt+10;
                }
                cpt=0;
            }
        }
        //RIO
        if(mouse_x>=426-20 && mouse_x<=426+20 && mouse_y>=479-20 && mouse_y<=479+20)
        {
            draw_sprite(buffer,rio,10,490);
            if(mouse_b&1)
            {
                textprintf_centre_ex(buffer, font2, 1075, 10, makecol(255,255,255), -1, "%s", m_aeroports[2]->getNom().c_str());

                textprintf_centre_ex(buffer, font, 1148, 30, makecol(255,255,255), -1, "BOUCLE");
                if(!m_aeroports[2]->getBoucleAttente().empty())
                {
                    for(auto it : m_aeroports[2]->getBoucleAttente())
                    {
                        textprintf_centre_ex(buffer, font, 1123, 40+cpt, makecol(255,255,255), -1, "%s", it->getMatricule().c_str());
                        textprintf_ex(buffer, font, 1143, 40+cpt, makecol(255,255,255), -1, "%.0f", it->getCapaciteCarburant());
                        cpt = cpt+10;
                    }
                }
                cpt=0;
                textprintf_centre_ex(buffer, font, 1075, 30, makecol(255,255,255), -1, "STATIONS");
                if(!m_aeroports[2]->getStations().empty())
                {
                    for(auto it : m_aeroports[2]->getStations())
                    {
                        textprintf_centre_ex(buffer, font, 1075, 40+cpt, makecol(255,255,255), -1, "%s", it->getMatricule().c_str());
                        cpt = cpt+10;
                    }
                }
                cpt=0;
                textprintf_centre_ex(buffer, font, 1000, 30, makecol(255,255,255), -1, "PISTES");
                for(auto it : m_aeroports[2]->getPistes())
                {
                    if(!it.empty())
                    {
                        textprintf_ex(buffer, font, 972, 40+cpt, makecol(255,255,255), -1, "p%d : %s", (cpt/10)+1, it.front()->getMatricule().c_str());
                    }
                    else
                    {
                        textprintf_ex(buffer, font, 972, 40+cpt, makecol(255,255,255), -1, "p%d :", (cpt/10)+1);
                    }
                    cpt = cpt+10;
                }
                cpt=0;
            }
        }
        //LONDRES
        if(mouse_x>=565-20 && mouse_x<=565+20 && mouse_y>=224-20 && mouse_y<=224+20)
        {
            draw_sprite(buffer,londres,10,490);
            if(mouse_b&1)
            {
                textprintf_centre_ex(buffer, font2, 1075, 10, makecol(255,255,255), -1, "%s", m_aeroports[0]->getNom().c_str());

                textprintf_centre_ex(buffer, font, 1148, 30, makecol(255,255,255), -1, "BOUCLE");
                if(!m_aeroports[0]->getBoucleAttente().empty())
                {
                    for(auto it : m_aeroports[0]->getBoucleAttente())
                    {
                        textprintf_centre_ex(buffer, font, 1123, 40+cpt, makecol(255,255,255), -1, "%s", it->getMatricule().c_str());
                        textprintf_ex(buffer, font, 1143, 40+cpt, makecol(255,255,255), -1, "%.0f", it->getCapaciteCarburant());
                        cpt = cpt+10;
                    }
                }
                cpt=0;
                textprintf_centre_ex(buffer, font, 1075, 30, makecol(255,255,255), -1, "STATIONS");
                if(!m_aeroports[0]->getStations().empty())
                {
                    for(auto it : m_aeroports[0]->getStations())
                    {
                        textprintf_centre_ex(buffer, font, 1075, 40+cpt, makecol(255,255,255), -1, "%s", it->getMatricule().c_str());
                        cpt = cpt+10;
                    }
                }
                cpt=0;
                textprintf_centre_ex(buffer, font, 1000, 30, makecol(255,255,255), -1, "PISTES");
                for(auto it : m_aeroports[0]->getPistes())
                {
                    if(!it.empty())
                    {
                        textprintf_ex(buffer, font, 972, 40+cpt, makecol(255,255,255), -1, "p%d : %s", (cpt/10)+1, it.front()->getMatricule().c_str());
                    }
                    else
                    {
                        textprintf_ex(buffer, font, 972, 40+cpt, makecol(255,255,255), -1, "p%d :", (cpt/10)+1);
                    }
                    cpt = cpt+10;
                }
                cpt=0;
            }
        }
        //CAPTOWN
        if(mouse_x>=635-20 && mouse_x<=635+20 && mouse_y>=526-20 && mouse_y<=526+20)
        {
            draw_sprite(buffer,captown,10,490);
            if(mouse_b&1)
            {
                textprintf_centre_ex(buffer, font2, 1075, 10, makecol(255,255,255), -1, "%s", m_aeroports[4]->getNom().c_str());

                textprintf_centre_ex(buffer, font, 1148, 30, makecol(255,255,255), -1, "BOUCLE");
                if(!m_aeroports[4]->getBoucleAttente().empty())
                {
                    for(auto it : m_aeroports[4]->getBoucleAttente())
                    {
                        textprintf_centre_ex(buffer, font, 1123, 40+cpt, makecol(255,255,255), -1, "%s", it->getMatricule().c_str());
                        textprintf_ex(buffer, font, 1143, 40+cpt, makecol(255,255,255), -1, "%.0f", it->getCapaciteCarburant());
                        cpt = cpt+10;
                    }
                }
                cpt=0;
                textprintf_centre_ex(buffer, font, 1075, 30, makecol(255,255,255), -1, "STATIONS");
                if(!m_aeroports[4]->getStations().empty())
                {
                    for(auto it : m_aeroports[4]->getStations())
                    {
                        textprintf_centre_ex(buffer, font, 1075, 40+cpt, makecol(255,255,255), -1, "%s", it->getMatricule().c_str());
                        cpt = cpt+10;
                    }
                }
                cpt=0;
                textprintf_centre_ex(buffer, font, 1000, 30, makecol(255,255,255), -1, "PISTES");
                for(auto it : m_aeroports[4]->getPistes())
                {
                    if(!it.empty())
                    {
                        textprintf_ex(buffer, font, 972, 40+cpt, makecol(255,255,255), -1, "p%d : %s", (cpt/10)+1, it.front()->getMatricule().c_str());
                    }
                    else
                    {
                        textprintf_ex(buffer, font, 972, 40+cpt, makecol(255,255,255), -1, "p%d :", (cpt/10)+1);
                    }
                    cpt = cpt+10;
                }
                cpt=0;
            }
        }
        //DUBAI
        if(mouse_x>=739-20 && mouse_x<=739+20 && mouse_y>=333-20 && mouse_y<=333+20)
        {
            draw_sprite(buffer,dubai,10,490);
            if(mouse_b&1)
            {
                textprintf_centre_ex(buffer, font2, 1075, 10, makecol(255,255,255), -1, "%s", m_aeroports[5]->getNom().c_str());

                textprintf_centre_ex(buffer, font, 1148, 30, makecol(255,255,255), -1, "BOUCLE");
                if(!m_aeroports[5]->getBoucleAttente().empty())
                {
                    for(auto it : m_aeroports[5]->getBoucleAttente())
                    {
                        textprintf_centre_ex(buffer, font, 1123, 40+cpt, makecol(255,255,255), -1, "%s", it->getMatricule().c_str());
                        textprintf_ex(buffer, font, 1143, 40+cpt, makecol(255,255,255), -1, "%.0f", it->getCapaciteCarburant());
                        cpt = cpt+10;
                    }
                }
                cpt=0;
                textprintf_centre_ex(buffer, font, 1075, 30, makecol(255,255,255), -1, "STATIONS");
                if(!m_aeroports[5]->getStations().empty())
                {
                    for(auto it : m_aeroports[5]->getStations())
                    {
                        textprintf_centre_ex(buffer, font, 1075, 40+cpt, makecol(255,255,255), -1, "%s", it->getMatricule().c_str());
                        cpt = cpt+10;
                    }
                }
                cpt=0;
                textprintf_centre_ex(buffer, font, 1000, 30, makecol(255,255,255), -1, "PISTES");
                for(auto it : m_aeroports[5]->getPistes())
                {
                    if(!it.empty())
                    {
                        textprintf_ex(buffer, font, 972, 40+cpt, makecol(255,255,255), -1, "p%d : %s", (cpt/10)+1, it.front()->getMatricule().c_str());
                    }
                    else
                    {
                        textprintf_ex(buffer, font, 972, 40+cpt, makecol(255,255,255), -1, "p%d :", (cpt/10)+1);
                    }
                    cpt = cpt+10;
                }
                cpt=0;
            }
        }
        //BANGKOK
        if(mouse_x>=889-20 && mouse_x<=889+20 && mouse_y>=372-20 && mouse_y<=372+20)
        {
            draw_sprite(buffer,bangkok,10,490);
            if(mouse_b&1)
            {
                textprintf_centre_ex(buffer, font2, 1075, 10, makecol(255,255,255), -1, "%s", m_aeroports[1]->getNom().c_str());

                textprintf_centre_ex(buffer, font, 1148, 30, makecol(255,255,255), -1, "BOUCLE");
                if(!m_aeroports[1]->getBoucleAttente().empty())
                {
                    for(auto it : m_aeroports[1]->getBoucleAttente())
                    {
                        textprintf_centre_ex(buffer, font, 1123, 40+cpt, makecol(255,255,255), -1, "%s", it->getMatricule().c_str());
                        textprintf_ex(buffer, font, 1143, 40+cpt, makecol(255,255,255), -1, "%.0f", it->getCapaciteCarburant());
                        cpt = cpt+10;
                    }
                }
                cpt=0;
                textprintf_centre_ex(buffer, font, 1075, 30, makecol(255,255,255), -1, "STATIONS");
                if(!m_aeroports[1]->getStations().empty())
                {
                    for(auto it : m_aeroports[1]->getStations())
                    {
                        textprintf_centre_ex(buffer, font, 1075, 40+cpt, makecol(255,255,255), -1, "%s", it->getMatricule().c_str());
                        cpt = cpt+10;
                    }
                }
                cpt=0;
                textprintf_centre_ex(buffer, font, 1000, 30, makecol(255,255,255), -1, "PISTES");
                for(auto it : m_aeroports[1]->getPistes())
                {
                    if(!it.empty())
                    {
                        textprintf_ex(buffer, font, 972, 40+cpt, makecol(255,255,255), -1, "p%d : %s", (cpt/10)+1, it.front()->getMatricule().c_str());
                    }
                    else
                    {
                        textprintf_ex(buffer, font, 972, 40+cpt, makecol(255,255,255), -1, "p%d :", (cpt/10)+1);
                    }
                    cpt = cpt+10;
                }
                cpt=0;
            }
        }
        //MELBOURNE
        if(mouse_x>=1044-20 && mouse_x<=1044+20 && mouse_y>=543-20 && mouse_y<=543+20)
        {
            draw_sprite(buffer,melbourne,10,490);
            if(mouse_b&1)
            {
                textprintf_centre_ex(buffer, font2, 1075, 10, makecol(255,255,255), -1, "%s", m_aeroports[7]->getNom().c_str());

                textprintf_centre_ex(buffer, font, 1148, 30, makecol(255,255,255), -1, "BOUCLE");
                if(!m_aeroports[7]->getBoucleAttente().empty())
                {
                    for(auto it : m_aeroports[7]->getBoucleAttente())
                    {
                        textprintf_centre_ex(buffer, font, 1123, 40+cpt, makecol(255,255,255), -1, "%s", it->getMatricule().c_str());
                        textprintf_ex(buffer, font, 1143, 40+cpt, makecol(255,255,255), -1, "%.0f", it->getCapaciteCarburant());
                        cpt = cpt+10;
                    }
                }
                cpt=0;
                textprintf_centre_ex(buffer, font, 1075, 30, makecol(255,255,255), -1, "STATIONS");
                if(!m_aeroports[7]->getStations().empty())
                {
                    for(auto it : m_aeroports[7]->getStations())
                    {
                        textprintf_centre_ex(buffer, font, 1075, 40+cpt, makecol(255,255,255), -1, "%s", it->getMatricule().c_str());
                        cpt = cpt+10;
                    }
                }
                cpt=0;
                textprintf_centre_ex(buffer, font, 1000, 30, makecol(255,255,255), -1, "PISTES");
                for(auto it : m_aeroports[7]->getPistes())
                {
                    if(!it.empty())
                    {
                        textprintf_ex(buffer, font, 972, 40+cpt, makecol(255,255,255), -1, "p%d : %s", (cpt/10)+1, it.front()->getMatricule().c_str());
                    }
                    else
                    {
                        textprintf_ex(buffer, font, 972, 40+cpt, makecol(255,255,255), -1, "p%d :", (cpt/10)+1);
                    }
                    cpt = cpt+10;
                }
                cpt=0;
            }
        }
        //SHANGHAI
        if(mouse_x>=963-20 && mouse_x<=963+20 && mouse_y>=308-20 && mouse_y<=308+20)
        {
            draw_sprite(buffer,shanghai,10,490);
            if(mouse_b&1)
            {
                textprintf_centre_ex(buffer, font2, 1075, 10, makecol(255,255,255), -1, "%s", m_aeroports[6]->getNom().c_str());

                textprintf_centre_ex(buffer, font, 1148, 30, makecol(255,255,255), -1, "BOUCLE");
                if(!m_aeroports[6]->getBoucleAttente().empty())
                {
                    for(auto it : m_aeroports[6]->getBoucleAttente())
                    {
                        textprintf_centre_ex(buffer, font, 1123, 40+cpt, makecol(255,255,255), -1, "%s", it->getMatricule().c_str());
                        textprintf_ex(buffer, font, 1143, 40+cpt, makecol(255,255,255), -1, "%.0f", it->getCapaciteCarburant());
                        cpt = cpt+10;
                    }
                }
                cpt=0;
                textprintf_centre_ex(buffer, font, 1075, 30, makecol(255,255,255), -1, "STATIONS");
                if(!m_aeroports[6]->getStations().empty())
                {
                    for(auto it : m_aeroports[6]->getStations())
                    {
                        textprintf_centre_ex(buffer, font, 1075, 40+cpt, makecol(255,255,255), -1, "%s", it->getMatricule().c_str());
                        cpt = cpt+10;
                    }
                }
                cpt=0;
                textprintf_centre_ex(buffer, font, 1000, 30, makecol(255,255,255), -1, "PISTES");
                for(auto it : m_aeroports[6]->getPistes())
                {
                    if(!it.empty())
                    {
                        textprintf_ex(buffer, font, 972, 40+cpt, makecol(255,255,255), -1, "p%d : %s", (cpt/10)+1, it.front()->getMatricule().c_str());
                    }
                    else
                    {
                        textprintf_ex(buffer, font, 972, 40+cpt, makecol(255,255,255), -1, "p%d :", (cpt/10)+1);
                    }
                    cpt = cpt+10;
                }
                cpt=0;
            }
        }

        /// AFFICHAGE GRAPHIQUE
        textprintf_centre_ex(buffer, font, 600, 10, makecol(255,255,255), -1, "%d UT", cpt_ut);
        draw_sprite(screen, buffer, 0, 0);

        /*std::cout<<std::endl<<cpt_ut<<" UT"<<std::endl;
        for(auto it : m_trajets)
        {
            it->afficher();
            std::cout<<std::endl;
        }*/


        /// CHOIX ALEATOIRE DES PROCHAINS DEPARTS
        // on plafonne le nombre de trajets dans le monde en même temps à 5
        if(m_trajets.size()>=m_avions.size()-10) {}
        else
        {
            do
            {
                nb_km.clear();
                chemin.clear();
                verif3 = true;
                //verif = true;
                //verif2 = true;

                // choix aléatoire de l'aéroport de départ
                depart_int = rand()%(7+1);

                //s'il n'y a plus d'avions dans l'aéroport de départ
                if(m_aeroports[depart_int]->getStations().empty())
                {
                    verif = true;               //booléen pour ne pas envoyer d'avions depuis l'aéroport de départ
                    verif2 = false;             //booléen pour rajouter ou non le trajet dans le vecteur de trajets
                }
                //s'il y a encore des avions dans les stations
                else
                {
                    // assignation de l'aéroport de départ
                    depart = m_aeroports[depart_int]->getPremiereLettre();

                    // vérification que le premier avion de la station ne soit pas déjà assigné
                    // si c'est le cas, on change d'aéroport de départ
                    if(!m_aeroports[depart_int]->getAvion()->getAssignation())
                        type = m_aeroports[depart_int]->getAvion()->getType()[0];
                    else
                    {
                        verif3 = false;
                        verif2 = false;
                    }

                    if(verif3)
                    {
                        // choix aléatoire de l'aéroport d'arrivée
                        arrivee_int = rand()%(7+1);

                        // vérification que les aéroports de départ et d'arrivée ne soient pas les mêmes
                        while(arrivee_int==depart_int)
                        {
                            arrivee_int = rand()%(7+1);
                        }
                        arrivee = m_aeroports[arrivee_int]->getPremiereLettre();

                        // dijkstra pour un trajet aléatoire
                        chemin = Dijkstra(depart, arrivee, type, nb_km);

                        //si aucun chemin n'a été trouvé, on relance dijkstra jusqu'à en trouver un
                        if(chemin.empty())
                            verif = false;
                        else
                            verif = true;
                    }
                }
            }
            while(!verif);

            /// Ajout du trajet dans le vecteur trajets
            if(verif2)
            {
                m_trajets.push_back(new Trajet{nb_km, chemin, m_aeroports[depart_int]->getAvion()});
            }
        }

        rest(2000);

        // on redéfinit les booléens sur leur valeur intiale
        verif=true;
        verif2=true;

        clear_bitmap(buffer);
    }

    // destruction des bitmaps utilisées
    destroy_bitmap(atlanta);
    destroy_bitmap(rio);
    destroy_bitmap(londres);
    destroy_bitmap(shanghai);
    destroy_bitmap(bangkok);
    destroy_bitmap(captown);
    destroy_bitmap(dubai);
    destroy_bitmap(melbourne);
    destroy_bitmap(nuage2);
    destroy_bitmap(nuage);
    destroy_bitmap(explosion);
    destroy_sample(son);
}

/// Algorithme DIJKSTRA
std::vector <Aeroport *> Monde::Dijkstra(char depart, char arrive, char type,std::vector <float> & distance )
{
    std::vector <Aeroport *> parcours_avion_inter;
    std::vector <Aeroport *> parcours_avion;
    std::map<char,char> pred ;
    std::map<char,float> couleurs ;
    std::map<char,float> dists;
    std::pair < Aeroport*,double>  p;
    int distance_capa = 6000;

    if(type=='C')
    {
        distance_capa = 6000;
    }
    else if (type=='M')
    {
        distance_capa = 8000;
    }
    else if (type=='L')
    {
        distance_capa = 10000;
    }

    //m_sucesseur = m_distance aeroport
    ///définition de la fonction de comparaison cmp
    auto cmp = [](std::pair< Aeroport*,double> p1, std::pair< Aeroport*,double> p2)
    {
        return p2.second<p1.second;
    };

    /// déclaration de la file de priorité
    std::priority_queue<std::pair< Aeroport*,double>,std::vector<std::pair< Aeroport*,double>>,decltype(cmp)> file(cmp);

    ///Initialisation de map distance qui associe un aeroport a une distance, de map couleur qui permet de savoir si un aeroport est parcouru, map predecesseur qui permet de savoir l'étape precedente
    for (size_t i=0; i<m_aeroports.size(); i++)
    {
        dists.insert({m_aeroports[i]->getPremiereLettre(),-1});
        couleurs.insert({m_aeroports[i]->getPremiereLettre(),0});
        pred.insert({m_aeroports[i]->getPremiereLettre(),0});
    }
    ///On rentre l'aeroport de depart dans le djikstra
    for(const auto& it: m_aeroports)
    {
        if (it->getPremiereLettre()==depart)
        {
            file.push({it,0});
            auto it2 = dists.find(it->getPremiereLettre());
            it2->second=0;
        }
    }

    //La condition de sortie est que la file soit vide
    while(!file.empty())
    {
        p=file.top();

        //Certains sommets ont déja étaient marqués car on a utilisé un chemin plus court, on les enlève donc de la file
        if(couleurs[p.first->getPremiereLettre()]==1)
        {
            file.pop();
        }
        else
        {
            file.pop();
            //On marque le sommet que l'on traite
            auto it3 = couleurs.find(p.first->getPremiereLettre());
            it3->second=1;

            //Pour tous les sucesseurs
            for(const auto& it: (p.first)->getDistanceAeroport())
            {
                /* Si ils sont non marqués et que le chemin
                qui passe par le sommet en cours de traitement est plus court*/

                ///it parcourt le vecteur map
                //auto it2 = dists.find(it->getPremiereLettre())

                ///On cherche si dans la map couleur aeroport a deja été parcouru
                auto it4 = couleurs.find(it.first);
                auto it5 = dists.find(it.first);

                ///it parcourt de la map des successeur sommet actuel
                ///p.second = distance du sommet traité
                ///it.second = distance du successeur du sommet traite
                ///it5->second = distance sommet initial/ sommet successeur

                if( it4->second == 0 && ((p.second)+(it.second) < it5->second || (it5->second == -1) ) && it.second < distance_capa )
                {
                    //On leur attribue une distance estimée la plus courte
                    it5->second = p.second+it.second;
                    //On note le prédecesseur qui permet d'arriver au sommet par le chemin le plus court
                    auto it6 = pred.find (it.first);
                    it6->second = p.first->getPremiereLettre();
                    //On l'ajoute a la file
                    for(const auto& it7: m_aeroports)
                    {
                        if (it7->getPremiereLettre()==it.first)
                        {
                            file.push({it7,((p.second)+(it.second))});
                        }
                    }
                }
            }
        }
    }

    ///Affichage du Dijkstra
    //std::cout<< std::endl;
    //std::cout<< "Parcours Djiska a partir de  " << depart <<std::endl;
    char interm;

    ///it different aeroport
    ///
    /*for(const auto& it: m_aeroports)
    {
        auto it10 = couleurs.find(it->getPremiereLettre());
        if(it10->second == 1 && it->getPremiereLettre()!= depart)
        {
            std::cout<< it->getPremiereLettre()  ;
            interm = it->getPremiereLettre();
            while(interm!=depart)
            {
                std::cout << "<--" ;
                auto it13 = pred.find(interm);
                std::cout << it13->second;
                interm=it13->second;
            }
            std::cout<< std::endl ;
        }
    }*/
    std::vector<Aeroport *>::iterator it15;

    for(const auto& it: m_aeroports)
    {
        auto it10 = couleurs.find(it->getPremiereLettre());
        /// Si on a parcouru cette aeroport, que c'est l'aeroport d'arrive,
        if(it10->second == 1  && it->getPremiereLettre()== arrive)
        {
            //std::cout<< it->getPremiereLettre();
            interm = it->getPremiereLettre();
            parcours_avion_inter.push_back(it);
            //Ajouter S

            while(interm!=depart)
            {
                //std::cout << "<--" ;
                auto it13 = pred.find(interm);
                //std::cout << it13->second;
                ///it14 parcourt vecteur d'aeroport
                ///it13 nom predecesseur
                ///it15 iterateur parcours_avion
                for(const auto& it14: m_aeroports)
                {
                    if (it14->getPremiereLettre()==it13->second)
                    {
                        parcours_avion_inter.push_back(it14);
                    }
                }
                interm=it13->second;
            }
            //std::cout<< std::endl ;
        }
    }
    for(const auto& it16: parcours_avion_inter)
    {
        parcours_avion.push_back(parcours_avion_inter.back());
        parcours_avion_inter.pop_back();
    }

    Aeroport * inter_aero;
    int z = 0;
    float interf;

    /// on parcout le vecteur d'aeroport par lequel l'avion passe
    for(const auto& it17: parcours_avion)
    {
        ///Toutes les deux cases du vecteur
        if(z!=0)
        {
            for(const auto& it18: it17->getDistanceAeroport() )
            {
                if (it18.first == inter_aero->getPremiereLettre())
                {
                    interf=float(it18.second);
                    distance.push_back(interf);
                }
            }
        }
        inter_aero=it17;
        z++;
    }

    return (parcours_avion) ;
}

/// Algorithme pour trouver les PCC
void Monde::afficher_court_chemin ()
{
    char type, aero_depart;
    /*std::cout << "PLUS COURT CHEMIN" << std::endl;
    std::cout << "Parametre du trajet " << std::endl;
    std::cout << std::endl << std::endl;
    std::cout << "Quel est le type de l'avion ?:" <<std::endl;
    std::cout << std::endl;
    std::cout << "C : avion de courte taille" <<std::endl;
    std::cout << "M : avion de moyenne taille" <<std::endl;
    std::cout << "L : avion de longue taille" <<std::endl;
    std::cin >>  type;
    std::cout << "Quel est l'aeroport de départ de l'avion ?:" <<std::endl;
    std::cout << std::endl;
    std::cin >>  aero_depart;*/


    std::vector <float> distance;
    std::map <char,char> pred;
    std::map<char,float> couleurs ;
    std::vector <Aeroport*> Aeroport;
    int X;
    int Y;
    int X_inter;
    int Y_inter;
    char interm;
    int dep_affichage=0;
    int type_affichage=0;


    BITMAP *buffer=NULL, *fond=NULL;

    fond=load_bitmap("bmp/monde.bmp", NULL);

    buffer = create_bitmap(1200, 650);
    clear_bitmap(buffer);

    blit(fond, buffer, 0,0,0,0, 1200, 650);

    while (dep_affichage==0 || type_affichage==0)
    {
        blit(fond, buffer, 0,0,0,0, 1200, 650);
        //ATLANTA
        if((mouse_b&1) && mouse_x>=283-20 && mouse_x<=283+20 && mouse_y>=279-20 && mouse_y<=279+20)
        {
            aero_depart='A';
            dep_affichage=1;
        }
        //RIO
        if((mouse_b&1) && mouse_x>=426-20 && mouse_x<=426+20 && mouse_y>=479-20 && mouse_y<=479+20)
        {
            aero_depart='R';
            dep_affichage=1;
        }
        //LONDRES
        if((mouse_b&1) && mouse_x>=565-20 && mouse_x<=565+20 && mouse_y>=224-20 && mouse_y<=224+20)
        {
            aero_depart='L';
            dep_affichage=1;
        }
        //CAPTOWN
        if((mouse_b&1) && mouse_x>=635-20 && mouse_x<=635+20 && mouse_y>=526-20 && mouse_y<=526+20)
        {
            aero_depart='C';
            dep_affichage=1;
        }
        //DUBAI
        if((mouse_b&1) && mouse_x>=739-20 && mouse_x<=739+20 && mouse_y>=333-20 && mouse_y<=333+20)
        {
            aero_depart='D';
            dep_affichage=1;
        }
        //BANGKOK
        if((mouse_b&1) && mouse_x>=889-20 && mouse_x<=889+20 && mouse_y>=372-20 && mouse_y<=372+20)
        {
            aero_depart='B';
            dep_affichage=1;
        }
        //MELBOURNE
        if((mouse_b&1) && mouse_x>=1044-20 && mouse_x<=1044+20 && mouse_y>=543-20 && mouse_y<=543+20)
        {
            aero_depart='M';
            dep_affichage=1;
        }
        //SHANGHAI
        if( (mouse_b&1) && mouse_x>=963-20 && mouse_x<=963+20 && mouse_y>=308-20 && mouse_y<=308+20)
        {
            aero_depart='S';
            dep_affichage=1;
        }
        //Long
        if( (mouse_b&1) && mouse_x>=8 && mouse_x<=26 && mouse_y>=25 && mouse_y<=45)
        {
            type='L';
            type_affichage=1;
        }
        //Moyen
        if( (mouse_b&1) && mouse_x>=8 && mouse_x<=26 && mouse_y>=58 && mouse_y<=80)
        {
            type='M';
            type_affichage=1;
        }
        if( (mouse_b&1) && mouse_x>=8 && mouse_x<=26 && mouse_y>=90 && mouse_y<=108)
        {
            type='C';
            type_affichage=1;
        }

        draw_sprite(screen, buffer, 0, 0);
    }

    Aeroport = Dijkstra_affichage(aero_depart,'B',type,distance,pred,couleurs);

    while(!key[KEY_ESC])
    {
        if (Aeroport.begin()!=Aeroport.end())
        {
            for(const auto& it: m_aeroports)
            {
                auto it10 = couleurs.find(it->getPremiereLettre());
                if(it10->second == 1 && it->getPremiereLettre()!= aero_depart)
                {

                    ///Recup coordonné x et y du premier aeroport
                    ///Si aeroport diff de 1
                    X=it->getX();
                    Y=it->getY();
                    //std::cout<< it->getPremiereLettre();
                    interm = it->getPremiereLettre();
                    while(interm!=aero_depart)
                    {
                        //std::cout << "<--" ;
                        auto it13 = pred.find(interm);

                        ///it13 donne lettre aeroport predecesseur
                        for(const auto& it14: m_aeroports)
                        {
                            if (it14->getPremiereLettre() == it13->second)
                            {
                                X_inter=it14->getX();
                                Y_inter=it14->getY();
                            }
                        }

                        for (int j=0; j<5; j++)
                        {
                            line(buffer, X, Y-j, X_inter, Y_inter-j,makecol(0,122,122));
                        }

                        //std::cout << it13->second;
                        interm=it13->second;
                        X=X_inter;
                        Y=Y_inter;
                    }
                    //std::cout<< std::endl ;
                    ///Stocker les données aeroport interm
                }
                draw_sprite(screen, buffer, 0, 0);
            }
        }
        else
        {
            textprintf_ex(buffer,font,100,500,makecol(0,122,122),-1,"PAS DE TRAJET POSSIBLE");
            draw_sprite(screen, buffer, 0, 0);
        }
    }
}

/// Algorithme DIJKSTRA pour les PCC
std::vector <Aeroport *> Monde::Dijkstra_affichage(char depart, char arrive, char type,std::vector <float> & distance,std::map<char,char> &pred, std::map<char,float> &couleurs )
{
    std::vector <Aeroport *> parcours_avion_inter;
    std::vector <Aeroport *> parcours_avion;
    //std::map<char,char> pred ;
    //std::map<char,float> couleurs ;
    std::map<char,float> dists;
    std::pair < Aeroport*,double>  p;
    int distance_capa = 6000;

    if(type=='C')
    {
        distance_capa = 6000;
    }
    else if (type=='M')
    {
        distance_capa = 8000;
    }
    else if (type=='L')
    {
        distance_capa = 10000;
    }

    //m_sucesseur = m_distance aeroport
    ///définition de la fonction de comparaison cmp
    auto cmp = [](std::pair< Aeroport*,double> p1, std::pair< Aeroport*,double> p2)
    {
        return p2.second<p1.second;
    };

    /// déclaration de la file de priorité
    std::priority_queue<std::pair< Aeroport*,double>,std::vector<std::pair< Aeroport*,double>>,decltype(cmp)> file(cmp);

    ///Initialisation de map distance qui associe un aeroport a une distance, de map couleur qui permet de savoir si un aeroport est parcouru, map predecesseur qui permet de savoir l'étape precedente
    for (size_t i=0; i<m_aeroports.size(); i++)
    {
        dists.insert({m_aeroports[i]->getPremiereLettre(),-1});
        couleurs.insert({m_aeroports[i]->getPremiereLettre(),0});
        pred.insert({m_aeroports[i]->getPremiereLettre(),0});
    }
    ///On rentre l'aeroport de depart dans le djikstra
    for(const auto& it: m_aeroports)
    {
        if (it->getPremiereLettre()==depart)
        {
            file.push({it,0});
            auto it2 = dists.find(it->getPremiereLettre());
            it2->second=0;
        }
    }

    //La condition de sortie est que la file soit vide
    while(!file.empty())
    {
        p=file.top();

        //Certains sommets ont déja étaient marqués car on a utilisé un chemin plus court, on les enlève donc de la file
        if(couleurs[p.first->getPremiereLettre()]==1)
        {
            file.pop();
        }
        else
        {
            file.pop();
            //On marque le sommet que l'on traite
            auto it3 = couleurs.find(p.first->getPremiereLettre());
            it3->second=1;

            //Pour tous les sucesseurs
            for(const auto& it: (p.first)->getDistanceAeroport())
            {
                /* Si ils sont non marqués et que le chemin
                qui passe par le sommet en cours de traitement est plus court*/

                ///it parcourt le vecteur map
                //auto it2 = dists.find(it->getPremiereLettre())

                ///On cherche si dans la map couleur aeroport a deja été parcouru
                auto it4 = couleurs.find(it.first);
                auto it5 = dists.find(it.first);

                ///it parcourt de la map des successeur sommet actuel
                ///p.second = distance du sommet traité
                ///it.second = distance du successeur du sommet traite
                ///it5->second = distance sommet initial/ sommet successeur

                if( it4->second == 0 && ((p.second)+(it.second) < it5->second || (it5->second == -1) ) && it.second < distance_capa )
                {
                    //On leur attribue une distance estimée la plus courte
                    it5->second = p.second+it.second;
                    //On note le prédecesseur qui permet d'arriver au sommet par le chemin le plus court
                    auto it6 = pred.find (it.first);
                    it6->second = p.first->getPremiereLettre();
                    //On l'ajoute a la file
                    for(const auto& it7: m_aeroports)
                    {
                        if (it7->getPremiereLettre()==it.first)
                        {
                            file.push({it7,((p.second)+(it.second))});
                        }
                    }
                }
            }
        }
    }

    ///Affichage du Dijkstra
    //std::cout<< std::endl;
    //std::cout<< "Parcours Djiska a partir de  " << depart <<std::endl;
    char interm;

    ///it different aeroport
    ///
    /*for(const auto& it: m_aeroports)
    {
        auto it10 = couleurs.find(it->getPremiereLettre());
        if(it10->second == 1 && it->getPremiereLettre()!= depart)
        {
            std::cout<< it->getPremiereLettre()  ;
            interm = it->getPremiereLettre();
            while(interm!=depart)
            {
                std::cout << "<--" ;
                auto it13 = pred.find(interm);
                std::cout << it13->second;
                interm=it13->second;
            }
            std::cout<< std::endl ;
        }
    }*/
    std::vector<Aeroport *>::iterator it15;

    for(const auto& it: m_aeroports)
    {
        auto it10 = couleurs.find(it->getPremiereLettre());
        /// Si on a parcouru cette aeroport, que c'est l'aeroport d'arrive,
        if(it10->second == 1  && it->getPremiereLettre()== arrive)
        {
            //std::cout<< it->getPremiereLettre();
            interm = it->getPremiereLettre();
            parcours_avion_inter.push_back(it);
            //Ajouter S

            while(interm!=depart)
            {
                //std::cout << "<--" ;
                auto it13 = pred.find(interm);
                //std::cout << it13->second;
                ///it14 parcourt vecteur d'aeroport
                ///it13 nom predecesseur
                ///it15 iterateur parcours_avion
                for(const auto& it14: m_aeroports)
                {
                    if (it14->getPremiereLettre()==it13->second)
                    {
                        parcours_avion_inter.push_back(it14);
                    }
                }
                interm=it13->second;
            }
            //std::cout<< std::endl ;
        }
    }
    for(const auto& it16: parcours_avion_inter)
    {
        parcours_avion.push_back(parcours_avion_inter.back());
        parcours_avion_inter.pop_back();
    }

    Aeroport * inter_aero;
    int z = 0;
    float interf;

    /// on parcout le vecteur d'aeroport par lequel l'avion passe
    for(const auto& it17: parcours_avion)
    {
        ///Toutes les deux cases du vecteur
        if(z!=0)
        {
            for(const auto& it18: it17->getDistanceAeroport() )
            {
                if (it18.first == inter_aero->getPremiereLettre())
                {
                    interf=float(it18.second);
                    distance.push_back(interf);
                }
            }
        }
        inter_aero=it17;
        z++;
    }

    return (parcours_avion) ;
}

/// Algorithme Welsh et Powell pour l'altitude
void Monde::Welsh_et_Powell ()
{

    std::map< Trajet*, int> tab_degre;
    std::map <Trajet*, int> altitude;


    for(const auto& it: m_trajets)
    {
        tab_degre.insert({it,0});
        altitude.insert({it,0});
    }

///Création d'un graphe exploitable par Welsh et Powell

    for(size_t i=0; i<m_trajets.size(); i++)
    {
        for(const auto& it: m_trajets)
        {
            ///Blinder pas le même sommet
            ///Si ils font le même trajet ou le trajet inverse
            if ((m_trajets[i]->getDepart1()== it->getDepart1()&& m_trajets[i]->getArrivee1()== it->getArrivee1())||(m_trajets[i]->getDepart1()== it->getArrivee1()&& m_trajets[i]->getArrivee1()== it->getDepart1()))
            {
                ///On le met en successeur et on lui incremente sa connexité
                auto it1 = tab_degre.find(m_trajets[i]);
                it1->second = it1->second + 1;
                m_trajets[i]->setSuccesseurs_WP(it);

            }
            ///Si les trajets se croisent
            else if (((m_trajets[i]->getDepart1()== "Shanghai"&& m_trajets[i]->getArrivee1()== "Dubai")||(m_trajets[i]->getDepart1()== "Dubai" && m_trajets[i]->getArrivee1()== "Shanghai" ))&& ((it->getDepart1()== "Londres" && it->getArrivee1()== "Bangkok")||(it->getDepart1()== "Bangkok" && it->getArrivee1()== "Londres" )))
            {
                ///On le met en successeur et on lui incremente son compteur de connexité
                auto it1 = tab_degre.find(m_trajets[i]);
                it1->second = it1->second + 1;
                m_trajets[i]->setSuccesseurs_WP(it);
            }
            else if (((m_trajets[i]->getDepart1()== "Shanghai"&& m_trajets[i]->getArrivee1()== "Captown")||(m_trajets[i]->getDepart1()== "Captown" && m_trajets[i]->getArrivee1()== "Shanghai" ))&& ((it->getDepart1()== "Londres" && it->getArrivee1()== "Bangkok")||(it->getDepart1()== "Bangkok" && it->getArrivee1()== "Londres" )))
            {
                ///On le met en successeur et on lui incremente son compteur de connexité
                auto it2 = tab_degre.find(m_trajets[i]);
                it2->second = it2->second + 1;
                m_trajets[i]->setSuccesseurs_WP(it);
            }
            else if (((m_trajets[i]->getDepart1()== "Shanghai"&& m_trajets[i]->getArrivee1()== "Captown")||(m_trajets[i]->getDepart1()== "Captown" && m_trajets[i]->getArrivee1()== "Shanghai" ))&& ((it->getDepart1()== "Dubai" && it->getArrivee1()== "Bangkok")||(it->getDepart1()== "Bangkok" && it->getArrivee1()== "Dubai" )))
            {
                ///On le met en successeur et on lui incremente son compteur de connexité
                auto it3 = tab_degre.find(m_trajets[i]);
                it3->second = it3->second + 1;
                m_trajets[i]->setSuccesseurs_WP(it);
            }
        }
    }

    ///ALGORITHME WELSH ET POWELL

    ///Chaque couleur differente correspond à une vitesse differente
    int vitesse_initial= 9200;
    int incrementation_vit=0;
    bool booleen_successeur;

    ///Trie la liste des sommets par ordre décroissant de degrés



    auto cmp = [](std::pair< Trajet*,int> p1, std::pair< Trajet*,int> p2)
    {
        ///On cherche le sommet le plus connecté
        return p2.second>p1.second;
    };

    /// déclaration de la file de priorité
    std::priority_queue<std::pair< Trajet*,int>,std::vector<std::pair< Trajet*,int>>,decltype(cmp)> file(cmp);


    for(const auto& it: m_trajets)
    {
        auto it6 = tab_degre.find(it);
        file.push({it6->first,it6->second});
    }

    /*Parcourir la liste triée :
    − Trouver le premier sommet non déjà coloré et lui attribuer la « plus
    petite » couleur c non déjà utilisée.
    − Parcourir la suite de la liste en attribuant cette couleur c aux
    sommets non colorés et non adjacents aux sommets déjà colorés
    avec c

    3. Recommencer en 2 s’il reste des sommets non colorés*/


    std::pair< Trajet*,int> interm;

    while (!file.empty())
    {

        interm = file.top();
        auto it17 = altitude.find(interm.first);

        if(it17->second!=0)
        {
            file.pop();
        }
        else
        {
            file.pop();
            it17->second = vitesse_initial + incrementation_vit;

            for(const auto& it: m_trajets)
            {
                for(const auto& it2: interm.first->getSuccesseurs_WP())
                {
                    if(it==it2)
                    {
                        booleen_successeur = false;
                    }
                }

                if(booleen_successeur)
                {
                    auto it18 = altitude.find(it);
                    ///Si le sommet n'est pas déja marqué
                    if (it18->second==0)
                    {
                        it18->second = vitesse_initial + incrementation_vit;
                    }

                }

                booleen_successeur = true ;
            }

            incrementation_vit = incrementation_vit + 400;

        }


    }

    ///On attribue une vitesse à chaque sommet

    for(const auto& it: m_trajets)
    {
       auto it19 = altitude.find(it);
       it->setAltitude(it19->second);
    }

    for(const auto& it: m_trajets)
    {
       std::cout <<it->getDepart1() << "  " <<it->getArrivee1() << "  "<< it->getAltitude() <<std::endl;
    }

}
