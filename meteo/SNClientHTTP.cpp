#include "SNClientHTTP.h"



#ifdef _WIN32 || _WIN64
#include <winsock2.h>
#include <sys/types.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;



int SNClientHTTP::nbClientTCP = 0;

SNClientHTTP::SNClientHTTP()
{
    if(nbClientTCP == 0)
    {
        //cout << "Bonjour" << endl;
#ifdef _WIN32 || _WIN64
        WSADATA wsaData ;
        int iResult;

        //cout << "Bonjour 2 " << endl;

        iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if(iResult != 0)
        {
            //cout << "Initialisation de la DLL Winsock : ERROR." << endl;
        }
        //cout << "Initialisation de la DLL Winsock : OK." << endl;
#endif

    }

    nbClientTCP++;
    corpsReponse = NULL;
    bloc = NULL;
}

SNClientHTTP::~SNClientHTTP()
{
    if(corpsReponse != NULL)
    {
        delete [] corpsReponse;
        corpsReponse = NULL;
        //cout << "Suppression de corpsReponse du TAS." << endl;
    }
    if(bloc != NULL)
    {
        delete [] bloc;
        bloc = NULL;
        //cout << "Suppression de bloc du TAS." << endl;
    }
}

bool SNClientHTTP::connexionAuServeurParNomDeDomaine(string nomDeDomaine, unsigned short portServeur)
{
    // Création de la socket TCP
    m_maSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(m_maSocket == -1)
    {
        //cout << "Creation de la socket : ERREUR" << endl;
        return false;
    }

    // Recherche de l'adresse IP du serveur de l'API
    //char * hostName = "api.wunderground.com";
    struct hostent * host = NULL;
    host = gethostbyname(nomDeDomaine.c_str());
    //cout<<WSAGetLastError();
    if(host == NULL)
    {
        //cout << "Nom de domaine : INTROUVABLE." << endl;
        return false;
    }
    //cout << "Nom de domaine : OK." << endl;

    m_portServeur = portServeur;

    // Connexion au serveur
    struct sockaddr_in serveurAJoindre;
    serveurAJoindre.sin_family = AF_INET;
    serveurAJoindre.sin_addr.s_addr = *(u_long *) host->h_addr;
    serveurAJoindre.sin_port = htons(m_portServeur);
    int resultat = connect(m_maSocket, (const struct sockaddr *) &serveurAJoindre, sizeof(serveurAJoindre));
    if(resultat != 0)
    {
        //cout << "Connexion au serveur : ERREUR." << endl;
        return false;
    }
    //cout << "Connexion au serveur : OK" << endl;

    return true;

}


bool SNClientHTTP::connexionAuServeurParAdresseIP(string adresseIPServeur, unsigned short portServeur)
{
    // Création de la socket TCP
    m_maSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(m_maSocket == -1)
    {
        cout << "Creation de la socket : :ERREUR." << endl;
        return false;
    }

    m_adresseIPServeur = adresseIPServeur;
    m_portServeur = portServeur;

    // Connexion au serveur
    struct sockaddr_in serveurAJoindre;
    serveurAJoindre.sin_family = AF_INET;
    serveurAJoindre.sin_addr.s_addr = inet_addr(m_adresseIPServeur.c_str());
    serveurAJoindre.sin_port = htons(m_portServeur);
    int resultat = connect(m_maSocket, (const struct sockaddr *) &serveurAJoindre, sizeof(serveurAJoindre));
    if(resultat != 0)
    {
        cout << "Connexion au serveur : ERREUR." << endl;
        return false;
    }

    return true;

}


bool SNClientHTTP::envoyer(string requete)
{
    int resultat = send(m_maSocket, requete.c_str(), requete.length(), 0);    
    //if(resultat == SOCKET_ERROR)
    if(resultat == -1)
    {
        cout << "Envoi du message : ERREUR." << endl;
        return false;
    }
    return true;
}


