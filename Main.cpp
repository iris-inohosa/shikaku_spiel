#include <iostream>
#include "Field.h"
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <tuple>
#include <map>
#include <iterator>
#include <iomanip>
#include <stdio.h>
#include <fstream>

using namespace std;


//---------------------Methods-----------------------//
bool readFile(string fileName, vector<string> &partitionField);
int randomNumber();
Field generateNewField(map<string, Rectangle> &partition, int size);
Field replaceWithNumbers(map<string, Rectangle> partition, int size);
void showSolution(Field f);
void displayMenu();
void play(Field &playField, Field solutionField, char select, int size, map<string, Rectangle> &partition);
void play(Field &playField, char select);


//----------------------- MAIN ----------------------//
int main()
{
	int size = 10;
	vector<string> partitionFile;
	string fileName;
	map<string, Rectangle> partition;	
	srand(time(NULL));
	Field playField;
	Field solutionField;
	unsigned choice;
	char select;
	
	// random genarator nano Sekunden statt sekunden
	struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand((time_t)ts.tv_nsec);
	

	//------------------User Input------------------//
	
	cout << "\n  -------------- SHIKAKU ---------------- \n" << endl;
	while(true)
	{
		cout << "Wählen Sie eine Funktion aus: \n";
		cout << "1 - Externe Datei auslesen \n";
		cout << "2 - Die Anordnung generieren \n";
		cout << "3 - Das Spiel beenden \n";
		cin >> choice;
		if(choice == 1 || choice == 2 || choice == 3) break;
	}

	switch(choice)
	{
		// Fall 1 : Die Belegung auszulesen
		case 1:
			cout << "Name der Quelldatei: " << endl;
			cin >> fileName;
	
			// Datei auslesen
			if(!(readFile(fileName, partitionFile)))
			{
				cout << "Die Datei kann nicht gelesen werden" << endl;
				exit(0);
			}
			size = stoi(partitionFile[0]);							// Die erste Zeile zeigt die groesse
			partitionFile.erase(partitionFile.begin());				
			for(string &str : partitionFile)						// \n Zeichen ausschneiden
			{
				str.erase(remove(str.begin(), str.end(), '\r'), str.end());
				str.erase(remove(str.begin(), str.end(), '\n'), str.end());
			}	
			playField.setFieldSize(size);								// Das Feld der gegebenen groesse

			//Prüfen ob die gegebene Datei richtig sind 
			if(partitionFile.size() == pow(size, 2))
			{
				playField.divideByFile(partitionFile);
				playField.print();
				play(playField, select);
			}
			else 
			{
				cout << "Die Datei ist falsch definiert!";
				cout << partitionFile.size();
				exit(0);
			}
			break;
			
		// Fall 2 : Die Belegung generieren	
		case 2:

			solutionField = generateNewField(partition, size);
			playField = replaceWithNumbers(partition, size);
			play(playField, solutionField, select, size, partition);
			break;
		
		// Fall 3 : Abschliessen
		case 3:
			cout << "Das Spiel beenden" << endl;
			exit(0);
	}


	return 0;
}

