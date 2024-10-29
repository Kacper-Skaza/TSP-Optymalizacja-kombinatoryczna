#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <climits>
#include <cstdlib>
using namespace std;

/*	TODO:
	Zamiana wspolrzednych na macierz sasiedztwa
*/

// Wypisywanie wektora 2D
void printVector2D(vector<vector<int>> &graph)
{
	for (int i=0; i<graph.size(); i++)
	{
		cout<<i<<": ";

		for (int j=0; j<graph[i].size(); j++)
		{
			cout<<graph[i][j];
			if (j != graph[i].size()-1)
				cout<<", ";
		}

		cout<<endl;
	}

	cout<<endl;
}

// Wczytywanie tablicy wspolrzednych z pliku
vector<vector<int>> readCoordinates(string fileName)
{
	ifstream inp(fileName);
	vector<vector<int>> result;
	int n;
	inp>>n;

	for (int i=0; i<n; i++)
	{
		int a=0, x=0, y=0;
		inp>>a;
		inp>>x;
		inp>>y;
		result.push_back({x, y});
	}

	inp.close();
	return result;
}

// Generowanie tablicy wspolrzednych
vector<vector<int>> generateCoordinates()
{
	srand(time( nullptr ));
	vector<vector<int>> result;
	int n;
	n = (rand()%21) + 40; // Zakres: <40, 60>

	for (int i=0; i<n; i++)
	{
		int x=0, y=0;
		x = (rand()%2500) + 1; // Zakres: <1, 2500>
		y = (rand()%2500) + 1; // Zakres: <1, 2500>

		for (int j=0; j<result.size(); j++)
		{
			if (result[j][0] == x && result[j][1] == y) // Eliminacja powtorzen
			{
				i--;
				continue;
			}
		}

		result.push_back({x, y});
	}

	return result;
}

// Szukanie najblizszego miasta
int minDistance(vector<vector<int>> &graph, vector<bool> &visited, int currentCity)
{
	// graph - odleglosci miedzy poszczegolnymi miastami
	// visited - odwiedzone miasta
	// currentCity - numer aktualnego miasta
	int minDist;
	int nextCity;

	minDist = INT_MAX;
	nextCity = -1;

	for (int i=0; i<graph.size(); i++)
	{
		if (visited[i] == false && graph[currentCity][i] < minDist)
		{
			minDist = graph[currentCity][i];
			nextCity = i;
		}
	}

	return nextCity;
}

// Algorytm zachlanny TSP
int tspGreedy(vector<vector<int>> &graph, int startCity)
{
	cout<<"Sciezka: "<<startCity<<", ";

	vector<bool> visited;
	int totalCost;
	int currentCity;

	visited.assign(graph.size(), false);
	visited[startCity] = true;
	totalCost = 0;
	currentCity = startCity;

	for (int i=0; i<graph.size()-1; i++)
	{
		int nextCity = minDistance(graph, visited, currentCity);
		totalCost += graph[currentCity][nextCity];
		visited[nextCity] = true;
		currentCity = nextCity;
		cout<<currentCity<<", ";
	}

	totalCost += graph[currentCity][startCity]; // Powrot do miasta poczatkowego
	cout<<startCity<<";"<<endl;

	return totalCost;
}



int main()
{
	vector<vector<int>> graph, coordinates; 
    int n=0, result=0, startCity=0;

	while (n != 1 && n != 2)
	{
		cout<<">> Wybierz zrodlo danych:"<<endl;
		cout<<"1 - Plik 'Input.txt'"<<endl;
		cout<<"2 - Generator danych"<<endl;
		cout<<">> ";
		cin>>n;
		cout<<endl;
	}

	if (n == 1)
	{
		coordinates = readCoordinates("Input.txt");
	}
	else if (n == 2)
	{
		coordinates = generateCoordinates();
		cout<<"Wygenerowane dane: "<<endl;
		printVector2D(coordinates);
	}

	graph =
	{
		{0, 10, 15, 20},
		{10, 0, 35, 25},
		{15, 35, 0, 30},
		{20, 25, 30, 0}
	};

	cout<<">> Wybierz miasto startowe:"<<endl;
	cout<<">> ";
	cin>>startCity;
	cout<<endl;

	if (startCity < graph.size())
	{
		result = tspGreedy(graph, startCity);
		cout<<"Minimalny koszt znaleziony przez algorytm zachlanny: "<<result;
	}
	else
	{
		cout<<"Miasto startowe nie istnieje !!!";
	}

	return 0;
}