void SNClientHTTP::recevoir()
{
    enteteReponse = "";
    corpsReponse = "";

    string messageRecu = "";
    int nbOctetsLus;
    int nbOctetsLusTotal = 0;
    int nbOctetsAttendus = 0;
    char buffer[1500];  // Lecture par bloc de 1500 octets
    contentLength = 0;
    int positionFinEntete = string::npos;
    do
    {
        nbOctetsLus = recv(m_maSocket, buffer, 1500, 0);
        //memcpy(reponse+nbOctetsLusTotal, buffer, nbOctetsLus);
        if(nbOctetsLus > 0)
        {
            enteteReponse += string(buffer, nbOctetsLus);
            positionFinEntete = enteteReponse.find("\r\n\r\n");
            nbOctetsLusTotal += nbOctetsLus;
        }
    }while( (positionFinEntete == string::npos) );

    //cout << "Position fin entete : " << positionFinEntete << endl;
    //cout << "Nb Octets Lus : " << nbOctetsLusTotal << endl;


    // fin de réception de l'entete
    if(enteteReponse.find("Content-Length: ") != string::npos)
    {
        /* EXTRACTION DE CONTENT-LENGTH*/
        //cout << "Utilisation de Content-Length..." << endl;
        contentLength = 0;
        typeReponse = 1;
        string chaineATrouver = "Content-Length: ";
        int pos2 = enteteReponse.find(chaineATrouver);
        int pos3 = enteteReponse.find("\r\n", pos2);
        //cout << "Pos3 : " << pos3 << endl;
        if(pos3 <= 0 ) {cout << "Erreur 2 lors de la recherche de Content-Length." << endl; return ;}
        if (pos3 <= pos2+chaineATrouver.length()) {cout << "Erreur 3 lors de la recherche de Content-Length." << endl; return ;}
        for(int i=pos2+chaineATrouver.length(); i<pos3; i++)
        {
            //cout << i << ":" << strBuffer[i] << endl;
            contentLength = contentLength*10 + (int)(enteteReponse[i]-'0');
        }
        /* RESERVATION de l'espace mémoire pour corpsReponse */
        //cout << "Content-Length vaut : " << contentLength << endl;
        corpsReponse = new char[contentLength];
        corpsReponse[0] = 0;
        indiceCourantCorpsReponse = 0;
        /* COPIE des éventuels octets recus à la suite de l'entête */
        if (positionFinEntete+4 < nbOctetsLusTotal) // Si des octest
        {
            //cout << "Reception du début du corps avec l'entête..." << endl;
            indiceCourantCorpsReponse = nbOctetsLusTotal-(positionFinEntete+4);
            memcpy(corpsReponse, enteteReponse.substr(positionFinEntete+4, indiceCourantCorpsReponse).c_str(), indiceCourantCorpsReponse) ;
            corpsReponse[indiceCourantCorpsReponse] = 0;
            enteteReponse = enteteReponse.erase(positionFinEntete+4, string::npos);
            //cout << "Taille de l'entete : " << enteteReponse.length() << endl;
            //cout << "Taille du corps : " << indiceCourantCorpsReponse << endl;
        }

        this->recevoirCorps();

    }
    else if(enteteReponse.find("Transfer-Encoding: chunked") != string::npos)
    {
        typeReponse = 2;
        cout << "Utilisation de chunked..." << endl;
        // Si des données sont présentes après l'entête HTTP, on les place dans 'bloc'
        longueurBloc = 0;
        if(positionFinEntete+4 < enteteReponse.length())
        {
            //longueurBloc = enteteReponse.length()-(positionFinEntete+4);
            //bloc = new char[longueurBloc];
            //memcpy(bloc, enteteReponse.substr(positionFinEntete+4, longueurBloc).c_str(), longueurBloc);
            ajouterALaSuiteDeBloc(enteteReponse.substr(positionFinEntete+4, enteteReponse.length()-(positionFinEntete+4)).c_str(), enteteReponse.length()-(positionFinEntete+4));
            enteteReponse = enteteReponse.erase(positionFinEntete+4, string::npos);
            //cout << "BLOC : " << string(bloc,longueurBloc) << endl;
            //cout << "TAILLE BLOC : " << longueurBloc << endl;

            // Les derniers caractères du bloc
            //for(int j=0 ; j<10 ; j++)
            //{
            //    cout << "bloc[" << j << "] = " << hex << (int)bloc[j] << dec<< endl;
            //}
        }

        this->recevoirCorps();
    }
}

