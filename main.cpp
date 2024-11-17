#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <limits>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <ctime>
using namespace std;

/*	TODO:
	Milej zabawy :/
*/

// CZAS
long long Get_time()
{
	return chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
}

// STROJENIE
const int NUM_ANTS = 10;			// Liczba mrowek
const int MAX_ITERATIONS = 100;		// Maksymalna liczba iteracji
const double ALPHA = 1.0;			// Waga sladu feromonowego
const double BETA = 5.0;			// Waga heurystyki (odleglosci)
const double RHO = 0.5;				// Wspolczynnik parowania feromonow
const double Q = 100.0;				// Stala uzywana do aktualizacji feromonow





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

// Liczenie odleglosci miedzy punktami
double calculateDistance(int x1, int x2, int y1, int y2)
{
	return sqrt(pow(x2-x1, 2) + pow(y2-y1, 2));
}

// Wpisywanie wartosci do macierzy sasiedztwa
vector<vector<double>> toAdjacencyMatrix(vector<vector<int>> &graph)
{
	int n = graph.size();
	vector<vector<double>> adjMatrix(n, vector<double>(n, 0.0));

	for (int i=0; i<n; i++)
	{
		for (int j=0; j<n; j++)
		{
			adjMatrix[i][j] = calculateDistance(graph[i][0], graph[j][0], graph[i][1], graph[j][1]);
		}
	}

	return adjMatrix;
}

