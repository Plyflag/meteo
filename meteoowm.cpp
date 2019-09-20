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
    cout << req;
    string req2 = requete.creerRequeteOWMFutur();
    cout << req2;

    clientOWM.connexionAuServeurParNomDeDomaine("api.openweathermap.org", 80);
    //clientOWM.envoyer(req);
    clientOWM.envoyer(req2);
    clientOWM.recevoir();
    reponseXML = clientOWM.CorpsReponse();
    cout << reponseXML << endl;

    istringstream(extraireContenuEntreBalise("temperature", 0, "max")) >> Actuellement.tempmax;
    cout << "La temperature maximale est actuellement de : " << Actuellement.tempmax << " degres Celsuis" << endl;
    istringstream(extraireContenuEntreBalise("temperature", 0, "min")) >> Actuellement.tempmin;
    cout << "La temperature minimale est actuellement de : " << Actuellement.tempmin << " degres Celsuis" << endl;
    istringstream(extraireContenuEntreBalise("temperature", 0, "value")) >> Actuellement.temp;
    cout << "La temperature est actuellement de : " << Actuellement.temp << " degres Celsuis \n\r" << endl;


    for(int i = 0; i < 4; i++)
    {
        //previsions[i].tempmax = istringstream(extraireContenuEntreBalise("temperature", i, "max"));
        cout << "La temperature maximale est actuellement de : " << previsions[i].tempmax << " degres Celsuis" << endl;
        istringstream(extraireContenuEntreBalise("temperature", i, "min")) >> previsions[i].tempmin;
        cout << "La temperature minimale est actuellement de : " << previsions[i].tempmin << " degres Celsuis" << endl;
        istringstream(extraireContenuEntreBalise("temperature", i, "value")) >> previsions[i].temp;
        cout << "La temperature est actuellement de : " << previsions[i].temp << " degres Celsuis \r\n" << endl;
    }


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