void SNClientHTTP::recevoirCorps()
{
    char buffer[1500];  // Lecture par bloc de 1500 octets
    if(typeReponse == 1)        // Content-Length:
    {
        while (indiceCourantCorpsReponse < contentLength)
        {
            int nbOctetsLus = recv(m_maSocket, buffer, 1500, 0);
            //memcpy(reponse+nbOctetsLusTotal, buffer, nbOctetsLus);
            memcpy(corpsReponse+indiceCourantCorpsReponse, buffer, nbOctetsLus);
            indiceCourantCorpsReponse += nbOctetsLus;
            //cout << "Nb octets lus : " << nbOctetsLus << endl;
            //cout << "Nb octets lus total : " << indiceCourantCorpsReponse << endl;
            //cout << "Content-Length vaut : " << contentLength << endl;
        }
        //cout << "Fin de la lecture... " << endl;
        longueurCorpsReponse = contentLength;
    }
    else if(typeReponse == 2) // Transfer-Encoding: chunked
    {

        int nbOctetsTotal = 0;
        int nbOctetsAttendu = 0;
        int nbOctetsRecus;
        longueurCorpsReponse = 0;

        do {
            nbOctetsTotal = 0;
            nbOctetsAttendu = 0;
            int posFinTailleBloc = -1;
            if(bloc != NULL)
            {
                posFinTailleBloc = this->rechercher("\r\n", 2, bloc, longueurBloc);
                nbOctetsTotal = longueurBloc /*- (posFinTailleBloc+2)*/;
                //cout << "NB Octets total " << nbOctetsTotal << endl;
            }
            //cout << "Position de \\r\\n : " << posFinTailleBloc << endl;
            if(posFinTailleBloc == -1) // Si on ne trouve pas "\r\n", lire la trame suivante
            {
                nbOctetsRecus = recv(m_maSocket, buffer, 1500, 0);
                nbOctetsTotal += nbOctetsRecus;
                //if(nbOctetsRecus > 0) buffer[nbOctetsRecus] = '\0';
                //else return;
                this->ajouterALaSuiteDeBloc(buffer, nbOctetsRecus);
                //cout << bloc << endl;
                posFinTailleBloc = this->rechercher("\r\n", 2, bloc, longueurBloc);
            }
            //cout << "Position Fin de taille du bloc : " << posFinTailleBloc << endl;
            char * strLongueur = new char[posFinTailleBloc];
            memcpy(strLongueur, bloc, posFinTailleBloc);
            //cout << string(strLongueur,posFinTailleBloc) << " octets dans le bloc...  " <<  endl;
            nbOctetsAttendu = hexToDec(strLongueur, posFinTailleBloc);
            delete [] strLongueur;
            strLongueur = NULL;
            //cout << nbOctetsAttendu << " octets..." << endl;
            //cout << "Taille de la reponse : " << longueurBloc << endl;
            while (nbOctetsTotal-(posFinTailleBloc+2) < nbOctetsAttendu) {
                //cout << "Autre recv..." << endl;
                nbOctetsRecus = recv(m_maSocket, buffer, 1500, 0);
                nbOctetsTotal += nbOctetsRecus;
                cout << "NbOctetsTotal : " << nbOctetsTotal << endl;
                if(nbOctetsRecus <= 0)
                {
                    //cout << "ERREUR 2 : nbOctetsRecus négatif -> erreur lors de la lecture des octets." << endl;
                    return ;
                }
                this->ajouterALaSuiteDeBloc(buffer, nbOctetsRecus);
                //cout << bloc << endl;

            }
            //posFinTailleBloc = this->rechercher("\r\n", 2, bloc+posFinTailleBloc+2, longueurBloc-(posFinTailleBloc+2));
            //cout << "Position Fin bloc : " << posFinTailleBloc << endl;
            //cout << "Fin de la lecture d'un bloc..." << endl;
            //cout << "Nombre d'octets total lus : " << nbOctetsTotal << endl;
            //cout << bloc << endl;

            int posFinBloc = this->rechercher("\r\n", 2, bloc+posFinTailleBloc+2, nbOctetsTotal-(posFinTailleBloc+2));
            posFinBloc += posFinTailleBloc+2;

            // Les derniers caractères du bloc
            //for(int j=nbOctetsTotal-10 ; j<nbOctetsTotal ; j++)
            //{
            //    cout << "octet[" << j << "] = " << hex << (int)bloc[j] << dec<< endl;
            //}

            if(posFinBloc == -1){cout << "ERREUR 3 :  Les caractères de fin de bloc n'ont pas été trouvés." <<endl; }
            //cout << "Position de la fin du bloc: " << posFinBloc << endl;
            //longueurCorpsReponse += posFinBloc - (posFinTailleBloc+2);

            // Copie du bloc lu dans CorpsReponse
            int longueurTemp = longueurCorpsReponse;
            char * temp = NULL;
            if(longueurCorpsReponse > 0)
            {
                temp = new char [longueurTemp];
                memcpy(temp,corpsReponse,longueurTemp);
                delete [] corpsReponse;
                corpsReponse = NULL;
            }
            longueurCorpsReponse += nbOctetsAttendu;
            corpsReponse = new char [longueurCorpsReponse];
            if(longueurTemp > 0)
            {
                memcpy(corpsReponse,temp,longueurTemp);
                delete [] temp;
                temp = NULL;
            }
            memcpy(corpsReponse+longueurTemp,bloc+posFinTailleBloc+2,nbOctetsAttendu);

            //xml += bloc.substr(posFinTailleBloc+2, posFinBloc-(posFinTailleBloc+2));
            //cout << "Substring..." << endl;
            //cout << "XML : " << xml << endl;
            if(posFinBloc+2 < nbOctetsTotal)
            {
                //bloc = bloc.substr(posFinBloc+2, bloc.length()-(posFinBloc+2));

                int longueurTemp = nbOctetsTotal-(posFinBloc+2);
                char * temp = new char [longueurTemp];
                memcpy(temp,bloc+posFinBloc+2,longueurTemp);
                delete [] bloc;
                bloc = NULL;
                longueurBloc = longueurTemp;
                bloc = new char [longueurBloc];
                memcpy(bloc,temp,longueurTemp);
                delete [] temp;
                temp = NULL;

            }
            else
            {
                longueurBloc = 0;
                delete [] bloc;
                bloc = NULL;
            }
            //cout << "Nb octets restant pour le bloc suivant : " << longueurBloc << endl;
            // extraction des données du bloc...
         }while(nbOctetsAttendu != 0);

        //cout << "Fin de l'extraction..." << endl;
    }

}

