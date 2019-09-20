#ifndef SNCLIENTHTTP_H
#define SNCLIENTHTTP_H
#include <string>

using namespace std;

class SNClientHTTP
{

public:
    SNClientHTTP();
    ~SNClientHTTP();
    //bool connexionAuServeur(const char * adresseIPServeur, unsigned short portServeur);
    bool connexionAuServeurParAdresseIP(string adresseIPServeur, unsigned short portServeur);
    bool connexionAuServeurParNomDeDomaine(string nomDeDomaine, unsigned short portServeur);
    //bool envoyer(const char * requete, int longueurRequete);
    bool envoyer(string requete);
    //void recevoir(unsigned int nbOctetsAttendus);
    //string recevoirCIRPARK();
    int recevoirBinaire(string nomDuFichier);
    void recevoir();
    //int recevoir(char * reponse, int longueurMaxReponse);
    inline string EnteteReponse() const { return enteteReponse;}
    inline const char * CorpsReponse() const { return corpsReponse;}
    inline int LongueurCorpsReponse() const { return longueurCorpsReponse; }
    inline string Reponse() const { return enteteReponse+corpsReponse;}


private :
    static int nbClientTCP;
    int m_maSocket;
    string m_adresseIPServeur;
    unsigned short m_portServeur;

    string enteteReponse;
    int longueurCorpsReponse;
    int typeReponse;    // 1 pour Content-Length, et 2 pour l'autre
    int contentLength;  // utilisé si typeReponse == 1
    char * corpsReponse;
    int indiceCourantCorpsReponse;

    char * bloc;
    int longueurBloc;

    int hexToDec(string hex);
    int hexToDec(const char * h, int l);
    int rechercher(const char * aChercher, int tailleAChercher, const char * message, int tailleMessage);
    string recevoirDonnees(int taille);
    void afficherDebutEtFin(string msg);

    void ajouterALaSuiteDeBloc(const char * src, int longueurSrc);
    void recevoirCorps();

};

#endif // CLIENTHTTP_H
