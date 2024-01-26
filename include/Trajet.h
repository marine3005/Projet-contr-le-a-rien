#ifndef TRAJET_H
#define TRAJET_H

#include "Aeroport.h"
#include "Graphe_AS.h"

class Trajet
{
private:
    Aeroport* m_depart;
    Aeroport* m_arrivee;
    Avion* m_avion_actuel;
    std::vector<Aeroport*> m_chemin;
    std::vector<float> m_nb_km;
    std::vector<float> m_unite_tps;
    bool m_voler;
    int m_cpt_escale;
    int m_heure_arrivee;
    int m_heure_depart;
    int m_minute_arrivee;
    int m_minute_depart;
    int m_seconde_arrivee;
    int m_seconde_depart;
    float m_cpt_ut;
    int m_altitude;
    std::vector<Trajet*> m_successeurs_WP;
    std::vector<Coordonnee*> m_coord_astar;

public:
    Trajet(std::vector<float> _nb_km, std::vector<Aeroport*> chemin, Avion* _avion);
    ~Trajet();

    float getAvionX();
    float getAvionY();
    int getDepartX();
    int getDepartY();
    int getArriveeX();
    int getArriveeY();
    BITMAP* getBmp();
    bool getVoler();
    bool getAvionDecollage();
    bool getAvionAtterrissage();
    bool getAvionBoucleAttente();
    float getAvionCapaCarburant();
    Aeroport* getArrivee();
    Aeroport* getDepart();
    std::string getArrivee1();
    std::string getDepart1();
    bool getAvionAssignation();
    void setNvllesDestinations();
    int getTailleTrajet();
    std::string getNomEscale(int place);
    int getHeureArrivee();
    int getHeureDepart();
    int getMinuteArrivee();
    int getMinuteDepart();
    int getSecondeArrivee();
    int getSecondeDepart();
    int getAltitude();
    void setAltitude(int nv_altitude);
    std::vector<Trajet*> getSuccesseurs_WP ();
    void setSuccesseurs_WP (Trajet* traj);

    void calcul_horaires();

    int getCptUt();
    void setCptUt(bool cpt);

    void setCoordAstar(std::vector<Coordonnee*> coord_astar);
    std::vector<Coordonnee*> getCoordAstar();

    void deplacement_astar(std::map<std::pair<float, float>, std::pair<float, float>> equivalence);

    void afficher();
    void deplacement();
    void attente(bool verif_station, bool verif_piste);
    void decollage();
    void atterrissage(bool& suppr);
    void boucle();
};

#endif // TRAJET_H
