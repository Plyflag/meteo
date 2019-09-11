#include <QCoreApplication>
#include "requeteowm.h"
#include "meteoowm.h"
#include <iostream>
#include <string>
#include <fstream>
#include <SNClientHTTP.h>

int main(int argc, char *argv[])
{
    string v;
    string p;
    string text;
    string reponseXML;
    requeteOWM requete;
    meteoOWM meteo;
    QCoreApplication a(argc, argv);

    std::cout << "Choisir la ville" << std::endl;
    cin >> pays;
    requete.modifierPays(pays);

    std::cout << "Choisir le pays" << std::endl;
    cin >> ville;
    requete.modifierVille(ville);

    string req = requete.creerRequeteOWM();
    std::cout << req;

    meteo.rechercher(v, p);
    return a.exec();

}
