#ifndef REQUETEOWM_H
#define REQUETEOWM_H

class requeteOWM {
private:
    string ville;
    string pays;
    string clé;

public:
    void modifierVille(string uneVille);
    void modifierPays(string unPays);
    requeteOWM();
    ~requeteOWM();
};

#endif // REQUETEOWM_H
