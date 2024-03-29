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
    //cout << reponseXML << endl;

    istringstream(extraireContenuEntreBalise("temperature", 0, "max")) >> Actuellement.tempmax;
    cout << "La temperature maximale est actuellement de : " << Actuellement.tempmax << " degres Celsuis" << endl;
    istringstream(extraireContenuEntreBalise("temperature", 0, "min")) >> Actuellement.tempmin;
    cout << "La temperature minimale est actuellement de : " << Actuellement.tempmin << " degres Celsuis" << endl;
    istringstream(extraireContenuEntreBalise("temperature", 0, "value")) >> Actuellement.temp;
    cout << "La temperature est actuellement de : " << Actuellement.temp << " degres Celsuis \n\r" << endl;

     int pos = reponseXML.find("forecast", 0);
      pos = reponseXML.find("<time", pos);
    for(int i = 0; i < 4; i++ )
    {
        pos = reponseXML.find("<time", pos + 1);
        istringstream(extraireContenuEntreBalise("temperature", pos, "max")) >> previsions[i].tempmax;
        cout << "La temperature maximale prevue est de : "<< previsions[i].tempmax << " degres Celsuis " << endl;
        istringstream(extraireContenuEntreBalise("temperature", pos, "min")) >> previsions[i].tempmin;
        cout << "La temperature minimale prevue est de : " << previsions[i].tempmin << " degres Celsuis \r\n" << endl;

        previsions[i].date=extraireContenuEntreBalise("time", pos, "to") ;
        //cout << "La date du : "<< previsions[i].date <<  endl;

       // cout<<"Il est "<<DetailHeure(previsions[i].date);
       // cout <<  "l'année est : " << DetailHeure(previsions[i].date, annee);

        cout << DetailHeure(reponseXML);
    }


    ofstream fichierMeteo;
    fichierMeteo.open("meteoAvenir.xml");
    fichierMeteo << reponseXML;
    fichierMeteo.close();

}

string meteoOWM::extraireContenuEntreBalise(string balise, int positionDepart, string baliseRecherche)
{

    int positionBaliseDepart=reponseXML.find(balise, positionDepart);
    int positionBaliseFinale=reponseXML.find("<time"+ balise, positionBaliseDepart);
    string chaineBalise=reponseXML.substr(positionBaliseDepart + balise.length(), positionBaliseFinale - positionBaliseDepart - baliseRecherche.length());

    baliseRecherche += "=\"";
    positionBaliseDepart=chaineBalise.find(baliseRecherche, 0);
    positionBaliseFinale=chaineBalise.find("\"", positionBaliseDepart+baliseRecherche.length()+ 1);
    string chaineMot= chaineBalise.substr(positionBaliseDepart+baliseRecherche.length(), positionBaliseFinale-positionBaliseDepart-baliseRecherche.length());

    return chaineMot;
    //cout << chaineBalise;
}

string meteoOWM::DetailHeure(string dateMeteo)
{
    string balise = dateMeteo.substr(0, 19);
    string jour = balise.substr(dateMeteo.find("set", 0));
    string annee = balise.substr(0, 4);
    string mois = balise.substr(5, 7);

   // heure=dateMeteo.substr(dateMeteo.find("T", 0)+1,2)+"h";
  //   annee=dateMeteo.substr(dateMeteo.find("set", posi)+1,4)+"h";
string TrameFinale = jour + "/" +  mois + "/" + annee;
    return TrameFinale;


}
