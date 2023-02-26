#include "GraphNode.h"
#include <iostream>
GraphNode::GraphNode() : station_(nullptr) {}

GraphNode::GraphNode(Station* station) : station_(station) {}

GraphNode::~GraphNode() {
	if (!nodesDirectionA.empty()) {
		for (auto it : this->nodesDirectionA) {
			if (it != nullptr) {
				delete it;
				it = nullptr;
			}
		}
		this->nodesDirectionA.clear();
	}
	
	if (!nodesDirectionB.empty()) {
		for (auto it : this->nodesDirectionB) {
			if (it != nullptr) {
				delete it;
				it = nullptr;
			}
		}
		this->nodesDirectionB.clear();
	}

	delete station_;
	station_ = nullptr;

	linesDirectionA.clear();
	linesDirectionB.clear();
	existingStations_.clear();
	mapLines.clear();
}

int GraphNode::getGraphNodeNumber() const {
	return this->station_->getStationNumber();
}

string GraphNode::getGraphNodeName() const {
	return this->station_->getStationString();
}

void GraphNode::addLine(map<int, bool> &rootMap, vector<Station*> stations, const string& lineName, const string& lineStations) {
	GraphNode* tmp = this; 
	int pos = 0; //pomocna promenljiva za parsiranje stringa
	bool turn = false; //pomocna promenljiva koja oznacava da li se doslo okretnice, odnosno do '#'

	int num = readNumber(lineStations, pos); //procitani broj stajalista
	while (num) {
		GraphNode* gn;
		if (rootMap.at(num) == false) {//ako ne postoji broj stajalista u nekom cvoru grafa
			Station* st = findStationNumber(stations, num); //pronadji stajaliste iz ucitanog vektora na osnovu procitanog broja
			gn = new GraphNode(st); //napravi novi cvor grafa
			rootMap.at(num) = true; //u mapi oznaci da od sada taj broj stajalista postoji u grafu
		}
		else gn = this->searchByStationNumber(num);
		
		if (turn == false) { //sve dok nisi dosao do okretnice, dodaj u A smeru
			GraphNode* curr = findStationPointer(tmp->nodesDirectionA, gn);
			if (curr == nullptr) {
				tmp->nodesDirectionA.push_back(gn);
				tmp = tmp->nodesDirectionA.back();
				if (tmp != this) tmp->linesDirectionA.push_back(lineName);
			}
			else {
				curr->linesDirectionA.push_back(lineName);
				tmp = curr;
			}
		}
		else { //ako si dosao do okretnice, promeni smer
			GraphNode* curr = findStationPointer(tmp->nodesDirectionB, gn);
			if (curr == nullptr) {
				tmp->nodesDirectionB.push_back(gn);
				tmp = tmp->nodesDirectionB.back();
				if (tmp != this) tmp->linesDirectionB.push_back(lineName);
			}
			else {
				curr->linesDirectionB.push_back(lineName);
				tmp = curr;
			}
		}

		if (lineStations[pos] == '#') { //ako si dosao do okretnice, oznaci turn promenljivu
			turn = true;
			pos++; //preskoci '#'
			num = readNumber(lineStations, pos); //preskoci ponovno citanje sifre stajalista okretnice
		}
		pos++; //u svakom slucaju, pomeri pos da bi mogao da se procita naredni broj 
		
		if (pos <= lineStations.length()) num = readNumber(lineStations, pos); //citaj narednu sifru stajalista i udji u petlju ponovo
		else break;
	}
}

void GraphNode::loadLines(const string& lineName, const string& lineStations) {
	list<int> lst;
	int pos = 0;
	int n = readNumber(lineStations, pos);
	while (n) {
		lst.push_back(n);
		if (lineStations[pos] == '\0') break;
		pos++;
		n = readNumber(lineStations, pos);
	}
	this->mapLines.insert(pair<string, list<int>>(lineName, lst));
}

void GraphNode::addMapElement(int number, bool b) {
	existingStations_.insert(pair<int, bool>(number, b));
}

map<int, bool> GraphNode::getStationsMap() {
	return existingStations_;
}

vector<string> GraphNode::getLines(char c) const {
	switch (c) {
		case 'A': {
			return linesDirectionA;
			break;
		}
		case 'B': {
			return linesDirectionB;
			break;
		}
	}
}

int GraphNode::getNodesDirectionAsize() const {
	return this->nodesDirectionA.size();
}

