#include "requeteowm.h"
#include "meteoowm.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <SNClientHTTP.h>
using namespace std;

string reponseXML;

int main()
{

    string text;
    string ville;
    string pays;


    requeteOWM requete;
    meteoOWM meteo;

    cout << "Choisir la ville" << endl;
    cin >> ville;
    //ville = "Paris";

   cout << "Choisir le pays" << endl;
    cin >> pays;
   //pays = "France";

    meteo.rechercher(ville, pays);
//    meteo.extraireContenuEntreBalise("temperature", 0);
//    cout << reponseXML;

    return 0;
}

