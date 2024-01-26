#include "Aeroport.h"

/// Constructeur
Aeroport::Aeroport(std::string nom_aeroport)
{
    std::string ligne;
    srand(time(NULL));
    int type, cpt=1;

    std::ifstream ifs(nom_aeroport);
    if (!ifs)
        throw std::runtime_error( "Impossible d'ouvrir en lecture " + nom_aeroport );

    char nom_v;
    int dist;

    getline(ifs, ligne);
    m_nom = ligne;

    ifs >> m_coordonnees_x;
    ifs >> m_coordonnees_y;
    ifs >> m_nb_piste;
    for(int j=0; j<m_nb_piste; j++)
    {
        std::queue<Avion*> piste;
        m_pistes.push_back(piste);
    }
    ifs >> m_nb_station;
    for(int i=0; i<m_nb_station; i++)
    {
        if(m_nom=="Atlanta" || m_nom=="Rio" || m_nom=="Melbourne")
        {
            type = rand()%(2-1+1)+1;
            if(type==1)
                m_stations.push_back(new Avion{"infos/Moyen.txt", getPremiereLettre(), cpt, "bmp/Moyen.bmp", getX(), getY()});
            else if(type==2)
                m_stations.push_back(new Avion{"infos/Long.txt", getPremiereLettre(), cpt, "bmp/Long.bmp", getX(), getY()});
        }
        else
        {
            type = rand()%(3-1+1)+1;
            if(type==1)
                m_stations.push_back(new Avion{"infos/Court.txt", getPremiereLettre(), cpt, "bmp/Court.bmp", getX(), getY()});
            else if(type==2)
                m_stations.push_back(new Avion{"infos/Moyen.txt", getPremiereLettre(), cpt, "bmp/Moyen.bmp", getX(), getY()});
            else if(type==3)
                m_stations.push_back(new Avion{"infos/Long.txt", getPremiereLettre(), cpt, "bmp/Long.bmp", getX(), getY()});
        }
        cpt++;
    }
    ifs >> m_delai_attente_sol;
    ifs >> m_temps_acces_piste;
    ifs >> m_delai_anticollision;
    ifs >> m_temps_decollage;
    ifs >> m_duree_boucle_attente;

    for(int j=0; j<8; j++)
    {
        ifs>>nom_v>>dist;
        m_distance_aeroport.insert({nom_v,dist});
    }
}


/// Destructeur
Aeroport::~Aeroport()
{
}

/// Getters et setters
char Aeroport::getPremiereLettre() const
{
    return m_nom[0];
}

std::string Aeroport::getNom()
{
    return m_nom;
}

int Aeroport::getNbStations() const
{
    return m_nb_station;
}

std::vector<Avion*> Aeroport::getStations()
{
    return m_stations;
}

std::vector<std::queue<Avion*>> Aeroport::getPistes()
{
    return m_pistes;
}

std::vector<Avion*> Aeroport::getBoucleAttente()
{
    return m_boucle_attente;
}

Avion* Aeroport::getAvion()
{
    return m_stations.front();
}

std::map<char,float> Aeroport::getDistanceAeroport()
{
    return m_distance_aeroport;
}

float Aeroport::getX()
{
    return m_coordonnees_x;
}

float Aeroport::getY()
{
    return m_coordonnees_y;
}

int Aeroport::getDelaiAttenteSol()
{
    return m_delai_attente_sol;
}

int Aeroport::getTpsAccesPiste()
{
    return m_temps_acces_piste;
}

int Aeroport::getDelaiAnticollision()
{
    return m_delai_anticollision;
}

int Aeroport::getTpsDecollage()
{
    return m_temps_decollage;
}

int Aeroport::getDureeBoucleAttente()
{
    return m_duree_boucle_attente;
}

/// Méthode de modification des places dans les stations
void Aeroport::setStations(Avion* a_rajouter, int place)
{
    // si c'est un avion qui quitte l'aéroport, on envoie NULL pour pouvoir supprimer cet avion
    if(a_rajouter==NULL)
    {
        m_stations.erase(m_stations.begin() + place);
    }
    // sinon si c'est un avion qui atterrit dans l'aéroport, on le rajoute juste à la file de stations
    else
    {
        m_stations.push_back(a_rajouter);
    }
}

