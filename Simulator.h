#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include "GraphNode.h"
#include <iostream>
#include <fstream>
#include <set>
#include <sstream>

using namespace std;

class Simulator {
public:
	Simulator();
	~Simulator();
	Simulator(const Simulator&) = delete;
	Simulator(Simulator&&) = delete;

	void readStationsInfo(const string&);
	void readLinesInfo(const string&);
	void getStationInfo(int);
	void getLineInfo(const string&);
	void getLineStats(const string&);
	void findShortestRoute(int, int);
	void findLightestRoute(int, int);
private:
	vector<Station*> stations_;
	vector<string> lines_;
	GraphNode* root_;

	bool findLineInVector(vector<string>, const string&);
	bool findNumberInVector(vector<Station*>, int);
	void sortVector(vector<string>&);
	void updateStats(GraphNode*, const string&, vector<string>&, map<string, int>&);
	vector<string> findMaxValue(map<string, int>);
	void printPath(list<int>, int, int);
	string findConnection(map<string, list<int>>, string, int, int);
};

#endif