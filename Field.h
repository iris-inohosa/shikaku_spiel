/*
Das Modul : Programmierkurs WS20/21
Author: Iris Inokhosa
Das Projekt: SHIKAKU-Spiel
*/

#include <iostream>
#include <vector>
#include <tuple>
#include <ctime>
#include <unistd.h>

using namespace std;

//-----------------RectangleClass------------------//
class Rectangle
{
friend class Field;
private:
	int width = 1;
	int height = 1;
	int area = 1;
	int x = 0;				// x-Koordinate
	int y = 0;				// y-Koordinate
	char sign;			// Zeichen des Rechtecks
	
	// Pruefen ob die Zahl ist Prim
	bool isPrime(int number)
	{
		bool isPrime = true;
		for(int i = 2; i <= number/2; i++)
		{
			if (number % i == 0) isPrime = false;
			return isPrime;
		}
		return isPrime;
	}
	
	// Zufaelliger Auswahl -> width/height or height/width
	tuple <int, int> randSwap(int a, int b)
	{
		int r = rand() % 2;
		if (r == 0) return make_tuple(a,b);
		else return make_tuple(b, a);
	}
	
	// alle Faktoren bestimmne
	// return einen zufaelligen Faktor
	int areaFactors(int area)
	{
		vector<int> factors;
		for(int i = 2; i <= area/2; i++ )
		{
			if(area % i == 0) factors.push_back(i);
		}
		int idx = rand()%factors.size();
	return factors[idx];
	}

public:

	// Konstruktor 1
	Rectangle()
	{

	}
	
	// Konstruktor 2
	Rectangle(int rectArea, char ch)
	{
		sign = ch;
		area = rectArea;
		if (isPrime(rectArea))
		{
			tie(width, height) = randSwap(rectArea, 1);
		}
		else
		{
			width = areaFactors(rectArea);
			height = rectArea/width;
		}

	}
	
	// Konstruktor 3
	Rectangle(int w, int h, char ch)
	{
		sign = ch;
		width = w;
		height = h;
		area = w*h;
	}
	
	// Getter
	int getWidth()
	{
		return width;
	}
	
	int getHeight()
	{
		return height;
	}
		
	int getArea()
	{
		return area;
	}
	
	char getSign()
	{
		return sign;
	}
	
	int getX()
	{
		return x;
	}
	
	int getY()
	{
		return y;
	}
	
	// Setter
	void updateArea(int a)
	{
		area += a;
	}
	
	void rotate()
	{
		// drehung
		int temp;
		temp = width;
		width = height;
		height = temp;
	}

}; // end class Rectangle


//-----------------FieldClass------------------//
class Field
{
public:
	int size;
	vector<vector<string>> field;
	vector <string> row;
	string freeCell = " ";
	int xCoord = 0;				// Koordinaten des Kursors
	int yCoord = 2;
	vector<string> xyGrid;		// Vektor mit Koordinaten xy, als string fuer "geloeschte" Grenzen

	// Konstruktor 1 
	Field()
	{
		
	}

	// Konstruktor
	Field(int customSize)
	{
		size = customSize;
		for(int i = 0; i < size; i++)
		{
			for(int j = 0; j < size; j++)
			{
				row.push_back(freeCell);
			}
			field.push_back(row);
		}
		
	}
	
	//setter
	void setFieldSize(int customSize)
	{
		size = customSize;
		for(int i = 0; i < size; i++)
		{
			for(int j = 0; j < size; j++)
			{
				row.push_back(freeCell);
			}
			field.push_back(row);
		}
	}
	
	// Das Fels ausdrueken
	void print()
	{
		cout << " ------------- Die Belegung -------------- \n";
		cout << "\x1B[35mxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\033[0m\n";
		for(int i = 0; i < size*2; i++)
		{
			cout << "\x1B[35mx \033[0m";
			for(int j = 0; j < size*2; j++)
			{	
				if((i % 2 == 0) && (j % 2 != 0) && (!(isErased(i,j)))) cout << "\x1B[35m|\033[0m";			// Falls die Koordinaten i,j (y,x) in vektor xyGrid sind, gelten die als vom Nutzer geloschten
				else if((i % 2 != 0) && (j % 2 == 0)  && (!(isErased(i,j)))) cout << "\x1B[35m...\033[0m";	// "Geloeschte" grenzen werden hier nicht ausdruecken
				else if((i % 2 != 0) && (j % 2 != 0)) cout << "\x1B[35m.\033[0m";
				else if((i % 2 == 0) && (j % 2 == 0))
				{
					if(field[i/2][j/2].size() == 2) cout <<"\e[1m"  << field[i/2][j/2] << " \e[0m";
					else cout <<"\e[1m "  << field[i/2][j/2] << " \e[0m";									// fuer alles i, j - gerade -> platziere die Zahlen, sonst die Grenzen
				}
				if((i % 2 == 0) && (j % 2 != 0) && (isErased(i,j))) cout << " ";
				else if((i % 2 != 0) && (j % 2 == 0)  && (isErased(i,j))) cout << "   ";
				if(i == yCoord && j == xCoord) cout <<"\e[1m" << "\x1B[33m\b$\033[0m" << "\e[0m"; 
			}
			cout << "\x1B[35mx\033[0m";
			cout << endl;
		}
		cout << "\x1B[35mxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\033[0m\n";
	}
	
