#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <climits>
#include <cstdlib>
#include <cmath>
using namespace std;

const int MAX_CITIES = 5;        // Liczba miast (zmieñ na odpowiedni¹)
const int NUM_ANTS = 10;         // Liczba mrówek
const int MAX_ITERATIONS = 100;  // Maksymalna liczba iteracji
const double ALPHA = 1.0;        // Waga œladu feromonowego
const double BETA = 5.0;         // Waga heurystyki (odleg³oœci)
const double RHO = 0.5;          // Wspó³czynnik parowania feromonów
const double Q = 100.0;          // Sta³a u¿ywana do aktualizacji feromonów

// Funkcja liczaca odleg³osc (tu euklidesowa)
double distance(pair<int, int> city1, pair<int, int> city2) {
    return sqrt(pow(city1.first - city2.first, 2) + pow(city1.second - city2.second, 2));
}

// Inicjalizowanie grafu miasta (macierz odleg³oœci)
void initializeGraph(vector<vector<double>>& graph, vector<pair<int, int>>& cities) {
    for (int i = 0; i < MAX_CITIES; i++) {
        for (int j = 0; j < MAX_CITIES; j++) {
            if (i != j) {
                graph[i][j] = distance(cities[i], cities[j]);
            } else {
                graph[i][j] = 0.0;
            }
        }
    }
}

// Funkcja wyboru nastêpnego miasta na podstawie prawdopodobieñstwa
int selectNextCity(int currentCity, const vector<vector<double>>& pheromone, const vector<vector<double>>& graph, vector<bool>& visited) {
    vector<double> probabilities(MAX_CITIES, 0.0);
    double sum = 0.0;

    for (int i = 0; i < MAX_CITIES; i++) {
        if (!visited[i]) {
            probabilities[i] = pow(pheromone[currentCity][i], ALPHA) * pow(1.0 / graph[currentCity][i], BETA);
            sum += probabilities[i];
        }
    }

    double randVal = ((double) rand() / RAND_MAX) * sum;
    double cumulative = 0.0;

    for (int i = 0; i < MAX_CITIES; i++) {
        if (!visited[i]) {
            cumulative += probabilities[i];
            if (cumulative >= randVal) {
                return i;
            }
        }
    }

    return -1;  // B³¹d
}

// Funkcja wykonuj¹ca trasê dla jednej mrówki
double antTravel(int startCity, const vector<vector<double>>& graph, vector<vector<double>>& pheromone, vector<int>& bestPath, double& bestLength) {
    vector<bool> visited(MAX_CITIES, false);
    vector<int> path;
    int currentCity = startCity;
    double pathLength = 0.0;

    path.push_back(currentCity);
    visited[currentCity] = true;

    for (int i = 1; i < MAX_CITIES; i++) {
        int nextCity = selectNextCity(currentCity, pheromone, graph, visited);
        path.push_back(nextCity);
        visited[nextCity] = true;
        pathLength += graph[currentCity][nextCity];
        currentCity = nextCity;
    }

    pathLength += graph[currentCity][startCity];  // Powrót do miasta pocz¹tkowego
    path.push_back(startCity);

    // Sprawdzanie, czy znaleziono lepsz¹ trasê
    if (pathLength < bestLength) {
        bestPath = path;
        bestLength = pathLength;
    }

    return pathLength;
}

// Aktualizacja feromonów
void updatePheromones(vector<vector<double>>& pheromone, vector<vector<int>>& paths, vector<double>& lengths) {
    // Parowanie feromonów
    for (int i = 0; i < MAX_CITIES; i++) {
        for (int j = 0; j < MAX_CITIES; j++) {
            pheromone[i][j] *= (1.0 - RHO);
        }
    }

    // Dodawanie nowych feromonów
    for (int ant = 0; ant < NUM_ANTS; ant++) {
        for (int i = 0; i < MAX_CITIES - 1; i++) {
            int cityA = paths[ant][i];
            int cityB = paths[ant][i + 1];
            pheromone[cityA][cityB] += Q / lengths[ant];
            pheromone[cityB][cityA] += Q / lengths[ant];  // Symetryczny graf
        }
    }
}

int mainmrowka() {
    srand(time( NULL ));

    // Lista miast (x, y)
    vector<pair<int, int>> cities = {{0, 0}, {1, 5}, {5, 3}, {6, 7}, {8, 2}};
    
    // Macierz odleg³oœci i feromonów
    vector<vector<double>> graph(MAX_CITIES, vector<double>(MAX_CITIES, 0.0));
    vector<vector<double>> pheromone(MAX_CITIES, vector<double>(MAX_CITIES, 1.0));

    initializeGraph(graph, cities);

    vector<int> bestPath;
    double bestLength = numeric_limits<double>::max();

    for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
        vector<vector<int>> paths(NUM_ANTS);
        vector<double> lengths(NUM_ANTS);

        // Symulacja dla ka¿dej mrówki
        for (int ant = 0; ant < NUM_ANTS; ant++) {
            int startCity = rand() % MAX_CITIES;
            lengths[ant] = antTravel(startCity, graph, pheromone, bestPath, bestLength);
            paths[ant] = bestPath;
        }

        // Aktualizacja feromonów
        updatePheromones(pheromone, paths, lengths);
    }

    // Wyœwietlanie najlepszego rozwi¹zania
    cout << "Najlepsza znaleziona trasa: ";
    for (int city : bestPath) {
        cout << city << " ";
    }
    cout << "\nD³ugoœæ trasy: " << bestLength << endl;

    return 0;
}

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
		coordinates = readCoordinates("Input/Input.txt");
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