void SNClientHTTP::ajouterALaSuiteDeBloc(const char *src, int longueurSrc)
{
    //cout << "Ajout dans le bloc de " << longueurSrc << " octets." << endl;
    //cout << "Taille du bloc avant : " << longueurBloc << " octets." << endl;
    // Copie du contenu actuel de dest dans tmp si longueurDest > 0
    int longueurTmp = longueurBloc;
    char * tmp = NULL;
    if(longueurBloc > 0)
    {
        tmp = new char[longueurTmp];
        memcpy(tmp,bloc,longueurTmp);
        delete [] bloc;
        bloc = NULL;
    }

    longueurBloc += longueurSrc;
    bloc = new char [longueurBloc];
    if(longueurTmp > 0)
    {
        memcpy(bloc, tmp, longueurTmp);
        delete [] tmp;
        tmp = NULL;
    }
    memcpy(bloc+longueurTmp, src, longueurSrc);
    //cout << "Taille du bloc apres : " << longueurBloc << " octets." << endl;

}





/*void SNClientHTTP::recevoir(unsigned int nbOctetsAttendus)
{
    int nbOctetsLus;
    int nbOctetsLusTotal = corpsReponse.length();
    char buffer[1500];  // Lecture par bloc de 1500 octets
    while (nbOctetsLusTotal < nbOctetsAttendus)
    {
        nbOctetsLus = recv(m_maSocket, buffer, 1500, 0);
        //memcpy(reponse+nbOctetsLusTotal, buffer, nbOctetsLus);
        corpsReponse += string (buffer, nbOctetsLus);
        nbOctetsLusTotal += nbOctetsLus;
        //cout << "Nb octets lus : " << nbOctetsLus << endl;
        //cout << "Nb octets lus total : " << nbOctetsLusTotal << endl;

    }while(nbOctetsLusTotal < nbOctetsAttendus);
    cout << "Fin de la lecture... " << endl;

}*/
/*string SNClientHTTP::recevoirCIRPARK(){

    string reponse = "";
    string entete = "";
    string xml = "";
    string bloc = "";
    bool continuer = true;
    int nbOctetsRecus = 0;

    char buffer[1501];

    do{

       nbOctetsRecus = recv(m_maSocket, buffer, 1500, 0);
       cout << "Nb octets recus : " << nbOctetsRecus << endl;
       if(nbOctetsRecus > 0) buffer[nbOctetsRecus] = '\0';
       else return "";
       reponse += string(buffer);
       //cout << reponse << endl;
    }while(reponse.find("\r\n\r\n") == -1);

    cout << "Fin de l'entete détectée" << endl;

    int posFinEntete = reponse.find("\r\n\r\n");
    cout << "Position fin entete : " << posFinEntete << endl;
    cout << "Taille de la reponse : " << reponse.length() << endl;
    entete = reponse.substr(0,posFinEntete+4);

    //cout << entete << endl;
    // Récupération des octets suivants s'il y en a :
    if(posFinEntete+4 < reponse.length())
        bloc = reponse.substr(posFinEntete+4, reponse.length()-(posFinEntete+4));


    int nbOctetsTotal = 0;
    int nbOctetsAttendu = 0;

    do {
        nbOctetsTotal = 0;
        nbOctetsAttendu = 0;
        int posFinTailleBloc = bloc.find("\r\n");
        if(posFinTailleBloc == -1) // Si on ne trouve pas "\r\n", lire la trame suivante
        {
            nbOctetsRecus = recv(m_maSocket, buffer, 1500, 0);
            nbOctetsTotal += nbOctetsRecus;
            if(nbOctetsRecus > 0) buffer[nbOctetsRecus] = '\0';
            else return "";
            bloc += string(buffer);
            //cout << bloc << endl;
            posFinTailleBloc = bloc.find("\r\n");
        }
        cout << "Position Fin de taille du bloc : " << posFinTailleBloc << endl;
        string strLongueur = bloc.substr(0,posFinTailleBloc);
        cout << strLongueur << " octets dans le bloc...  " <<  endl;
        nbOctetsAttendu = hexToDec(strLongueur);
        cout << nbOctetsAttendu << " octets..." << endl;
        cout << "Taille de la reponse : " << bloc.length() << endl;
        while (nbOctetsTotal < nbOctetsAttendu) {
            cout << "Autre recv..." << endl;
            nbOctetsRecus = recv(m_maSocket, buffer, 1500, 0);
            nbOctetsTotal += nbOctetsRecus;
            cout << "NbOctetsTotal : " << nbOctetsTotal << endl;
            if(nbOctetsRecus > 0) buffer[nbOctetsRecus] = '\0';
            else {
                cout << "ERREUR 2 : nbOctetsRecus négatif -> erreur lors de la lecture des octets." << endl;
                return "";
            }
            bloc += string(buffer);
            //cout << bloc << endl;
            posFinTailleBloc = bloc.find("\r\n");
        }
        cout << "Fin de la lecture d'un bloc..." << endl;
        cout << "Nombre d'octets total lus : " << nbOctetsTotal << endl;
        cout << bloc << endl;
        int posFinBloc = bloc.find("\r\n", posFinTailleBloc+2);
        if(posFinBloc == -1){cout << "ERREUR 3 :  Les caractères de fin de bloc n'ont pas été trouvés." <<endl; return "";}
        cout << "Position de la fin du bloc: " << posFinBloc << endl;
        xml += bloc.substr(posFinTailleBloc+2, posFinBloc-(posFinTailleBloc+2));
        cout << "Substring..." << endl;
        cout << "XML : " << xml << endl;
        if(posFinBloc+2 < bloc.length())
            bloc = bloc.substr(posFinBloc+2, bloc.length()-(posFinBloc+2));
        else
            bloc = "";
        cout << "bloc restant : " << bloc << endl;
        // extraction des données du bloc...
     }while(nbOctetsAttendu != 0);

    cout << "Fin de l'extraction..." << endl;
    cout << xml << endl;






    if(reponse.find("Transfer-Encoding: chunked", 0) != -1){
        reponse = "";
        do{


            string tailleChunkHex = "";
            string caractere = "";
            do{
                caractere = recevoirDonnees(1);
                //cout << caractere << endl;
                if(caractere.at(0) != '\r'){
                    tailleChunkHex += caractere;
                    //cout << tailleChunkHex << endl;
                }else{
                    recevoirDonnees(1); //degage le \n
                }
            }while(caractere.at(0) != '\r');
            int tailleChunk = hexToDec(tailleChunkHex);
            //cout << "Taille chunk : " << tailleChunck << endl;
            if(tailleChunk > 0){
                reponse += recevoirDonnees(tailleChunk);
                recevoirDonnees(2);
            }else{
                continuer = false;
                //cout << tailleChunk << endl;
                //cout << tailleChunkHex << endl;
            }

        }while(continuer);
    }
    recevoirDonnees(1500);

    return reponse;
    return xml;
}
*/

