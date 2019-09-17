#include "requeteowm.h"

requeteOWM::requeteOWM()
{
    pays='France';
    ville='Paris';
    cle = "2314ff15cd37dc7b7c8288a66fe8054b";
}

requeteOWM::~requeteOWM()
{
}



string requeteOWM::creerRequeteOWM()
{
    string http = "GET /data/2.5/weather?q=" + ville + "," + pays + "&units=metric&appid=" + cle + "&mode=xml HTTP/1.1\r\nHost: api.org\r\nConnection: keep-alive\r\n\r\n";
    return http;
}