// Wczytywanie tablicy wspolrzednych z pliku
vector<vector<int>> readCoordinates(string fileName)
{
	ifstream inp(fileName);
	vector<vector<int>> result(0);
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
	vector<vector<int>> result(0);

	for (int i=0; i<n; i++)
	{
		int x=0, y=0;
		x = (rand()%1000) + 1; // Zakres: <1, 1000>
		y = (rand()%1000) + 1; // Zakres: <1, 1000>

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
int minDistance(vector<vector<double>> &graph, vector<bool> &visited, int currentCity)
{
	// graph - odleglosci miedzy poszczegolnymi miastami
	// visited - odwiedzone miasta
	// currentCity - numer aktualnego miasta
	double minDist = numeric_limits<double>::max();
	int nextCity = -1;

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
double tspGreedy(vector<vector<double>> &graph, bool showPath)
{
	if (showPath == true)
		cout<<endl<<"Znaleziona trasa: 1, ";

	double totalCost = 0.0;
	int currentCity = 0;
	vector<bool> visited(graph.size(), false);
	visited[currentCity] = true;

	for (int i=1; i<graph.size(); i++)
	{
		int nextCity = minDistance(graph, visited, currentCity);
		totalCost += graph[currentCity][nextCity];
		visited[nextCity] = true;
		currentCity = nextCity;
		if (showPath == true)
			cout<<currentCity+1<<", ";
	}

	totalCost += graph[currentCity][0]; // Powrot do miasta poczatkowego
	if (showPath == true)
		cout<<"1;"<<endl;

	return totalCost;
}





// Funkcja wyboru nastepnego miasta na podstawie prawdopodobienstwa
int selectNextCity(int currentCity, vector<vector<double>> &pheromone, vector<vector<double>> &graph, vector<bool> &visited)
{
	int n = graph.size();

	vector<double> probabilities(n, 0.0);
	double sum = 0.0;

	for (int i=0; i<n; i++)
	{
		if (visited[i] == false)
		{
			probabilities[i] = pow(pheromone[currentCity][i], ALPHA) * pow(1.0 / graph[currentCity][i], BETA);
			sum += probabilities[i];
		}
	}

	double randVal = ((double) rand() / RAND_MAX) * sum;
	double cumulative = 0.0;

	for (int i=0; i<n; i++)
	{
		if (visited[i] == false)
		{
			cumulative += probabilities[i];
			if (cumulative >= randVal)
				return i;
		}
	}

	return -1; // Error
}

// Funkcja wykonujaca trase dla jednej mrowki
double antTravel(int startCity, vector<vector<double>> &graph, vector<vector<double>> &pheromone, vector<int> &bestPath, double &bestLength)
{
	int n = graph.size();

	vector<bool> visited(n, false);
	vector<int> path(0);
	int currentCity = startCity;
	double pathLength = 0.0;

	path.push_back(currentCity);
	visited[currentCity] = true;

	for (int i=1; i<n; i++)
	{
		int nextCity = selectNextCity(currentCity, pheromone, graph, visited);
		path.push_back(nextCity);
		visited[nextCity] = true;
		pathLength += graph[currentCity][nextCity];
		currentCity = nextCity;
	}

	pathLength += graph[currentCity][startCity]; // Powrot do miasta pocz¹tkowego
	path.push_back(startCity);

	// Sprawdzanie, czy znaleziono lepsza trase
	if (pathLength < bestLength)
	{
		bestPath = path;
		bestLength = pathLength;
	}

	return pathLength;
}

// Aktualizacja feromonow
void updatePheromones(vector<vector<double>> &pheromone, vector<vector<int>> &paths, vector<double> &lengths)
{
	int n = pheromone.size();

	// Parowanie feromonow
	for (int i=0; i<n; i++)
	{
		for (int j=0; j<n; j++)
		{
			pheromone[i][j] *= (1.0 - RHO);
		}
	}

	// Dodawanie nowych feromonow
	for (int ant=0; ant<NUM_ANTS; ant++)
	{
		for (int i=0; i<n-1; i++)
		{
			int cityA = paths[ant][i];
			int cityB = paths[ant][i+1];
			pheromone[cityA][cityB] += Q / lengths[ant];
			pheromone[cityB][cityA] += Q / lengths[ant]; // Symetria
		}
	}
}

// Algorytm mrowkowy TSP
double tspAntColony(vector<vector<double>> &graph, bool showPath)
{
	srand(time( nullptr ));

	int n = graph.size();

	vector<vector<double>> pheromone(n, vector<double>(n, 1.0));

	vector<int> bestPath;
	double bestLength = numeric_limits<double>::max();

	for (int i=0; i<MAX_ITERATIONS; i++)
	{
		vector<vector<int>> paths(NUM_ANTS);
		vector<double> lengths(NUM_ANTS);

		// Symulacja dla kazdej mrowki
		for (int ant=0; ant<NUM_ANTS; ant++)
		{
			int startCity = rand() % n;
			lengths[ant] = antTravel(startCity, graph, pheromone, bestPath, bestLength);
			paths[ant] = bestPath;
		}

		// Aktualizacja feromonow
		updatePheromones(pheromone, paths, lengths);
	}

	if (showPath == true)
	{
		cout<<endl<<"Znaleziona trasa:";
		for (int city : bestPath)
		{
			cout << city << " ";
		}
	}

	return bestLength;
}





int main()
{
	srand(time( nullptr ));

	vector<vector<int>> coordinates(0);
	vector<vector<double>> graph(0);
	long long time_1=0, time_2=0;
	double result=0;
	int n=0, m=0;

	while (n < 1 || n > 8)
	{
		cout<<">> Wybierz zrodlo danych:"<<endl;
		cout<<"1 - Ranking: 'berlin52.txt'"<<endl;
		cout<<"2 - Ranking: 'bier127.txt'"<<endl;
		cout<<"3 - Ranking: 'tsp250.txt'"<<endl;
		cout<<"4 - Ranking: 'tsp500.txt'"<<endl;
		cout<<"5 - Ranking: 'tsp1000.txt'"<<endl;
		cout<<"6 - Plik 'Test.txt'"<<endl;
		cout<<"7 - Plik 'Input.txt'"<<endl;
		cout<<"8 - Generator danych"<<endl;
		cout<<">> ";
		cin>>n;
		cout<<endl;
	}

	if (n == 1)
	{
		coordinates = readCoordinates("Data/Ranking/berlin52.txt");
	}
	else if (n == 2)
	{
		coordinates = readCoordinates("Data/Ranking/bier127.txt");
	}
	else if (n == 3)
	{
		coordinates = readCoordinates("Data/Ranking/tsp250.txt");
	}
	else if (n == 4)
	{
		coordinates = readCoordinates("Data/Ranking/tsp500.txt");
	}
	else if (n == 5)
	{
		coordinates = readCoordinates("Data/Ranking/tsp1000.txt");
	}
	else if (n == 6)
	{
		coordinates = readCoordinates("Data/Test.txt");
	}
	else if (n == 7)
	{
		coordinates = readCoordinates("Data/Input.txt");
	}
	else if (n == 8)
	{
		cout<<">> Podaj ilosc miast:"<<endl;
		cout<<">> ";
		cin>>m;
		cout<<endl;

		coordinates = generateCoordinates(m);
		cout<<">> Wygenerowane dane: "<<endl;
		printVector2D(coordinates);
	}

	if (coordinates.size() > 0)
	{
		graph = toAdjacencyMatrix(coordinates);
		cout<<fixed<<showpoint<<"Minimalny koszt znaleziony przez:"<<endl;

		time_1 = Get_time();
		result = tspGreedy(graph, false); // false/true -> wyswietlanie znalezionej trasy
		time_2 = Get_time() + 1;
		cout<<">> Algorytm zachlanny ["<<setprecision(3)<<(time_2-time_1)/1000.0<<"s]: "<<setprecision(2)<<result<<endl;

		time_1 = Get_time();
		result = tspAntColony(graph, false); // false/true -> wyswietlanie znalezionej trasy
		time_2 = Get_time() + 1;
		cout<<">> Algorytm mrowkowy ["<<setprecision(3)<<(time_2-time_1)/1000.0<<"s]: "<<setprecision(2)<<result;
	}
	else
	{
		cout<<"Pusty graf !!!";
	}

	return 0;
}

