#ifndef GRAPHNODE_H_
#define GRAPHNODE_H_
#include "Exceptions.h"
#include "Station.h"
#include <vector>
#include <list>
#include <map>

class GraphNode {
public:
	GraphNode();
	GraphNode(Station*);
	~GraphNode();
	GraphNode(const GraphNode& graphNode) = delete;
	GraphNode(GraphNode&& graphNode) = delete;

	int getGraphNodeNumber() const;
	string getGraphNodeName() const;
	map<int, bool> getStationsMap();
	vector<string> getLines(char) const;
	int getNodesDirectionAsize() const;
	int getNodesDirectionBsize() const;
	map<string, list<int>> getLinesMap() const;

	void addLine(map<int, bool>&, vector<Station*>, const string&, const string&);
	void loadLines(const string&, const string&);
	void addMapElement(int, bool);
	
	GraphNode* searchByStationNumber(int);
	GraphNode* searchDirectionALineName(const string&);
	GraphNode* searchDirectionBLineName(const string&);
	bool bfsRoute(map<int,int>&, map<int,bool>, int);
	bool bfsRouteModified(map<int, int>&, map<int, bool>, int);
private:
	map<int, bool> existingStations_;
	map<string, list<int>> mapLines;
	vector<string> linesDirectionA, linesDirectionB;
	vector<GraphNode*> nodesDirectionA, nodesDirectionB;
	Station* station_;

	int readNumber(const string& str, int& pos);
	GraphNode* findStationPointer(vector<GraphNode*>, GraphNode*);
	Station* findStationNumber(vector<Station*>, int);
};
#endif