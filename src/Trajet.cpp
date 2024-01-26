#include "Trajet.h"

/// Constructeur
Trajet::Trajet(std::vector<float> _nb_km, std::vector<Aeroport*> chemin, Avion* _avion)
{
    size_t i=0;

    for(i=0; i<chemin.size(); i++)
    {
        m_chemin.push_back(chemin[i]);
    }

    m_depart = m_chemin[0];
    m_arrivee = m_chemin[1];
    m_avion_actuel = _avion;
    m_avion_actuel->setAssignation(true);
    m_cpt_escale = 2;

    m_nb_km = _nb_km;

    for(i=0; i<m_nb_km.size(); i++)
        m_unite_tps.push_back(m_nb_km[i]/200);

    m_voler = false;

    m_cpt_ut = 0;
}

/// Destructeur
Trajet::~Trajet()
{

}

/// Getters et setters
float Trajet::getAvionX()
{
    return m_avion_actuel->getX();
}

float Trajet::getAvionY()
{
    return m_avion_actuel->getY();
}

int Trajet::getDepartX()
{
    return m_depart->getX();
}

int Trajet::getDepartY()
{
    return m_depart->getY();
}

int Trajet::getArriveeX()
{
    return m_arrivee->getX();
}

int Trajet::getArriveeY()
{
    return m_arrivee->getY();
}

BITMAP* Trajet::getBmp()
{
    return m_avion_actuel->getBmp();
}

bool Trajet::getVoler()
{
    return m_voler;
}

bool Trajet::getAvionDecollage()
{
    return m_avion_actuel->getDecollage();
}

bool Trajet::getAvionAtterrissage()
{
    return m_avion_actuel->getAtterrissage();
}

bool Trajet::getAvionBoucleAttente()
{
    return m_avion_actuel->getBoucleAttente();
}

Aeroport* Trajet::getArrivee()
{
    return m_chemin.back();
}

Aeroport* Trajet::getDepart()
{
    return m_chemin.front() ;
}

std::string Trajet::getArrivee1()
{
    return m_arrivee->getNom();
}

std::string Trajet::getDepart1()
{
    return m_depart->getNom();
}

bool Trajet::getAvionAssignation()
{
    return m_avion_actuel->getAssignation();
}

float Trajet::getAvionCapaCarburant()
{
    return m_avion_actuel->getCapaciteCarburant();
}

void Trajet::setNvllesDestinations()
{
    m_depart = m_arrivee;
    m_arrivee = m_chemin[m_cpt_escale];

    m_voler = false;

    m_cpt_escale++;
}

int Trajet::getTailleTrajet()
{
    return m_chemin.size();
}

std::string Trajet::getNomEscale(int place)
{
    return m_chemin[place]->getNom();
}

int Trajet::getCptUt()
{
    return m_cpt_ut;
}

void Trajet::setCptUt(bool cpt)
{
    if(!cpt)
        m_cpt_ut=0;
    else
        m_cpt_ut++;
}

void Trajet::setCoordAstar(std::vector<Coordonnee*> coord_astar)
{
    m_coord_astar = coord_astar;
}

std::vector<Coordonnee*> Trajet::getCoordAstar()
{
    return m_coord_astar;
}

int Trajet::getHeureArrivee()
{
    return m_heure_arrivee;
}

int Trajet::getHeureDepart()
{
    return m_heure_depart;
}

int Trajet::getMinuteArrivee()
{
    return m_minute_arrivee;
}

int Trajet::getMinuteDepart()
{
    return m_minute_depart;
}

int Trajet::getSecondeArrivee()
{
    return m_seconde_arrivee;
}

int Trajet::getSecondeDepart()
{
    return m_seconde_depart;
}

int Trajet::getAltitude()
{
    return m_altitude;
}
void Trajet::setAltitude(int nv_altitude)
{
    m_altitude=nv_altitude;
}
std::vector<Trajet*> Trajet::getSuccesseurs_WP ()
{
    return m_successeurs_WP;
}
void Trajet::setSuccesseurs_WP (Trajet* traj)
{
    m_successeurs_WP.push_back(traj);
}

