#ifndef METEOOWM_H
#define METEOOWM_H
#include <iostream>
#include <string>
#include <fstream>
#include <SNClientHTTP.h>
#include "requeteowm.h"

typedef struct {
    float temp;
    float tempmax;
    float tempmin;
    string ville;
    string description;
    string urlicone;
}ConditionsActuelles;

class meteoOWM
{
private :
    requeteOWM requete;
    SNClientHTTP clientOWM;
    string reponseXML;
    ofstream fichierMeteo;
    ConditionsActuelles Actuellement;
public:
    meteoOWM();
    void rechercher(string v, string p);
    string extraireContenuEntreBalise(string balise, int positionDepart, string baliseRecherche);
};

#endif // METEOOWM_H
