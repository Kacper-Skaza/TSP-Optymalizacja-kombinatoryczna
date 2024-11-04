#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <climits>
#include <cstdlib>
#include <cmath>
using namespace std;

/*	TODO:
	Zamiana wspolrzednych na macierz sasiedztwa
*/

// Wypisywanie wektora 2D
void printVector2D(vector<vector<int>> &graph)
{
	for (int i=0; i<graph.size(); i++)
	{
		cout<<i+1<<" ";

		for (int j=0; j<graph[i].size(); j++)
		{
			cout<<graph[i][j];
			if (j != graph[i].size()-1)
				cout<<" ";
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
vector<vector<int>> generateCoordinates(int n)
{
	srand(time( nullptr ));
	vector<vector<int>> result;

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

//liczenie odleglosci miedzy punktami i wpisywanie ich do miacierzy
double calculateDistance(int x1, int x2, int y1, int y2){
    double distance;
	distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));  
	return distance;
}

//wpisywanie wartosci do macierzy sasiedztwa
vector<vector<double>> toAdjacencyMatrix(vector<vector<int>> &graph){
	
	int n = graph.size();
    vector<vector<double>> adjMatrix(n, vector<double>(n, 0));

	for (int i=0; i < n; i++){
		for (int j=0; j < n; j++){
			double dist;
			dist = calculateDistance(graph[i][0], graph[j][0], graph[i][1], graph[j][1]);
			adjMatrix[i][j] = dist;
		}
	}
	return adjMatrix;
}

// Szukanie najblizszego miasta
int minDistance(vector<vector<double>> &graph, vector<bool> &visited, int currentCity)
{
	// graph - odleglosci miedzy poszczegolnymi miastami
	// visited - odwiedzone miasta
	// currentCity - numer aktualnego miasta
	double minDist;
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
double tspGreedy(vector<vector<double>> &graph, int startCity)
{
	cout<<"Sciezka: "<<startCity<<", ";

	vector<bool> visited;
	double totalCost;
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
	vector<vector<int>> coordinates; 
    int n=0, m=0, startCity=0;
	double result=0;

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
		cout<<">> Podaj ilosc miast:"<<endl;
		cout<<">> ";
		cin>>m;
		coordinates = generateCoordinates(m);
		cout<<"Wygenerowane dane: "<<endl;
		printVector2D(coordinates);
	}

	vector<vector<double>> graph = toAdjacencyMatrix(coordinates);
	// for (const auto& row : graph) {
    //     cout << "{";
    //     for (size_t i = 0; i < row.size(); ++i) {
    //         cout << row[i];
    //         if (i < row.size() - 1) {
    //             cout << ",";
    //         }
    //     }
    //     cout << "}" << endl; }
	// graph =
	// {
	// 	{0, 10, 15, 20},
	// 	{10, 0, 35, 25},
	// 	{15, 35, 0, 30},
	// 	{20, 25, 30, 0}
	// };

	cout<<">> Wybierz miasto startowe:"<<endl;
	cout<<">> ";
	cin>>startCity;
	cout<<endl;

	if (startCity < graph.size())
	{
		result = tspGreedy(graph, startCity);
		cout<<"Minimalny koszt znaleziony przez algorytm zachlanny: "<<result<< endl;
	}
	else
	{
		cout<<"Miasto startowe nie istnieje !!!";
	}

	return 0;
}