/// M�thode d'affichage
void Trajet::afficher()
{
    size_t j=0;

    std::cout<<m_chemin.front()->getNom()<<" "<<m_chemin.back()->getNom()<<" Matricule : "<<m_avion_actuel->getMatricule()<<std::endl;
    std::cout<<" X = "<<m_avion_actuel->getX()<<" Y = "<<m_avion_actuel->getY()<<std::endl;
    if(m_chemin.size()>2)
    {
        for(size_t i=0; i<m_chemin.size(); i++)
        {
            std::cout<<m_chemin[i]->getNom();
            if(j<m_chemin.size()-1)
                std::cout<<" > "<<m_nb_km[i]<<"km "<<m_unite_tps[i]<<"UT"<<" > ";
            j++;
        }
    }
    std::cout<<std::endl<<"Trajet en cours : "<<m_depart->getNom()<<" --> "<<m_arrivee->getNom()<<std::endl;
}

/// Calcul de la nouvelle position des avions en vol lorsqu'il y a une perturbation
void Trajet::deplacement_astar(std::map<std::pair<float, float>, std::pair<float, float>> equivalence)
{
    bool help=false;
    float x=0, y=0;
    Coordonnee* inter;

    inter = m_coord_astar[0];
    m_coord_astar.erase(m_coord_astar.begin());

    std::pair<float,float> p = std::make_pair(inter->getX(),inter->getY());

    auto it = equivalence.find(p);
    x = it->second.first;
    y = it->second.second;

    m_avion_actuel->setCoord(x,y);

    m_avion_actuel->setCapaciteCarburant(m_avion_actuel->getCapaciteCarburant()-(2*m_avion_actuel->getConso()));


    if(((m_avion_actuel->getX()>=m_arrivee->getX()-30) && (m_avion_actuel->getX()<=m_arrivee->getX()+30)) && ((m_avion_actuel->getY()>=m_arrivee->getY()-30) && (m_avion_actuel->getY()<=m_arrivee->getY()+30)))
    {
        help=true;
    }

    // si l'avion est dans le p�rim�tre d'atterrissage de son a�roport d'arriv�e
    if(help)
    {
        m_avion_actuel->setBoucleAttente(true);
        m_arrivee->ajoutAvionBoucleAttente(m_avion_actuel);
        m_avion_actuel->setCptUt(false);

        // on vide le vecteur de coordonn�es A*
        int taille = m_coord_astar.size();
        for(int i=0; i<taille; i++)
        {
            m_coord_astar.pop_back();
        }
    }
}

