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
    inline void modifierVille(string uneVille){ville=uneVille;}
    inline void modifierPays(string unPays){pays=unPays;}
    requeteOWM();
    ~requeteOWM();
    string creerRequeteOWM();
};



#endif // REQUETEOWM_H
