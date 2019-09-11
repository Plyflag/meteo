#include "meteoowm.h"
#include "requeteowm.h"
#include <QCoreApplication>
#include <iostream>
#include <string>
#include <fstream>
#include <SNClientHTTP.h>


meteoOWM::meteoOWM()
{


}

void meteoOWM::rechercher(string v, string p)
{
    string req = requete.creerRequeteOWM();
    std::cout << req;


    SNClientHTTP clientHTTP;
    clientHTTP.connexionAuServeurParNomDeDomaine("api.openweathermap.org", 80);
    clientHTTP.envoyer(req);
    clientHTTP.recevoir();
    std::cout << clientHTTP.CorpsReponse();

    ofstream fichierMeteo;
    fichierMeteo.open("meteoCourante.xml");
    fichierMeteo << (clientHTTP.CorpsReponse());
    fichierMeteo.close();

}