/// Calcul de la nouvelle position des avions en vol sans perturbation
void Trajet::deplacement()
{
    //std::cout<<"            DEPLACEMENT"<<std::endl;
    float diff_x=0, diff_y=0;
    float depl_x=0, depl_y=0;
    bool help=false;

    // calcul de la prochaine position de l'avion
    diff_x = m_arrivee->getX()-m_depart->getX();
    diff_y = m_arrivee->getY()-m_depart->getY();
    depl_x = diff_x/(m_unite_tps[m_cpt_escale-2]);
    depl_y = diff_y/(m_unite_tps[m_cpt_escale-2]);

    // d�termination de la prochaine position de l'avion en X et Y
    m_avion_actuel->setCoord(m_avion_actuel->getX()+depl_x, m_avion_actuel->getY()+depl_y);

    m_avion_actuel->setCapaciteCarburant(m_avion_actuel->getCapaciteCarburant()-(2*m_avion_actuel->getConso()));

    // si a�roport est en bas � droite de l'avion
    if(m_arrivee->getX()>m_avion_actuel->getX() && m_arrivee->getY()>m_avion_actuel->getY())
    {
        if(((m_avion_actuel->getX()>=m_arrivee->getX()-(2*depl_x)) && (m_avion_actuel->getX()<=m_arrivee->getX()+(2*depl_x))) && ((m_avion_actuel->getY()>=m_arrivee->getY()-(2*depl_y)) && (m_avion_actuel->getY()<=m_arrivee->getY()+(2*depl_y))))
            help=true;
    }
    // en haut � droite
    else if(m_arrivee->getX()>m_avion_actuel->getX() && m_arrivee->getY()<m_avion_actuel->getY())
    {
        if(((m_avion_actuel->getX()>=m_arrivee->getX()-(2*depl_x)) && (m_avion_actuel->getX()<=m_arrivee->getX()+(2*depl_x))) && ((m_avion_actuel->getY()>=m_arrivee->getY()+(2*depl_y)) && (m_avion_actuel->getY()<=m_arrivee->getY()-(2*depl_y))))
            help=true;
    }
    // en bas � gauche
    else if(m_arrivee->getX()<m_avion_actuel->getX() && m_arrivee->getY()>m_avion_actuel->getY())
    {
        if(((m_avion_actuel->getX()>=m_arrivee->getX()+(2*depl_x)) && (m_avion_actuel->getX()<=m_arrivee->getX()-(2*depl_x))) && ((m_avion_actuel->getY()>=m_arrivee->getY()-(2*depl_y)) && (m_avion_actuel->getY()<=m_arrivee->getY()+(2*depl_y))))
            help=true;
    }
    // en haut � gauche
    else if(m_arrivee->getX()<m_avion_actuel->getX() && m_arrivee->getY()<m_avion_actuel->getY())
    {
        if(((m_avion_actuel->getX()>=m_arrivee->getX()+(2*depl_x)) && (m_avion_actuel->getX()<=m_arrivee->getX()-(2*depl_x))) && ((m_avion_actuel->getY()>=m_arrivee->getY()+(2*depl_y)) && (m_avion_actuel->getY()<=m_arrivee->getY()-(2*depl_y))))
            help=true;
    }

    // si l'avion est dans le p�rim�tre d'atterrissage de son a�roport d'arriv�e
    if(help)
    {
        m_avion_actuel->setBoucleAttente(true);
        m_arrivee->ajoutAvionBoucleAttente(m_avion_actuel);
        m_avion_actuel->setCptUt(false);
    }
}

/// Lors de l'arriv�e de l'avion dans un a�roport, on le met dans la boucle d'attente avant son atterrissage
void Trajet::boucle()
{
    //std::cout<<"            BOUCLE D ATTENTE"<<std::endl;
    bool help_atterrissage=false;

    m_avion_actuel->setCapaciteCarburant(m_avion_actuel->getCapaciteCarburant()-(0.2*m_avion_actuel->getConso()));

    // si l'avion est le premier � devoir se poser dans la boucle d'attente
    if(m_arrivee->getBoucleAttente().front() == m_avion_actuel)
    {
        // on v�rifie s'il y a une station de libre aussi
        if((int)m_arrivee->getStations().size() < m_arrivee->getNbStations())
        {
            // si c'est le cas, on v�rifie s'il y a une piste d'atterrissage libre
            for(size_t i=(m_arrivee->getPistes().size()/2); i<m_arrivee->getPistes().size(); i++)
            {
                if(m_arrivee->getPistes()[i].empty())
                {
                    // si les trois conditions sont r�unies, l'avion peut atterrir
                    help_atterrissage=true;
                }
            }
        }

        // l'avion peut atterrir
        if(help_atterrissage)
        {
            // on l'enl�ve de la boucle d'attente
            m_avion_actuel->setBoucleAttente(false);
            m_arrivee->setBoucleAttente();

            // on lance l'atterrissage
            m_avion_actuel->setAtterrissage(true);
            m_arrivee->ajoutAvionPisteAtterrissage(m_avion_actuel);
        }
    }
}

