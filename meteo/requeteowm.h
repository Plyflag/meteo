#ifndef REQUETEOWM_H
#define REQUETEOWM_H

#include <string>
using namespace std;

class requeteOWM {
private:
    string ville;
    string pays;
    string cle;

public:
    void modifierVille(string uneVille);
    void modifierPays(string unPays);
    requeteOWM();
    ~requeteOWM();
    string creerRequeteOWM();
};

#endif // REQUETEOWM_H
