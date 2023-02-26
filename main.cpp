#include "Simulator.h"

int main() {
	Simulator* s = new Simulator();
	cout << "Dobrodosli u simulator mreze gradskog prevoza. Molimo Vas, odaberite opciju:" << endl;
	cout << "1. Ucitavanje podataka o mrezi gradskog prevoza" << endl;
	cout << "0. Kraj rada" << endl;

	while (1) {
		int n;
		cin >> n;

		switch (n) {
			case 1: {
				string stationsFilepath;
				cout << "Molimo Vas, unesite putanju do fajla sa stajalistima:" << endl;
				cin >> stationsFilepath;
				try {
					s->readStationsInfo(stationsFilepath);	
				}
				catch (invalidStationFilepath* e) {
					cout << e->what();
					return 0;
				};

				string linesFilepath;
				cout << "Molimo Vas, unesite putanju do fajla sa linijama gradskog prevoza:" << endl;
				cin >> linesFilepath;
				try {
 					s->readLinesInfo(linesFilepath);
				}
				catch (invalidLinesFilepath* e) {
					cout << e->what();
					return 0;
				};	
				break;
			}
			case 0: {
				delete s;
				return 0;
			}
			default: {
				cout << "Uneli ste nepostojecu opciju. Molimo Vas, pokusajte ponovo:" << endl;
				cout << "1. Ucitavanje podataka o mrezi gradskog prevoza" << endl;
				cout << "0. Kraj rada" << endl;
				continue;
			}
		}

		break;
	}

	cout << "Mreza gradskog prevoza je uspesno ucitana. Molimo Vas, odaberite opciju:" << endl; 
	cout << "1. Prikaz informacija o stajalistu" << endl;
	cout << "2. Prikaz osnovnih informacija o liniji gradskog prevoza" << endl;
	cout << "3. Prikaz statistickih informacija o liniji gradskog prevoza" << endl;
	cout << "4. Pronalazak putanje izmedju dva stajalista" << endl;
	cout << "0. Kraj rada" << endl;

	while (1) {
		int n; 
		cin >> n;

		switch (n) {
			case 1: {
				int num;
				cout << "Molimo Vas, unesite sifru stajalista." << endl;
				cin >> num;
				try {
					s->getStationInfo(num);
					cout << "Kreiran je fajl stajaliste_" << num << ".txt sa informacijama o stajalistu sa sifrom " << num << "." << endl;
				}
				catch (int) {
					cout << "Uneta sifra stajalista " << num << " ne postoji u sistemu! Molimo vas pokusajte ponovo." << endl;
				}
				break;
			}
			case 2: {
				string line;
				cout << "Molimo Vas, unesite oznaku linije." << endl;
				cin >> line;
				try {
					s->getLineInfo(line);
					cout << "Kreiran je fajl linija_" << line << ".txt sa informacijama o liniji " << line << "." << endl;
				}
				catch (string) {
					cout << "Linija " << line << " ne postoji u sistemu. Molimo Vas, pokusajte ponovo." << endl;
				};
				break;
			}
			case 3: {
				string line;
				cout << "Molimo Vas, unesite oznaku linije." << endl;
				cin >> line;
				try {
					s->getLineStats(line);
					cout << "Kreiran je fajl statistika_" << line << ".txt sa statistikom o liniji " << line << "." << endl;
				}
				catch (string) {
					cout << "Linija " << line << " ne postoji u sistemu. Molimo Vas, pokusajte ponovo." << endl;
				};
				break;
			}
			case 4: {
				int start, end;
				cout << "Molimo Vas, unesite sifru pocetnog stajalista." << endl;
				cin >> start;
				cout << "Molimo Vas, unesite sifru krajnjeg stajalista." << endl;
				cin >> end;

					int n;
					cout << "Molimo Vas, unesite vrstu putanje:" << endl;
					cout << "1. Bilo koja putanja izmedju datih stajalista" << endl;
					cout << "2. Putanja sa najmanje stajalista izmedju datih stajalista" << endl;
					cout << "3. Putanja sa najmanje presedanja izmedju datih stajalista" << endl;
					cin >> n;
					switch (n) {
						case 1: {
							try {
								s->findShortestRoute(start, end);
								cout << "Kreiran je fajl putanja_" << start << "_" << end << ".txt sa putanjom izmedju stajalista " << start << " i " << end << "." << endl;
							}
							catch (int) {
								cout << "Greska! Uneli ste nepostojecu sifru stanice!" << endl;
								break;
							}
							catch (bool) {
								cout << "Stajalista " << start << " i " << end << " nisu povezana!" << endl;
								break;
							}
							break;
						}
						case 2: {
							try {
								s->findShortestRoute(start, end);
								cout << "Kreiran je fajl putanja_" << start << "_" << end << ".txt sa najkracom putanjom izmedju stajalista " << start << " i " << end << "." << endl;
							}
							catch (int) {
								cout << "Greska! Uneli ste nepostojecu sifru stanice!" << endl;
								break;
							}
							catch (bool) {
								cout << "Stajalista " << start << " i " << end << " nisu povezana!" << endl;
								break;
							}
							break;
						}
						case 3: {
							try {
								s->findLightestRoute(start, end);
								cout << "Kreiran je fajl putanja_" << start << "_" << end << ".txt sa putanjom sa najmanje presedanja izmedju stajalista " << start << " i " << end << "." << endl;
								break;
							}
							catch (int) {
								cout << "Greska! Uneli ste nepostojecu sifru stanice!" << endl;
								break;
							}
							catch (bool) {
								cout << "Stajalista " << start << " i " << end << " nisu povezana!" << endl;
								break;
							}
							break;
						}
						default: {
							cout << "Uneli ste nepostojecu opciju. Pokusajte ponovo:" << endl;
							continue;
						}
					}

				break;
			}
			case 0: {
				delete s;
				return 0;
			}
			default: {
				cout << "Uneli ste nepostojecu opciju. Molimo Vas, pokusajte ponovo:" << endl;
				cout << "1. Prikaz informacija o stajalistu" << endl;
				cout << "2. Prikaz osnovnih informacija o liniji gradskog prevoza" << endl;
				cout << "3. Prikaz statistickih informacija o liniji gradskog prevoza" << endl;
				cout << "4. Pronalazak putanje izmedju dva stajalista" << endl;
				cout << "0. Kraj rada" << endl;
				continue;
			}
		}

		cout << "Molimo Vas, odaberite narednu opciju:" << endl;
		cout << "1. Prikaz informacija o stajalistu" << endl;
		cout << "2. Prikaz osnovnih informacija o liniji gradskog prevoza" << endl;
		cout << "3. Prikaz statistickih informacija o liniji gradskog prevoza" << endl;
		cout << "4. Pronalazak putanje izmedju dva stajalista" << endl;
		cout << "0. Kraj rada" << endl;
	}
}