/// Lorsqu'un nouveau trajet est cr��, l'avion doit se pr�parer � d�coller
void Trajet::attente(bool verif_station, bool verif_piste)
{
    //std::cout<<"            ATTENTE"<<std::endl;
    verif_station=false;
    verif_piste=false;

    // si l'avion est en temps de pr�paration dans une station
    for(size_t i=0; i<m_depart->getStations().size(); i++)
    {
        if(m_depart->getStations()[i] == m_avion_actuel)
        {
            // on incr�mente son tps d'attente d'une UT
            m_avion_actuel->setCptUt(true);
            verif_station=true;
        }
    }

    //si l'avion n'est pas dans la station, on v�rifie qu'il soit sur la piste
    if(!verif_station)
    {
        for(auto it : m_depart->getPistes())
        {
            if(it.front()==m_avion_actuel)
            {
                m_avion_actuel->setCptUt(true);
                verif_piste = true;
            }
        }
    }
    // si l'avion est dans une station, on v�rifie s'il peut en sortir
    else
    {
        // si son cpt d'UT a atteint la valeur de d�lai d'attente au sol de l'a�roport, on peut le d�placer vers les pistes
        if(m_avion_actuel->getCptUt()>=m_depart->getDelaiAttenteSol())
        {
            // remise � z�ro du compteur
            m_avion_actuel->setCptUt(false);

            // appel de la m�thode pour d�placer un avion d'une station � une piste
            m_depart->ajoutAvionPisteDecollage(m_avion_actuel);
        }
    }

    // si l'avion est sur la piste
    if(verif_piste)
    {
        // si son cpt d'UT a atteint la valeur de temps d'acc�s au piste, on peut le faire d�coller
        if(m_avion_actuel->getCptUt()>=m_depart->getTpsAccesPiste())
        {
            // remise � z�ro du compteur
            m_avion_actuel->setCptUt(false);
            m_avion_actuel->setDecollage(true);
            m_voler = true;

            calcul_horaires();
        }
    }
}

/// Apr�s le temps d'attente et de roulage vers une piste, l'avion peut d�coller
void Trajet::decollage()
{
    //std::cout<<"            DECOLLAGE"<<std::endl;
    int cpt=0;
    float diff_x=0, diff_y=0;
    float depl_x=0, depl_y=0;

    // on cherche sur quelle piste l'avion est pr�sent
    for(size_t i=0; i<m_depart->getPistes().size(); i++)
    {
        if(!m_depart->getPistes()[i].empty())
        {
            // on v�rifie qu'il soit le premier de la piste pour d�coller
            if(m_depart->getPistes()[i].front()==m_avion_actuel)
            {
                // on incr�mente son cpt d'UT (2 UT pour le d�collage)
                m_avion_actuel->setCptUt(true);
                cpt = i;
            }
        }
    }

    // calcul de la prochaine position de l'avion
    diff_x = m_arrivee->getX()-m_depart->getX();
    diff_y = m_arrivee->getY()-m_depart->getY();
    depl_x = (diff_x/(m_unite_tps[0]))/2.0;
    depl_y = (diff_y/(m_unite_tps[0]))/2.0;

    // d�termination de la prochaine position de l'avion en X et Y
    m_avion_actuel->setCoord(m_avion_actuel->getX()+depl_x, m_avion_actuel->getY()+depl_y);

    // si l'avion a bien effectu� ses 2 UT de d�collage, il vole dans son �tat "normal"
    if(m_avion_actuel->getCptUt()>=m_depart->getTpsDecollage())
    {
        // l'avion n'est plus en �tat de d�collage, mais en �tat de vol
        m_avion_actuel->setDecollage(false);
        m_avion_actuel->setCptUt(false);

        // l'avion n'est plus au niveau des pistes de l'a�roport, mais dans le ciel
        m_depart->setPistes(cpt);
    }
}