int SNClientHTTP::recevoirBinaire(string nomDuFichier)
{
    ofstream fichierBinaire;
    int nbOctetsLus;
    int nbOctetsLusTotal = 0;
    int nbOctetsAttendus = 0;
    char buffer[1500];  // Lecture par bloc de 1500 octets
    bool encore = true;
    int contentLength = 0;

    fichierBinaire.open(nomDuFichier.c_str());

    do
    {
        nbOctetsLus = recv(m_maSocket, buffer, 1500, 0);
        //memcpy(reponse+nbOctetsLusTotal, buffer, nbOctetsLus);
        if(nbOctetsLusTotal == 0)
        {
            //cout << buffer << endl;
            string strBuffer(buffer);
            int pos = strBuffer.find("\r\n\r\n");
            if (pos <= 0 ) return -1;
            fichierBinaire.write(buffer+pos+4, nbOctetsLus-(pos+4));
            //cout << "Pos : " << pos << endl;
            // Recherche de "Content-Length :"
            string chaineATrouver = "Content-Length: ";
            int pos2 = strBuffer.find(chaineATrouver);
            //cout << "Pos2 : " << pos2 << endl;
            if(pos2 <=0 ) return -1;
            int pos3 = strBuffer.find("\r\n", pos2);
            //cout << "Pos3 : " << pos3 << endl;
            if(pos3 <= 0 ) return -1;
            if (pos3 <= pos2+chaineATrouver.length()) return -1;
            for(int i=pos2+chaineATrouver.length(); i<pos3; i++)
            {
                //cout << i << ":" << strBuffer[i] << endl;
                contentLength = contentLength*10 + (int)(strBuffer[i]-'0');
            }
            //cout << "Content-Length : " << contentLength << endl;
            nbOctetsAttendus = contentLength + pos + 4;
            //cout << "Nb octets attendus : " << nbOctetsAttendus << endl;
        }
        else
        {
            fichierBinaire.write(buffer, nbOctetsLus);
        }
        nbOctetsLusTotal += nbOctetsLus;
        //cout << "Nb octets lus : " << nbOctetsLus << endl;
        //cout << "Nb octets lus total : " << nbOctetsLusTotal << endl;

    }while(nbOctetsLusTotal < nbOctetsAttendus);
    //cout << "Fin de la lecture... " << endl;
    fichierBinaire.close();

    return nbOctetsLusTotal;
}

