#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <exception>
using namespace std;

class invalidStationFilepath : public exception {
public:
	invalidStationFilepath() : exception("Greska! Ne postoji data putanja do fajla sa stajalistima.\n") {}
};

class invalidLinesFilepath : public exception {
public:
	invalidLinesFilepath() : exception("Greska! Ne postoji data putanja do fajla sa linijama gradskog prevoza.\n") {}
};

#endif