#ifndef COORDONNEE_H
#define COORDONNEE_H

#include <iostream>
#include <map>
#include <queue>
#include <vector>
#include <algorithm>
#include <fstream>
#include <time.h>
#include <allegro.h>
#include <cmath>

class Coordonnee
{
private:
    float m_coordonnee_x;
    float m_coordonnee_y;
    std::map<Coordonnee*,float> m_successeurs;

public:
    Coordonnee(float x,float y);
    ~Coordonnee();
    float getX();
    float getY();
    void setX(float x);
    void setY(float y);
    void ajouterSucc(Coordonnee* s, float n);
    std::map<Coordonnee*,float> getSuccesseur();


};

#endif // COORDONNEE_H