int SNClientHTTP::hexToDec(const char *h, int l)
{
    return this->hexToDec(string(h,l));
}


int SNClientHTTP::hexToDec(string hex){
    int decimal = 0;
    stringstream ss;
    ss << std::hex << hex;
    ss >> decimal;
    return decimal;
}

string SNClientHTTP::recevoirDonnees(int taille){
    char buffer[taille];
    int nbOctetsRecus = recv(m_maSocket, buffer, taille, 0);
    //for(int i = 0; i < taille; i++)
    //    cout << buffer[i] << endl;
    buffer[nbOctetsRecus]='\0';
    return buffer;
}

void SNClientHTTP::afficherDebutEtFin(string msg)
{
    // Affichage des 20 premiers octets
    int longueur = 20;
    if(longueur > msg.length())
        longueur = msg.length();
    cout << "** LES 1ERS OCTETS ** : ";
    for(int i=0 ; i<longueur; i++)
    {
        if(msg[i] == '\r')
            cout << "\r";
        else if(msg[i] == '\n')
            cout << "\n";
        else
            cout << msg[i];
    }
    cout << endl;

    // Affichage des 20 derniers octets
    if(msg.length() > 20)
    {
        cout << "** LES DERNIERS OCTETS ** : ";
        for(int i=msg.length()-20 ; i<msg.length(); i++)
        {
            if(msg[i] == '\r')
                cout << "\r";
            else if(msg[i] == '\n')
                cout << "\n";
            else
                cout << msg[i];
        }
        cout << endl;

    }
}

int SNClientHTTP::rechercher(const char *aChercher, int tailleAChercher, const char *message, int tailleMessage)
{
    int i = 0;
    bool estPresent = false;
    while( (i<=(tailleMessage-tailleAChercher)) && (estPresent==false) )
    {
        int j = 0;
        while((message[i+j] == aChercher[j]) && (j<tailleAChercher) )
        {
            j++;
        }
        if(j == tailleAChercher) estPresent = true;
        else
            i++;
    }
    if (estPresent)
        return i;
    return -1;
}
