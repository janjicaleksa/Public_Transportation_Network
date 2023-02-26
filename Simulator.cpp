#include "Simulator.h"

Simulator::Simulator() {
	this->root_ = new GraphNode();
}

Simulator::~Simulator() {
	for (auto it : stations_) {
		delete it;
	}
	stations_.clear();
	lines_.clear();

	delete root_;
}

void Simulator::readStationsInfo(const string& input) {
	fstream f(input, ios::in);
	if (!f.is_open()) throw invalidStationFilepath();

	int number; 
	string name;
	while (f >> number) { //sve dok mozes da procitas broj
		getline(f, name); // procitaj i ostatak linije fajla 
		name.erase(name.begin()); //obrisi ' ' na pocetku imena
		Station* s = new Station(number, name);
		stations_.push_back(s);
		root_->addMapElement(number, false);
	}

	f.close();
}

void Simulator::readLinesInfo(const string& input) {
	fstream f(input, ios::in);
	if (!f.is_open()) throw invalidLinesFilepath();

	string lineName, lineStations;
	map<int, bool> m = root_->getStationsMap();

	while (f.peek() != EOF) {
		f >> lineName; //procitaj naziv linije
		getline(f, lineStations); //procitaj ostatak reda, odnosno nizove stanica
		lineStations.erase(lineStations.begin()); //obrisi prvi char ' '
		lines_.push_back(lineName); //dodaje naziv linije u vektor
		
		try {
			root_->addLine(m, stations_, lineName, lineStations);
			root_->loadLines(lineName, lineStations);
		}
		catch (int num) {
			cout << "Greska! Sifra stajalista broj " << num << "se pojavljuje u liniji " << lineName << ", a ne postoji u sistemu. Ispravite ulazne fajlove!" << endl;
		}
	}

	f.close();
}

void Simulator::getStationInfo(int number) {
	bool found = findNumberInVector(this->stations_, number);
	if (found == false) throw number;  //ako nisi pronasao broj stanice medju ucitanim stanicama, odmah baci gresku

	GraphNode* tmp = root_->searchByStationNumber(number);

	vector<string> tmpLinesA = tmp->getLines('A'); //vektor naziva linija u smeru A
	vector<string> tmpLinesB = tmp->getLines('B'); //vektor naziva linija u smeru B
	if (!tmpLinesB.empty()) tmpLinesA.insert(tmpLinesA.end(), tmpLinesB.begin(), tmpLinesB.end()); //konkateniranje dva vektora
	set<string> s(tmpLinesA.begin(), tmpLinesA.end()); //izbacivanje duplikata
	tmpLinesA.assign(s.begin(), s.end());//vracanje u vektor
	sortVector(tmpLinesA); //sortiranje naziva linija
	
	string linesFormat = "["; //format ispisa
	for (auto it : tmpLinesA) {
		linesFormat.append(it);
		linesFormat.append(" ");
	}
	linesFormat.erase(linesFormat.end()-1);
	linesFormat.push_back(']');

	string output = "stajaliste_"; //ispis u fajl 
	output.append(to_string(number));
	output.append(".txt");

	fstream f(output,ios::out);
	f << tmp->getGraphNodeNumber() << " " << tmp->getGraphNodeName() << " " << linesFormat << endl;
	f.close();
}