int GraphNode::getNodesDirectionBsize() const {
	return this->nodesDirectionB.size();
}

map<string, list<int>> GraphNode::getLinesMap() const {
	return this->mapLines;
}

GraphNode* GraphNode::searchByStationNumber(int number) {
	if (!this->nodesDirectionA.empty()) {
		for (auto it : this->nodesDirectionA)
			if (it->getGraphNodeNumber() == number) return it;
	}
	if (!this->nodesDirectionB.empty()) {
		for (auto it : this->nodesDirectionB)
			if (it->getGraphNodeNumber() == number) return it;
	}

	list<GraphNode*> queue; //lista zato sto je moguce obrisati pop_front()
	vector<int> visited; //vektor koji oznacava da li je neki cvor vec posecen

	for (auto it : this->nodesDirectionA) queue.push_back(it); //dodaj u red za obradu

	while (!queue.empty()) {
		GraphNode* gn = queue.front(); //uzmi sa pocetka
		visited.push_back(gn->getGraphNodeNumber()); //dodaj u vektor sifru stanice koja je posecena
		queue.pop_front(); //pa obrisi sa pocetka reda za cekanje
		
		if (!gn->nodesDirectionA.empty()) { //ako uzeti cvor ima komsije u A smeru
			for (auto it : gn->nodesDirectionA)  //prodji kroz sve komsije
				if (it->getGraphNodeNumber() == number) return it; //vrati komsiju ako si pronasao
				else { //u suprotnom ispitaj da li je komsija vec posecen
					bool found = false; //pretpostavicemo da nije
					for (auto it2 : visited) //onda prodji kroz ceo vektor posecenih stajalista
						if (it2 == it->getGraphNodeNumber()) { //ako postoji vec ta sifra stajalista
							found = true; //komsija je vec posecen
							break;
						}

					if (found == false) queue.push_back(it); //ako nije bio posecen, dodaj u red za cekanje
				}
		}

		if (!gn->nodesDirectionB.empty()) { //ako uzeti cvor ima komsije u B smeru
			for (auto it : gn->nodesDirectionB)  //prodji kroz sve komsije
				if (it->getGraphNodeNumber() == number) return it; //vrati komsiju ako si pronasao broj koji trazis
				else { //u suprotnom ispitaj da li je komsija vec posecen
					bool found = false; //pretpostavicemo da nije
					for (auto it2 : visited) //onda prodji kroz ceo vektor posecenih stajalista
						if (it2 == it->getGraphNodeNumber()) { //ako postoji vec ta sifra stajalista
							found = true; //komsija je vec posecen i ne moras dalje ici kroz petlju
							break; 
						}

					if (found == false) queue.push_back(it); //ako nije bio posecen, dodaj u red za cekanje
				}
		}
	}
}

GraphNode* GraphNode::searchDirectionALineName(const string&lineName) {
	if (!this->nodesDirectionA.empty()) {
		for (auto i : this->nodesDirectionA) { //prodji kroz sve susedne (i) cvorove u smeru A
			for (auto j : i->linesDirectionA) //ako u njihovim vektorima linija postoji data linija
				if (j.compare(lineName) == 0) {
					return i; //vrati taj susedni cvor
				}
		}
	}
	return nullptr;
}

GraphNode* GraphNode::searchDirectionBLineName(const string& lineName) {
	if (!this->nodesDirectionB.empty()) { //isto sto za smer A vazi, vazi i ovde
		for (auto i : this->nodesDirectionB) {
			for (auto j : i->linesDirectionB)
				if (j.compare(lineName) == 0) {
					return i;
				}
		}
	}
	return nullptr;
}

