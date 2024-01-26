#include "Coordonnee.h"

/// Constructeur
Coordonnee::Coordonnee(float x,float y)
{
    m_coordonnee_x=x;
    m_coordonnee_y=y;
}

/// Destructeur
Coordonnee::~Coordonnee()
{

}

/// Getters et setters
float Coordonnee::getX()
{
    return m_coordonnee_x;
}

float Coordonnee::getY()
{
    return m_coordonnee_y;
}

void Coordonnee::setX(float x)
{
    m_coordonnee_x = x;
}

void Coordonnee::setY(float y)
{
    m_coordonnee_y = y;
}

void Coordonnee::ajouterSucc(Coordonnee * s, float n)
{
    m_successeurs.insert({s,n});
}

std::map<Coordonnee *,float> Coordonnee::getSuccesseur()
{
    return m_successeurs;
}
