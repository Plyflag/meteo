#ifndef METEOOWM_H
#define METEOOWM_H
#include <iostream>
#include <string>
#include <fstream>
#include <SNClientHTTP.h>
#include "requeteowm.h"

class meteoOWM
{
private :
    requeteOWM requete;
    SNClientHTTP clientOWM;
    string reponseXML;
    ofstream fichierMeteo;
public:
    meteoOWM();
    void rechercher(string v, string p);
};

#endif // METEOOWM_H