void Simulator::getLineInfo(const string& line) {
	bool found = findLineInVector(this->lines_, line);
	if (found == false) throw line;//ako nisi pronasao naziv linije medju ucitanim linijama, odmah baci gresku
	
	map<int, bool> visited;
	for (auto it : stations_) visited.insert(pair<int, bool>(it->getStationNumber(), false)); //inicijalizacija mape

	string start, end;
	stringstream ss;

	GraphNode* tmp = root_->searchDirectionALineName(line); //pronadji pocetnu stanicu date linije 
	start = tmp->getGraphNodeName(); //sacuvaj naziv te stanice
	map<string, list<int>> mapLines = root_->getLinesMap(); //mapa svih linija i stanica
	list<int> lst = mapLines.at(line); //lista stanica date linije
	lst.pop_front(); //posto je pronadjen pocetna stanica linije, moze da se obrise

	while (tmp) {
		visited.at(tmp->getGraphNodeNumber()) = true;
		int num = lst.front();//
		lst.pop_front(); //
		ss << to_string(tmp->getGraphNodeNumber()) << " " << tmp->getGraphNodeName() << "\n";
		GraphNode* tmp2 = tmp->searchDirectionALineName(line);
		
		if (!tmp2 || visited.at(tmp2->getGraphNodeNumber()) == true) { //ako u susednim cvorovima u smeru A grafa ne postoji naziv linije, znaci da je okretnica
			end = tmp->getGraphNodeName(); //sacuvaj naziv poslednje stanice
			break; //izadji iz prolaska u smeru A
		}
		if (tmp2->getGraphNodeNumber() != num) tmp2 = tmp->searchByStationNumber(num);//
		tmp = tmp2;
	}
	while (tmp) { //tamo gde si napustio smer A, nastavi u smeru B
		visited.at(tmp->getGraphNodeNumber()) = true;
		int num = -1;
		if (lst.size() != 0) {
			num = lst.front();
			lst.pop_front();
		}
		ss << to_string(tmp->getGraphNodeNumber()) << " " << tmp->getGraphNodeName() << "\n";
		GraphNode* tmp2 = tmp->searchDirectionBLineName(line);
		
		if (tmp->getGraphNodeName().compare(start) == 0) break;
		if (num != -1 && tmp2->getGraphNodeNumber() != num) tmp2 = tmp->searchByStationNumber(num);//
		tmp = tmp2;
	}

	string output = "linija_"; //ispis
	output.append(line);
	output.append(".txt");

	fstream f(output, ios::out);
	f << line << " " << start << "->" << end << endl;
	f << ss.str();
	f.close();
}

void Simulator::getLineStats(const string& line) {
	bool found = findLineInVector(this->lines_, line);
	if (found == false) throw line;//ako nisi pronasao naziv linije medju ucitanim linijama, odmah baci gresku

	GraphNode* tmp = root_->searchDirectionALineName(line); //pronadji pocetnu stanicu date linije
	map<string, int> stats;
	vector<string> mutualStationLines;
	map<int, bool> visited; //mapa posecenih stanica
	for (auto it : stations_) visited.insert(pair<int, bool>(it->getStationNumber(), false)); //inicijalizacija mape stanica
	map<string, list<int>> mapLines = root_->getLinesMap();
	list<int> lst = mapLines.at(line);
	int start = lst.front();
	lst.pop_front();

	while (tmp) {
		updateStats(tmp, line, mutualStationLines, stats);
		visited.at(tmp->getGraphNodeNumber()) = true;
		int nextNum = -1;
		if (lst.size()) {
			nextNum = lst.front();
			lst.pop_front();
		}
		if (nextNum == tmp->getGraphNodeNumber()) nextNum = lst.front(); //ovo je slucaj kad je okretnica
		if (nextNum == start || nextNum == -1 || visited.at(nextNum)) break; // ako je naredna stanica pocetna ili je neka od posecenih, prekini sa prolaskom
		else tmp = tmp->searchByStationNumber(nextNum);
	}

	vector<string> max = findMaxValue(stats); //sortiranje
	sortVector(mutualStationLines);
	sortVector(max);

	string output = "statistika_"; //ispis
	output.append(line);
	output.append(".txt");

	fstream f(output, ios::out);
	f << line << endl;
	for (auto it : mutualStationLines) f << it << " ";
	f << endl;
	string ss = "";
	for (auto it : max) {
		ss.append(it);
		ss.append(",");
	}
	ss.erase(ss.end());
	f << ss;
	f.close();
}

void Simulator::findShortestRoute(int start, int end) {
	bool found = findNumberInVector(this->stations_, start);
	if (found == false) throw start;
	bool found2 = findNumberInVector(this->stations_, end);
	if (found2 == false) throw end;

	GraphNode* startNode = root_->searchByStationNumber(start); //pocetni cvor
	map<int, int> prev;
	map<int, bool> visited;
	for (auto it : this->stations_) {
		prev.insert(pair<int, int>(it->getStationNumber(), -1)); //inicijalizacija mapa
		visited.insert(pair<int, bool>(it->getStationNumber(), false));
	}
	
	bool b = startNode->bfsRoute(prev, visited, end);
	if (!b) throw b;
	else {
		list<int> path;
		int tmp = end;
		path.push_front(tmp);
		while (prev[tmp] != -1) {
			path.push_front(prev[tmp]);
			tmp = prev[tmp];
		}
		printPath(path, start, end);
	}
}