/// Méthode d'affichage
void Aeroport::afficher()
{
    std::cout<<"    Aeroport : "<<m_nom<<std::endl;
    std::cout<<"x = "<<m_coordonnees_x<<"   y = "<<m_coordonnees_y<<std::endl;
    std::cout<<"Nb de pistes = "<<m_nb_piste<<std::endl;
    std::cout<<"Nb de stations = "<<m_nb_station<<std::endl;
    std::cout<<"Nb de delai d attente au sol = "<<m_delai_attente_sol<<std::endl;
    std::cout<<"Nb de tps acces piste = "<<m_temps_acces_piste<<std::endl;
    std::cout<<"Nb de delai anticollision = "<<m_delai_anticollision<<std::endl;
    std::cout<<"Nb de tps decollage = "<<m_temps_decollage<<std::endl;
    std::cout<<"Nb de duree boucle d attente = "<<m_duree_boucle_attente<<std::endl;

    for(auto it : m_distance_aeroport)
    {
        std::cout<<it.first<<" => "<<it.second<<std::endl;
    }
    std::cout<<"STATIONS"<<std::endl;
    for(size_t i=0; i<m_stations.size(); i++)
    {
        m_stations[i]->afficher();
    }
    std::cout<<"PISTES"<<std::endl;
    for(size_t i=0; i<m_pistes.size(); i++)
    {
        if(!m_pistes[i].empty())
        {
            std::queue<Avion*> file = m_pistes[i];
            std::cout<<"numero "<<i<<std::endl;
            for(size_t j=0; j<m_pistes[i].size(); j++)
            {
                file.front()->afficher();
                file.pop();
            }
        }
    }

    std::cout<<std::endl;
}

void Aeroport::ajoutAvionPisteDecollage(Avion* avion_a_deplacer)
{
    int place;

    for(size_t i=0; i<m_stations.size(); i++)
    {
        if(avion_a_deplacer==m_stations[i])
            place = i;
    }

    // première moitié de pistes : pistes de décollage
    int pistes_decollage = m_nb_piste/2;

    // 1 piste de décollage
    if(pistes_decollage==1)
    {
        // ajout de l'avion dans une file piste pour qu'il attende ses UT d'accès aux pistes
        m_pistes[0].push(m_stations[place]);

        // retrait de l'avion des stations
        setStations(NULL, place);
    }
    // 2 pistes de décollage
    else if(pistes_decollage==2)
    {
        // comparaison savoir quelle piste contient moins d'avions en attente
        if(m_pistes[0].size()<=m_pistes[1].size())
        {
            m_pistes[0].push(m_stations[place]);
        }
        else
        {
            m_pistes[1].push(m_stations[place]);
        }

        setStations(NULL, place);
    }
    // 3 pistes de décollage
    else if(pistes_decollage==3)
    {
        // comparaison savoir quelle piste contient moins d'avions en attente
        if(m_pistes[0].size()<=m_pistes[2].size())
        {
            if(m_pistes[0].size()<=m_pistes[1].size())
            {
                m_pistes[0].push(m_stations[place]);
            }
            else if(m_pistes[0].size()>m_pistes[1].size())
            {
                m_pistes[1].push(m_stations[place]);
            }
        }
        else
        {
            m_pistes[2].push(m_stations[place]);
        }

        setStations(NULL, place);
    }
}

void Aeroport::ajoutAvionPisteAtterrissage(Avion* avion_a_atterrir)
{
    // première moitié de pistes : pistes de décollage
    int pistes_atterrissage = m_nb_piste/2;

    // 1 piste de décollage
    if(pistes_atterrissage==1)
    {
        // ajout de l'avion dans une file piste pour qu'il attende ses UT d'accès aux pistes
        m_pistes[1].push(avion_a_atterrir);
    }
    // 2 pistes de décollage
    else if(pistes_atterrissage==2)
    {
        // comparaison savoir quelle piste contient moins d'avions en attente
        if(m_pistes[2].empty())
        {
            m_pistes[2].push(avion_a_atterrir);
        }
        else
        {
            m_pistes[3].push(avion_a_atterrir);
        }
    }
    // 3 pistes de décollage
    else if(pistes_atterrissage==3)
    {
        // comparaison savoir quelle piste contient moins d'avions en attente
        if(m_pistes[3].empty())
        {
            m_pistes[3].push(avion_a_atterrir);
        }
        else if(m_pistes[4].empty())
        {
            m_pistes[4].push(avion_a_atterrir);
        }
        else
        {
            m_pistes[5].push(avion_a_atterrir);
        }
    }
}

void Aeroport::ajoutAvionBoucleAttente(Avion* avion_en_attente)
{
    m_boucle_attente.push_back(avion_en_attente);

    Avion* var_temp = NULL;
    Avion* tempo = NULL;
    int j=0;

    if(!m_boucle_attente.empty())
    {

        /// Tri par insertion
        for(size_t i=0; i<m_boucle_attente.size(); i++)
        {
            var_temp = m_boucle_attente[i];
            j = i;

            while(j>0 && (m_boucle_attente[j-1]->getCapaciteCarburant() > var_temp->getCapaciteCarburant()))
            {
                tempo = m_boucle_attente[j-1];
                m_boucle_attente[j-1] = m_boucle_attente[j];
                m_boucle_attente[j] = tempo;
                j = j-1;
            }
            m_boucle_attente[j] = var_temp;
        }
    }
}

void Aeroport::setPistes(int place)
{
    m_pistes[place].pop();
}

void Aeroport::setBoucleAttente()
{
    m_boucle_attente.erase(m_boucle_attente.begin());
}
