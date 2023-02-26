#ifndef STATION_H_
#define STATION_H_

#include <string>
using namespace std;

class Station {
public:
	Station();
	Station(int number, string name_);
	Station(const Station& station) = delete;
	Station(Station&& station) = delete;

	int getStationNumber() const;
	string getStationString() const;
private:
	int number_;
	string name_;
};
#endif