#include <QCoreApplication>
#include "requeteowm.h"
#include <iostream>
#include <string>
#include <fstream>
#include <SNClientHTTP.h>

int main(int argc, char *argv[])
{
    string ville;
    string pays;
    string text;
    string reponseXML;
    requeteOWM requete;
    QCoreApplication a(argc, argv);

    std::cout << "Choisir le pays" << std::endl;
    cin >> pays;
    requete.modifierPays(pays);

    std::cout << "Choisir une ville" << std::endl;
    cin >> ville;
    requete.modifierVille(ville);

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
    return a.exec();
}