void Simulator::findLightestRoute(int start, int end) {
	bool found = findNumberInVector(this->stations_, start);
	if (found == false) throw start;
	bool found2 = findNumberInVector(this->stations_, end);
	if (found2 == false) throw end;

	GraphNode* startNode = root_->searchByStationNumber(start); //pocetni cvor
	map<int, int> prev;
	map<int, bool> visited;
	for (auto it : this->stations_) {
		prev.insert(pair<int, int>(it->getStationNumber(), -1)); //inicijalizacija mapa
		visited.insert(pair<int, bool>(it->getStationNumber(), false));
	}

	bool b = startNode->bfsRouteModified(prev, visited, end);
	if (!b) throw b;
	else {
		list<int> path;
		int tmp = end;
		path.push_front(tmp);
		while (prev[tmp] != -1) {
			path.push_front(prev[tmp]);
			tmp = prev[tmp];
		}
		printPath(path, start, end);
	}
}

bool Simulator::findLineInVector(vector<string> vct, const string& line) {
	if (vct.size() != 0) 
		for (auto it : vct) if (it.compare(line) == 0) return true;
	return false;
}

bool Simulator::findNumberInVector(vector<Station*> stations, int num) {
	for (auto it : stations)
		if (it->getStationNumber() == num) return true;
	return false; 	
}

void Simulator::sortVector(vector<string>& vct) {
	for (size_t i = 0; i < vct.size() - 1; i++) { //sortiranje naziva linija
		for (size_t j = i + 1; j < vct.size(); j++) {
			if (vct[i].compare(vct[j]) > 0) vct[i].swap(vct[j]);
		}
	}
}

void Simulator::updateStats(GraphNode* tmp, const string& line, vector<string>& mutualStationLines, map<string, int>& stats) {
	for (char i = 'A'; i < 'C'; i++) { //proveri i za smer A i za smer B 
		if (tmp->getLines(i).size()) { //ako postoji ijedna linija u smeru
			for (auto it : tmp->getLines(i)) {
				if (it.compare(line) != 0) { //za sve razlicite nazive linija od ucitane linije
					if (!findLineInVector(mutualStationLines, it)) { //ako ne postoji naziv (it) linije u vektoru zajednickih linija
						stats.insert(pair<string, int>(it, 1)); //ucitaj u mapu
						mutualStationLines.push_back(it); //dodaj naziv linije u vektor zajednickih linija
					}
					else {
						stats[it]++ ; //azuriraj mapu
					}
				}
			}
		}
	}
}

vector<string> Simulator::findMaxValue(map<string, int> map) {
	vector<string> vct;
	int max = 0;
	for (auto it : map) {
		if (it.second > max) {
			vct.clear();
			vct.push_back(it.first);
			max = it.second;
		}
		else if (it.second == max) {
			vct.push_back(it.first);
		}
	}
	return vct;
}

void Simulator::printPath(list<int> path, int start, int end) {
	string output = "putanja_";
	output.append(to_string(start));
	output.append("_");
	output.append(to_string(end));
	output.append(".txt");
	fstream f(output, ios::out);
	
	string prevLine = "";
	int n = path.front();
	path.pop_front();
	int m = path.front();
	path.pop_front();
	string nextLine = findConnection(root_->getLinesMap(), prevLine, n, m);
	f << "->" << nextLine << endl;
	f << n << " " << m << " ";
	prevLine = nextLine;
	while (!path.empty()) {
		n = m;
		m = path.front();
		path.pop_front();
		nextLine = findConnection(root_->getLinesMap(), prevLine, n, m);
		if (nextLine.compare(prevLine) == 0) {
			f << m << " ";
		}
		else {
			f << endl;
			f << prevLine << "->" << nextLine << endl;
			f << n << " " << m << " ";
			prevLine = nextLine;
		}
	}
	f.close();
}

string Simulator::findConnection(map<string, list<int>> m, string prevLine, int first, int next) {
	if (prevLine.size()) {
		list<int> lst = m.at(prevLine);
		vector<int> vct;
		while (!lst.empty()) {
			vct.push_back(lst.front());
			lst.pop_front();
		}
		for (int i = 0; i != vct.size() - 1; i++) {
			if (vct[i] != vct[i+1] && vct[i] == first && vct[i+1] == next) return prevLine;
		}
	}
	for (auto it : m) {
		if (it.first.compare(prevLine) != 0) {
			list<int> lst2 = it.second;
			vector<int> vct;
			while (!lst2.empty()) {
				vct.push_back(lst2.front());
				lst2.pop_front();
			}
			for (int i = 0; i != vct.size() - 1; i++) {
				if (vct[i] != vct[i+1] && vct[i] == first && vct[i+1] == next) return it.first;	
			}
		}
	}
}
