#include "Avion.h"

/// Constructeur
Avion::Avion(std::string type_avion, char aero, int nb, const char* bmp_avion, float _x, float _y)
    :m_x(_x), m_y(_y)
{
    std::ifstream ifs(type_avion);

    if (!ifs)
        throw std::runtime_error( "Impossible d'ouvrir en lecture " );

    ifs>>m_type_avion>>m_conso>>m_capacite_carburant;

    // création du matricule (1ère lettre de l'aéroport de départ + numéro entre 1 et 9)
    std::string matricule(1, aero);
    matricule += std::to_string(nb);
    m_matricule = matricule;

    m_img = load_bitmap(bmp_avion, NULL);

    m_cpt_ut = 0;

    m_assignation = false;

    m_decollage = false;
    m_atterrissage = false;
    m_boucle_attente = false;
}

/// Destructeur
Avion::~Avion()
{
}

/// Getters et setters
std::string Avion::getType()
{
    return m_type_avion;
}

std::string Avion::getMatricule()
{
    return m_matricule;
}

float Avion::getX()
{
    return m_x;
}

float Avion::getY()
{
    return m_y;
}

BITMAP* Avion::getBmp()
{
    return m_img;
}

int Avion::getCptUt()
{
    return m_cpt_ut;
}

void Avion::setCptUt(bool cpt)
{
    if(!cpt)
        m_cpt_ut=0;
    else
        m_cpt_ut++;
}

float Avion::getCapaciteCarburant()
{
    return m_capacite_carburant;
}

void Avion::setCapaciteCarburant(float capa_carburant)
{
    m_capacite_carburant = capa_carburant;
}

int Avion::getConso()
{
    return m_conso;
}

void Avion::setCoord(float x, float y)
{
    m_x = x;
    m_y = y;
}

bool Avion::getAssignation()
{
    return m_assignation;
}

void Avion::setAssignation(bool assignation)
{
    m_assignation = assignation;
}

bool Avion::getDecollage()
{
    return m_decollage;
}

void Avion::setDecollage(bool decollage)
{
    m_decollage = decollage;
}

bool Avion::getAtterrissage()
{
    return m_atterrissage;
}

void Avion::setAtterrissage(bool atterrissage)
{
    m_atterrissage = atterrissage;
}

bool Avion::getBoucleAttente()
{
    return m_boucle_attente;
}

void Avion::setBoucleAttente(bool boucle_attente)
{
    m_boucle_attente = boucle_attente;
}

/// Méthode d'affichage
void Avion::afficher()
{
    std::cout<<" Avion "<<m_type_avion<<" "<<m_matricule<<std::endl;
    std::cout<<"Conso = "<<m_conso<<std::endl;
    std::cout<<"Capacite carburant = "<<m_capacite_carburant<<std::endl;
    std::cout<<"X = "<<m_x<<" Y = "<<m_y<<std::endl;
}