bool GraphNode::bfsRoute(map<int,int>& prev, map<int,bool> visited, int end) {
	if (this->getGraphNodeNumber() == end) return true;

	list<GraphNode*> queue;
	queue.push_back(this);

	while (!queue.empty()) {
		GraphNode* gn = queue.front(); //uzmi sa pocetka
		int u = gn->getGraphNodeNumber();
		visited.at(u) = true; //dodaj u vektor posecenih stanica sifru stanice koja je posecena
		queue.pop_front(); //pa obrisi sa pocetka reda za cekanje

		if (!gn->nodesDirectionA.empty()) { //ako uzeti cvor ima komsije u A smeru
			for (auto it : gn->nodesDirectionA) {  //prodji kroz sve komsije
				/*if (it->getGraphNodeNumber() == end) {
					prev.at(it->getGraphNodeNumber()) = u;
					return true;
				}
				else { //u suprotnom ispitaj da li je komsija vec posecen
					bool found = false; //pretpostavicemo da nije
					for (auto it2 : visited) //onda prodji kroz ceo vektor posecenih stajalista
						if (it2 == it->getGraphNodeNumber()) { //ako postoji vec ta sifra stajalista
							found = true; //komsija je vec posecen
							break;
						}

					if (found == false) {
						queue.push_back(it); //ako nije bio posecen, dodaj u red za cekanje
						prev.at(it->getGraphNodeNumber()) = u; //trenutni cvor je prethodnik narednog cvora
					}
				}*/
				int v = it->getGraphNodeNumber();
				if (visited.at(v) == false) {
					visited.at(v) = true;
					prev.at(v) = u;
					queue.push_back(it);
					if (v == end) return true;
				}
			}
		}

		if (!gn->nodesDirectionB.empty()) { //ako uzeti cvor ima komsije u B smeru
			for (auto it : gn->nodesDirectionB) {  //prodji kroz sve komsije
				/*if (it->getGraphNodeNumber() == end) {
					prev.at(it->getGraphNodeNumber()) = u;
					return true;
				}
				else { //u suprotnom ispitaj da li je komsija vec posecen
					bool found = false; //pretpostavicemo da nije
					for (auto it2 : visited) //onda prodji kroz ceo vektor posecenih stajalista
						if (it2 == it->getGraphNodeNumber()) { //ako postoji vec ta sifra stajalista
							found = true; //komsija je vec posecen i ne moras dalje ici kroz petlju
							break;
						}

					if (found == false) {
						queue.push_back(it); //ako nije bio posecen, dodaj u red za cekanje
						prev.at(it->getGraphNodeNumber()) = u; //trenutni cvor je prethodnik narednog cvora
					}
				}*/
				int v = it->getGraphNodeNumber();
				if (visited.at(v) == false) {
					visited.at(v) = true;
					prev.at(v) = u;
					queue.push_back(it);
					if (v == end) return true;
				}
			}
		}
	}
	return false;
}

bool GraphNode::bfsRouteModified(map<int, int>& prev, map<int, bool> visited, int end) {
	if (this->getGraphNodeNumber() == end) return true;

	list<GraphNode*> queue;
	queue.push_back(this);

	while (!queue.empty()) {
		GraphNode* gn = queue.front(); //uzmi sa pocetka
		int u = gn->getGraphNodeNumber();
		visited.at(u) = true; //dodaj u vektor posecenih stanica sifru stanice koja je posecena
		queue.pop_front(); //pa obrisi sa pocetka reda za cekanje

		if (!gn->nodesDirectionA.empty()) { //ako uzeti cvor ima komsije u A smeru
			for (auto it : gn->nodesDirectionA) {  //prodji kroz sve komsije
				int v = it->getGraphNodeNumber();
				if (visited.at(v) == false) {
					visited.at(v) = true;
					prev.at(v) = u;
					queue.push_front(it);
					if (v == end) return true;
				}
			}
		}

		if (!gn->nodesDirectionB.empty()) { //ako uzeti cvor ima komsije u B smeru
			for (auto it : gn->nodesDirectionB) {  //prodji kroz sve komsije
				int v = it->getGraphNodeNumber();
				if (visited.at(v) == false) {
					visited.at(v) = true;
					prev.at(v) = u;
					queue.push_front(it);
					if (v == end) return true;
				}
			}
		}
	}
	return false;
}

int GraphNode::readNumber(const string& str, int& pos) {
	int num = 0;
	while (isdigit(str[pos])) {
		num = num * 10 + (int)(str[pos] - '0');
		pos++;
	}
	return num;
}

GraphNode* GraphNode::findStationPointer(vector<GraphNode*> vct, GraphNode* gn) {
	if (!vct.empty()) {
		for (auto it : vct) {
			if (gn->station_->getStationNumber() == it->station_->getStationNumber()) return it;
		}
	}
	return nullptr;
}

Station* GraphNode::findStationNumber(vector<Station*> stations, int num) {
	for (auto it : stations) {
		if (it->getStationNumber() == num) 
			return it;
	};
	throw num; //ako nisi pronasao stanicu po broju, baci gresku
}