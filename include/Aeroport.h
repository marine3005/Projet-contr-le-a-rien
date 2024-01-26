#ifndef AEROPORT_H
#define AEROPORT_H

#include "Avion.h"

class Aeroport
{
private:
    std::string m_nom;
    float m_coordonnees_x;
    float m_coordonnees_y;
    int m_nb_piste;
    std::vector<std::queue<Avion*>> m_pistes;
    int m_nb_station;
    std::vector<Avion*> m_stations;
    int m_delai_attente_sol;
    int m_temps_acces_piste;
    int m_delai_anticollision;
    int m_temps_decollage;
    int m_duree_boucle_attente;
    std::vector<Avion*> m_boucle_attente;
    std::map<char,float> m_distance_aeroport;

public:
    Aeroport(std::string nom_aeroport);
    ~Aeroport();

    char getPremiereLettre() const;
    std::string getNom();
    int getNbStations() const;
    std::vector<Avion*> getStations();
    std::vector<std::queue<Avion*>> getPistes();
    std::map<char,float> getDistanceAeroport();
    std::vector<Avion*> getBoucleAttente();
    Avion* getAvion();
    float getX();
    float getY();
    int getDelaiAttenteSol();
    int getTpsAccesPiste();
    int getDelaiAnticollision();
    int getTpsDecollage();
    int getDureeBoucleAttente();

    void setStations(Avion* a_rajouter, int place);
    void setPistes(int place);
    void setBoucleAttente();

    void afficher();
    void ajoutAvionPisteDecollage(Avion* avion_a_deplacer);
    void ajoutAvionPisteAtterrissage(Avion* avion_a_atterrir);
    void ajoutAvionBoucleAttente(Avion* avion_en_attente);
};

#endif // AEROPORT_H
