#ifndef AVION_H
#define AVION_H

#include <iostream>
#include <map>
#include <queue>
#include <vector>
#include <algorithm>
#include <fstream>
#include <time.h>
#include <allegro.h>
#include <ctime>
#include <cmath>

class Avion
{
private:
    std::string m_matricule;
    std::string m_type_avion;
    float m_capacite_carburant;
    int m_conso;
    BITMAP *m_img;
    float m_x;
    float m_y;
    int m_cpt_ut;
    bool m_assignation;
    bool m_decollage;
    bool m_atterrissage;
    bool m_boucle_attente;

public:
    Avion(std::string type_avion, char aero, int nb, const char* bmp_avion, float _x, float _y);
    ~Avion();

    std::string getType();
    std::string getMatricule();
    float getX();
    float getY();
    BITMAP* getBmp();
    void setCoord(float x, float y);
    int getCptUt();
    void setCptUt(bool cpt);
    float getCapaciteCarburant();
    void setCapaciteCarburant(float capa_carburant);
    int getConso();
    bool getAssignation();
    void setAssignation(bool assignation);
    bool getDecollage();
    void setDecollage(bool decollage);
    bool getAtterrissage();
    void setAtterrissage(bool atterrissage);
    bool getBoucleAttente();
    void setBoucleAttente(bool boucle_attente);


    void afficher();
};

#endif // AVION_H