	// Zerlegen auf Basis von txt-Datei
	void divideByFile(vector<string> file)					// uebergabe der txt-Datei in Form eines Vektores
	{
		int k = 0;
		for(int i = 0; i < size; i++)
		{
			for(int j = 0; j < size;j++)
			{
				field[i][j] = file[k];						// Schreibe die Werte aufs Feld
				k++;
			}
		}
	}

	// Rechteck platzieren
	bool placeRect(Rectangle &rect, int x, int y)
	{
		if((x + rect.width <= size) && (y + rect.height <= size))			// wenn rechte Grenze nicht erreicht wurde,
		{																	// dann platzieren
			for(int i = y; i < rect.height + y; i++)						
			{
				for(int j = x; j < rect.width + x; j++)
				{
					if(field[i][j] != freeCell) return false;				// pruefe ob Kaestchen frei ist
				}
			}
			
			//Fall alle Kaestchen fuer das Rechteck frei sind, schreibe die Werte ("ABCD...etc") rein
			for(int i = y; i < rect.height + y; i++)						
			{
				for(int j = x; j < rect.width + x; j++)
				{
					field[i][j] = rect.sign;
				}
			}
			rect.x = x;
			rect.y = y;
			return true;
		}
		else if((x + rect.width <= size) && (y + rect.height <= size))		// Fall die Kaestchen sind besetzt, drehen und versuchen noch mal
		{
			// drehung
			rect.rotate();
			
			for(int i = y; i < rect.height + y; i++)
			{
				for(int j = x; j < rect.width + x; j++)
				{
					if(field[i][j] != freeCell) return false;				// pruefe ob Kaestchen frei ist
				}
			}
			
			//Fall alle Kaestchen fuer das Rechteck frei sind, schreibe die Werte ("ABCD...etc") rein
			for(int i = y; i < rect.height + y; i++)
			{
				for(int j = x; j < rect.width + x; j++)
				{
					field[i][j] = rect.sign;
				}
			}
			rect.x = x;
			rect.y = y;
			return true;
		}
		else return false;						// Fall platzierung war nicht moeglich, return false und warte auf ein neues Rechteck
	}
	
	// Suche nach erste freie Zelle, sonst gebe zuerueck x = -1, y = -1 -> als nicht gefunden
	tuple <int, int> findFirstFreeCell()
	{
		int x = -1;
		int y = -1;
		for(int i = 0; i < size; i++)
		{
			for(int j = 0; j < size; j++)
			{
				if(field[i][j] == freeCell)
				{
					int x = j;
					int y = i;
					return make_tuple(x,y);
				}
			}
		}
		return make_tuple(x,y);			// Gebe zurueck ein Paar x,y
	}
	
	// Bewegung auf dem Feld
	// Lese ein Zeichen aus und wahle eine Funktion
	void move(char dir)
	{
		switch(dir)
		{
			case 'w':
				if(yCoord != 0) yCoord-=1;			// Kursor nach oben bewegen
				break;
			case 'a':
				if(xCoord != 0) xCoord-=1;			// Kursor nach links bewegen
				break;
			case 's':
				if(yCoord != size*2-1) yCoord+=1;	// Kursor nach unten bewegen
				break;
			case 'd':
				if(xCoord != size*2-1) xCoord+=1;	// Kursor nach rechts bewegen
				break;
		}
	}
	
	// loschen eine Linie des Gitters
	void erase()
	{
		string xy = to_string(xCoord) + "_" + to_string(yCoord);
		xyGrid.push_back(xy);
	}
	
	// Setzte alle geloeschte Grenzen zurueck
	void reset()
	{
		xyGrid.clear();
	}
	
	// Setzte die letzte geloeschte Grenze zurueck
	void undo()
	{
		if(xyGrid.size() != 0) xyGrid.pop_back();
	}
	
	// Setze zurueck die Grenze in der aktuellen Position des Kursors
	void recover()
	{
		string xy = to_string(xCoord) + "_" +to_string(yCoord);
		auto it = xyGrid.begin();
		while(it != xyGrid.end())
		{
			if(*it == xy)
			{
				xyGrid.erase(it);
				break;
			}
			else ++it;			
		}
	}
	
	// Pruefen ob die Linie geloescht wurde
	bool isErased(int i, int j)
	{
		bool r = false;
		string xy = to_string(j)+ "_" + to_string(i);
		for(auto x: xyGrid)
		{
			if(x == xy)
			{
				r = true;
				break;
			}
		}
		return r;
	}
	
}; //end class Field
	
	
	
	
	
