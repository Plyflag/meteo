#include "meteoowm.h"
#include <sstream>
#include <iostream>
#include <string>



meteoOWM::meteoOWM()
{
}

void meteoOWM::rechercher(string v, string p)
{

    requete.modifierVille(v);
    requete.modifierPays(p);


    string req = requete.creerRequeteOWM();
    std::cout << req;



    clientOWM.connexionAuServeurParNomDeDomaine("api.openweathermap.org", 80);
    clientOWM.envoyer(req);
    clientOWM.recevoir();
    reponseXML = clientOWM.CorpsReponse();
    cout << reponseXML << endl;

    istringstream(extraireContenuEntreBalise("temperature", 0, "max")) >> Actuellement.tempmax;
    cout << "La temperature maximale est actuellement de : " << Actuellement.tempmax << " degres Celsuis" << endl;
    istringstream(extraireContenuEntreBalise("temperature", 0, "min")) >> Actuellement.tempmin;
    cout << "La temperature minimale est actuellement de : " << Actuellement.tempmin << " degres Celsuis" << endl;
    istringstream(extraireContenuEntreBalise("temperature", 0, "value")) >> Actuellement.temp;
    cout << "La temperature est actuellement de : " << Actuellement.temp << " degres Celsuis" << endl;


    ofstream fichierMeteo;
    fichierMeteo.open("meteoCourante.xml");
    fichierMeteo << (clientOWM.CorpsReponse());
    fichierMeteo.close();

}

string meteoOWM::extraireContenuEntreBalise(string balise, int positionDepart, string baliseRecherche)
{

    int positionBaliseDepart=reponseXML.find(balise, positionDepart);
    int positionBaliseFinale=reponseXML.find("</"+ balise, positionBaliseDepart);
    string chaineBalise=reponseXML.substr(positionBaliseDepart + balise.length(), positionBaliseFinale - positionBaliseDepart - baliseRecherche.length());

    baliseRecherche += "=\"";
    positionBaliseDepart=chaineBalise.find(baliseRecherche, 0);
    positionBaliseFinale=chaineBalise.find("\"", positionBaliseDepart+baliseRecherche.length()+ 1);
    string chaineMot= chaineBalise.substr(positionBaliseDepart+baliseRecherche.length(), positionBaliseFinale-positionBaliseDepart-baliseRecherche.length());

    return chaineMot;
    //cout << chaineBalise;
}