/// Si une station est libre, l'avion peut atterrir sur une piste r�serv�e
void Trajet::atterrissage(bool &suppr)
{
    //std::cout<<"            ATTERRISSAGE"<<std::endl;
    int cpt=0;
    float diff_x=0, diff_y=0;
    float depl_x=0, depl_y=0;
    bool verif_station=false, verif_piste=false;

    // on cherche sur quelle piste l'avion est en train d'atterrir
    for(size_t i=0; i<m_arrivee->getPistes().size(); i++)
    {
        if(!m_arrivee->getPistes()[i].empty())
        {
            // on v�rifie qu'il soit le premier de la piste pour atterrir
            if(m_arrivee->getPistes()[i].front()==m_avion_actuel)
            {
                // on incr�mente son cpt d'UT (2 UT pour l'atterrissage)
                m_avion_actuel->setCptUt(true);
                verif_piste = true;
                cpt = i;
            }
        }
    }

    //si l'avion n'est pas en train d'atterrir, on v�rifie qu'il soit en d�placement vers la station
    if(!verif_piste)
    {
        for(auto it : m_arrivee->getStations())
        {
            // on v�rifie qu'il soit le premier de la piste pour atterrir
            if(it==m_avion_actuel)
            {
                // on incr�mente son cpt d'UT (2 UT pour l'atterrissage)
                m_avion_actuel->setCptUt(true);
                verif_station = true;
            }
        }

    }
    // si l'avion est en train d'atterrir, on v�rifie s'il a fini
    else
    {
        // si l'avion a bien effectu� ses 2 UT d'atterrissage, il est sur la piste, pr�t � rejoindre une station
        if(m_avion_actuel->getCptUt()>=m_arrivee->getTpsDecollage())
        {
            m_avion_actuel->setCptUt(false);

            // il prend les coordonn�es en X et Y de l'a�roport d'arriv�e
            m_avion_actuel->setCoord(m_arrivee->getX(), m_arrivee->getY());

            // l'avion n'est plus au niveau des pistes de l'a�roport, mais dans une station
            m_arrivee->setStations(m_arrivee->getPistes()[cpt].front(), 0);
            m_arrivee->setPistes(cpt);
        }
        else
        {
            // calcul de la prochaine position de l'avion
            diff_x = m_arrivee->getX()-m_depart->getX();
            diff_y = m_arrivee->getY()-m_depart->getY();
            depl_x = (diff_x/(m_unite_tps[0]))/2.0;
            depl_y = (diff_y/(m_unite_tps[0]))/2.0;

            // d�termination de la prochaine position de l'avion en X et Y
            m_avion_actuel->setCoord(m_avion_actuel->getX()+depl_x, m_avion_actuel->getY()+depl_y);
        }
    }

    // si l'avion est en d�placement vers la station
    if(verif_station)
    {
        if(m_avion_actuel->getCptUt()>=m_arrivee->getTpsAccesPiste())
        {
            // si cet a�roport est sa destination finale
            if(m_arrivee->getNom() == m_chemin.back()->getNom())
            {
                // suppression du trajet de cet avion
                suppr=true;

                // on ajoute l'avion dans l'a�roport d'arriv�e
                m_avion_actuel->setAssignation(false);
                m_voler=false;
            }
            // si ce n'est pas le cas
            else
            {
                setNvllesDestinations();
            }

            // l'avion n'est plus en �tat d'atterrissage, mais au sol
            //capacit� du carburant est remise � 100%
            if(m_avion_actuel->getType()[0]=='C')
            {
                m_avion_actuel->setCapaciteCarburant(22800);
            }
            else if(m_avion_actuel->getType()[0]=='M')
            {
                m_avion_actuel->setCapaciteCarburant(75040);
            }
            else if(m_avion_actuel->getType()[0]=='L')
            {
                m_avion_actuel->setCapaciteCarburant(160000);
            }

            m_avion_actuel->setAtterrissage(false);
            m_avion_actuel->setCptUt(false);
        }
    }
}