// Generiere eine Belegung
Field generateNewField(map<string, Rectangle> &partition, int size)
{
	partition.clear();
	Field customField(size);
	int x;																// x - Koordinate des Kästchens
	int y;																// y - Koordinate des Kästchens
	int area;															// flaeche des Rechtecks
	string albet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ*+#&?%123456789-0=";		// alle Mögliche Schluessel
	int i = 0;															// Index des Buchstaben
	bool placed;														// True -> fall den Rechteck platziert werden könnte, sonst False
	Rectangle rect = Rectangle();										// Default-Instantz der Klasse "Rechteck"
	int sum = 0;
	
	while(true)
	{
		/*
		Sobald keine freie Zelle mehr gibt, generiere ein Rechteck zufaelliger Flache und Groesse
		Versuche das zu platzieren, sonst geenriere ein neues Rechteck und versuche noochmal
		Fall es einzelne Kaestchen gibt (dh. die Zelle links und recht sind besetzt) geberiere ein Rechteck 1xh
		, versuche das zu platzieren, sonst hinzuefuge das zum Nachbaren mit der Hoehe bzw Breite == 1
		Gebe zurueck ein Feld mit Partitionen, das wird spaeter fuer funktion "show solution" benutzt 
		*/
		tie(x, y) = customField.findFirstFreeCell();											// Suche auf die Koordinaten der ersten freien Zelle, von links nach rechts, von oben nach unten
		if(x == -1) break;																		// Falls die Koordinaten nicht gefunden, das Feld ist komplett besetzt

		if(((y == 9) && (x == 9)) || ((y == 9) && (customField.field[y][x+1] != customField.freeCell)))			// Falls nur einzelne Zelle ist gebllieben, dann die zum Rechteck 1xh, oder wx1 mergen
		{
			if(x == 0)																			// fuer die Zelle mit Koordinaten [0, 9]. Mergen die zum oberen Rechteck
			{
				customField.field[y][x] = customField.field[y - 1][x];							
				string key = customField.field[y - 1][x];
				partition[key].updateArea(1);
				placed = false;
			}				
			else if((customField.field[y - 1][x] == customField.field[y - 2][x]) && (customField.field[y - 1][x] != customField.field[y - 1][x - 1]))
			{
				customField.field[y][x] = customField.field[y - 1][x];							// fall nur ein Kaestchen und das obere Rechteck
				string key = customField.field[y - 1][x];										// die Breite 1 hat, hinzuefuege zum
				partition[key].updateArea(1);													// und vergroese die entsprechende Flaeche auf 1
				placed = false;
			}
			else
			{																					// falls nur ein Kaestchen und das obere Rechteck die Breite
				customField.field[y][x] = customField.field[y][x-1];							// groesser als 1 hat					
				string key = customField.field[y][x-1];											// hinzufuege das Kaestchen zum linken Nachbarn
				partition[key].updateArea(1);													// vergroese die entsprechende Flaeche auf 1
				placed = false;																	
			}	
		}
		else if((x == 9) || (customField.field[y][x+1] != customField.freeCell))					// Falls das letzte Zelle in der Zeile, oder rechter Nachbar ist besetzt
		{																		// Generiere Rechteck 1xh
			int dist = size - y - 1; 											// maximale moegliche Laenge bis zur Grenze
			int h = rand()%dist+2;
			rect = Rectangle(1, h, albet[i]);									
			placed = customField.placeRect(rect, x, y);							// versuche zu platzieren
		}
		else																	// Sonst generiere ein Rechteck zufaelliger Hoehe und Groesse
		{
			area = rand()%11+2;													// zufallzahlen zwischen 2 und 12
			rect = Rectangle(area, albet[i]);
			placed = customField.placeRect(rect, x, y);							// versuche zu platzieren
		}			

		if(placed)																// Falls erfolgreich platziert
		{												
			string key (1, albet[i]);
			partition[key] = rect;												// Speichere Rechteck ins Woerterbuch
			i++;																// Waehle den naechsten Buchstaben
		}
	}
	
	return customField;
}

// Ersetze mit den Zahle/Flaechen
Field replaceWithNumbers(map<string, Rectangle> partition, int size)
{
	/*
	Die Methode geht durch jeden platzierten Rechteck, liest die Position
	und die Groesse aus, und schreibt auf ein zufaelliges Kaestchen innerhalb des Rechtecks
	die Faeche rein
	*/

	int xPos;
	int yPos;
	Field numbersField(size);
	int i = 0;
	
	map<string, Rectangle>::iterator it = partition.begin();				// ein Iterator fuer das Woerterbuch
    for (pair<string, Rectangle> element : partition) 
	{
        string key = element.first;
        Rectangle rect = element.second;
        if(rect.getWidth() == 1) xPos = rect.getX();						// Falls Rechteck ist 1xh -> x = rect.x, y = rand(hoehe)+offset
		else xPos = rand()%rect.getWidth() + rect.getX();
		if(rect.getHeight() == 1) yPos = rect.getY();						// Falls Rechteck ist wx1 -> y = rect.y, x = rand(breite)+offset
		else yPos = rand()%rect.getHeight() + rect.getY();
		numbersField.field[yPos][xPos] = to_string(rect.getArea());
		i++;
    }
	return numbersField;
}

