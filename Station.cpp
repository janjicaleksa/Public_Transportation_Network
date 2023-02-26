#include "Station.h"

Station::Station() : number_(0) {}

Station::Station(int number, string name) : number_(number), name_(name) {}

int Station::getStationNumber() const {
	return number_;
}

string Station::getStationString() const {
	return name_;
}