/// Calcul de l'heure de d�part et d'arriv�e de chaque trajet
void Trajet::calcul_horaires()
{
    // date / heure actuelle bas�e sur le syst�me actuel
    time_t actuel = time(0);

    // r�cup�ration de l'heure
    tm *ltm = localtime(&actuel);

    m_heure_depart = ltm->tm_hour;
    m_minute_depart = ltm->tm_min;
    m_seconde_depart = ltm->tm_sec;

    float ut=0;

    // calcul du temps total du trajet en UT
    for(auto it : m_unite_tps)
    {
        ut = ut + it;
    }

    // s'il y a des escales, le temps doit �tre rallong�
    switch(m_unite_tps.size())
    {
    // 1 escale
    case 3:
        ut = ut + 18;
        break;

    // 2 escales
    case 4:
        ut = ut + 36;
        break;

    // 3 escales
    case 5:
        ut = ut + 53;
        break;
    }

    ut = ut*2;

    float tempo=0;

    ///si le temps d'un trajet est sup�rieur � 1 minute
    if(ut>=60)
    {
        ///s'il est �gal � 1min on incr�mente la minute
        if(ut==60)
        {
            ltm->tm_min=ltm->tm_min + 1;

        }
        ///s'il se situe entre 1 et 2 minutes
        else if(ut>60 && ut<120)
        {
            //on ajoute 1 min au compteur
            ltm->tm_min=ltm->tm_min + 1;
            //si le nb de sec restante et le nb de secondes actuelles font plus de 1 min
            if((ltm->tm_sec+(ut-60)>=60))
            {
                //on ajoute 1 min au compteur
                ltm->tm_min=ltm->tm_min+1;
                //on d�duit la minute des sec
                tempo=60-(ut-60);
                ltm->tm_sec=ltm->tm_sec-tempo;
            }
            else
                ltm->tm_sec = ltm->tm_sec + (ut-60);
        }
        ///s'il se situe entre 2 et 3 minutes
        else if(ut>=120 && ut<180)
        {
            //on ajoute 2 min au compteur
            ltm->tm_min=ltm->tm_min + 2;
            //si le nb de sec restante et le nb de secondes actuelles font plus de 1 min
            if((ltm->tm_sec+(ut-120)>=60))
            {
                ltm->tm_min = ltm->tm_min+1;
                ltm->tm_sec = ltm->tm_sec + (ut-120)-60;
            }
            else
                ltm->tm_sec =ltm->tm_sec + (ut-120);
        }
        ///s'il se situe entre 3 et 4 minutes
        else if(ut>=180 && ut<240)
        {
            //on ajoute 3 min au compteur
            ltm->tm_min=ltm->tm_min + 3;
            //si le nb de sec restante et le nb de secondes actuelles font plus de 1 min
            if((ltm->tm_sec+(ut-180)>=60))
            {
                ltm->tm_min=ltm->tm_min+1;
                tempo=60-(ut-180);
                ltm->tm_sec=ltm->tm_sec-tempo;
            }
            else
                ltm->tm_sec =ltm->tm_sec + (ut-180);
        }
        ///s'il se situe entre 4 et 5 minutes
        else if(ut>=240 && ut<=300)
        {
            //on ajoute 4 min au compteur
            ltm->tm_min=ltm->tm_min + 4;
            //si le nb de sec restante et le nb de secondes actuelles font plus de 1 min
            if((ltm->tm_sec+(ut-240)>=60))
            {
                ltm->tm_min=ltm->tm_min+1;
                tempo=60-(ut-240);
                ltm->tm_sec=ltm->tm_sec-tempo;
            }
            else
                ltm->tm_sec =ltm->tm_sec + (ut-240);
        }
        if(ltm->tm_min>59)
        {
            ltm->tm_hour=ltm->tm_hour+1;
            ltm->tm_min=ltm->tm_min-60;
        }
    }
    /// si il n'y a que quelques secondes
    else if(ut<60)
    {
        ///et que le nb de secondes + le nb de secondes qu'on rajoute est sup�rieur � 1 min
        if((ltm->tm_sec+ut)>=60)
        {
            ///il faut incr�menter d'une minute de plus
            ltm->tm_min=ltm->tm_min+1;
            ///et changer les secondes
            tempo=60-ut;
            ltm->tm_sec=ltm->tm_sec-tempo;
        }
        else
            ltm->tm_sec =ltm->tm_sec + ut;
    }

    m_heure_arrivee = ltm->tm_hour;
    m_minute_arrivee = ltm->tm_min;
    m_seconde_arrivee = ltm->tm_sec;
}