// Zeigen die Loesung bzw. das generierte Feld
void showSolution(Field f)
{
	cout << "\n -------------- Die Lösung ------------- \n";
	f.print();
}

// die Anordnung von txt-datea auslesen
bool readFile(string fileName, vector<string> &partitionFile)
{
	ifstream newFile;
	string str;
	
	newFile.open(fileName.c_str());
	if(!newFile)
	{
		cerr << fileName << " - Datei nicht gefunden";
		exit(-1);
	}
	while(getline(newFile, str, '|'))
	{
		partitionFile.push_back(str);
	}
	newFile.close();
	return true;
}

// zeige die Menu-auswahl
void displayMenu()
{
   cout << "Optionen:\n"
        << "   wasd - Kursor bewegen\n"
		<< "   Leerzeichen - Grenze loeschen\n"
        << "   r - alle Grenze zurücksetzen\n"
		<< "   z - Undo\n"
		<< "   q - Aktuelle Grenze zurücksetzen\n"
		<< "   l - Lösung zeigen\n"
		<< "   k - Lösung ausblenden\n"
		<< "   g - Neu generieren\n"
		<< "   x - Beenden\n"
        << endl;
   return;
}

void play(Field &playField, Field solutionField, char select, int size, map<string, Rectangle> &partition)
{
	while(select != 'x')
	{
		system("clear");								
		playField.print();
		displayMenu();
		system("stty raw");
		select = getchar();
		system("stty cooked");
		if(select == 'w') playField.move('w');				// Kursor nach oben bewegen
		else if(select == 'a') playField.move('a');			// Kursor nach links bewegen
		else if(select == 's') playField.move('s');			// Kursor nach unten bewegen
		else if(select == 'd') playField.move('d');			// Kursor nach recht bewegen
		else if(select == ' ') playField.erase();			// Grenze loeschen
		else if(select == 'r') playField.reset();			// Alle Grenze zuruecksetzen
		else if(select == 'z') playField.undo();			// die letzte geloeschte Grenze zuruecksetzen
		else if(select == 'q') playField.recover();			// die aktuelle geloeschte Grenze zuruecksetzen
		else if(select == 'g')								// Eine neue Belegung generieren
		{
			solutionField = generateNewField(partition, size);
			playField = replaceWithNumbers(partition, size);
		}
		while(select == 'l')
		{
			system("clear");
			showSolution(solutionField);
			displayMenu();
			system("stty raw");
			select = getchar();
			system("stty cooked");
			displayMenu();
		}
	}	
}

// play overload fuer externe belegungen
void play(Field &playField, char select)
{
	while(select != 'x')
	{
		system("clear");								
		playField.print();
		displayMenu();
		system("stty raw");
		select = getchar();
		system("stty cooked");
		if(select == 'w') playField.move('w');				// Kursor nach oben bewegen
		else if(select == 'a') playField.move('a');			// Kursor nach links bewegen
		else if(select == 's') playField.move('s');			// Kursor nach unten bewegen
		else if(select == 'd') playField.move('d');			// Kursor nach recht bewegen
		else if(select == ' ') playField.erase();			// Grenze loeschen
		else if(select == 'r') playField.reset();			// Alle Grenze zuruecksetzen
		else if(select == 'z') playField.undo();			// die letzte geloeschte Grenze zuruecksetzen
		else if(select == 'q') playField.recover();			// die aktuelle geloeschte Grenze zuruecksetzen
		while(select == 'l' || select == 'g')
		{
			system("clear");
			cout << "- NOT AVAILABLE -";
			system("stty raw");
			select = getchar();
			system("stty cooked");
		}
	}	
}







