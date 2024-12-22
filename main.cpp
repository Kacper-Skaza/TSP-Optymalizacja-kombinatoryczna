#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include <thread>
#include <iomanip>
#include <limits>
#include <cmath>
using namespace std;

/*	TODO:
	Milej zabawy :D
*/

// CZAS
long long Get_time()
{
	return chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
}

// GENERATOR LICZB LOSOWYCH
int Random_num()
{
	static random_device rd;
	static mt19937 gen(rd());
	static uniform_int_distribution<> dis(0, RAND_MAX); // Zakres: <0, 32767>

	return dis(gen);
}

// STALE
const int MIN_ITERATIONS = 25;			// Min liczba iteracji
const int MAX_ITERATIONS = 250;			// Max liczba iteracji
const int MAX_SAME_ITERATIONS = 10;		// Max liczba takich samych iteracji

// ZMIENNE -> STROJENIE
int NUM_INSTANCES = 10;		// Liczba instancji (ten sam algorytm uruchamiany n razy -> bierzemy najlepszy wynik)
int NUM_ANTS = 100;			// Liczba mrowek
double ALPHA = 1.0;			// Waga sladu feromonowego
double BETA = 3.0;			// Waga odleglosci
double RHO = 0.50;			// Wspolczynnik parowania feromonow (wieksza losowosc -> dluzsze zanikanie -> wartosc w gore)
double Q = 100.0;			// Stala uzywana do aktualizacji feromonow (wieksza losowosc -> mniejsze wartosci -> wartosc w dol)





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
	vector<vector<int>> result(0);

	for (int i=0; i<n; i++)
	{
		int x=0, y=0;
		x = (Random_num()%1000) + 1; // Zakres: <1, 1000>
		y = (Random_num()%1000) + 1; // Zakres: <1, 1000>

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


// -------------------------Algorytm-zachlanny-TSP-------------------------


// Szukanie najblizszego miasta
int minDistance(const vector<vector<double>> &graph, const vector<bool> &visited, const int currentCity)
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
double tspGreedy(const vector<vector<double>> &graph, const bool showPath)
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


// -------------------------Algorytm-mrowkowy-TSP-------------------------


// Aktualizacja feromonow
void updatePheromones(	const int &n,
						vector<vector<double>> &pheromone,
						const vector<vector<int>> &paths, const vector<double> &lengths)
{
	vector<vector<double>> deltaTau(n, vector<double>(n, 0.0)); // Poziom feromonow na krawedzi

	// Obliczamy deltaTau
	for (int ant=0; ant<NUM_ANTS; ant++)
	{
		for (int i=0; i<n-1; i++)
		{
			int cityA = paths[ant][i];
			int cityB = paths[ant][i+1];
			deltaTau[cityA][cityB] += Q / lengths[ant];
			deltaTau[cityB][cityA] += Q / lengths[ant];
		}

		// Powrot do miasta startowego
		int lastCity = paths[ant][n-1];
		int firstCity = paths[ant][0];
		deltaTau[lastCity][firstCity] += Q / lengths[ant];
		deltaTau[firstCity][lastCity] += Q / lengths[ant];
	}

	// Aktualizujemy feromony wedlug wzoru 5.2 z pdf
	for (int i=0; i<n; i++)
	{
		for (int j=0; j<n; j++)
		{
			//pheromone[i][j] = RHO * oldPheromone[i][j] + deltaTau[i][j];
			pheromone[i][j] *= RHO;
			pheromone[i][j] += deltaTau[i][j];
		}
	}
}

// Funkcja wyboru nastepnego miasta na podstawie prawdopodobienstwa
int selectNextCity(	const int &n, const int &currentCity,
					const vector<vector<double>> &graph, const vector<vector<double>> &pheromone, const vector<bool> &visited)
{
	// Obliczamy prawdopodobienstwa
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

	// Metoda ruletki
	double randVal = (Random_num()*sum) / (RAND_MAX*1.0);
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

	return -1; // Blad
}

// Funkcja wykonujaca trase dla jednej mrowki
void antTravel(	const int &n, const int antNum,
				vector<vector<int>> &paths, vector<double> &lengths,
				const vector<vector<double>> &graph, const vector<vector<double>> &pheromone)
{
	vector<bool> visited(n, false);
	vector<int> path(0);
	double length = 0.0;
	int startCity = Random_num() % n;
	int currentCity = startCity;

	// Odwiedzenie miasta poczatkowego
	visited[currentCity] = true;
	path.push_back(currentCity);

	// Odwiedzenie pozostalych miast
	for (int i=1; i<n; i++)
	{
		int nextCity = selectNextCity(n, currentCity, graph, pheromone, visited);
		path.push_back(nextCity);
		visited[nextCity] = true;
		length += graph[currentCity][nextCity];
		currentCity = nextCity;
	}

	// Powrot do miasta poczatkowego
	length += graph[currentCity][startCity];
	path.push_back(startCity);

	// Zapisywanie wynikow
	lengths[antNum] = length;
	paths[antNum] = path;
}

// Algorytm mrowkowy TSP
void tspAntColony(	const int &n,
					vector<int> &bestPath, double &bestLength,
					const vector<vector<double>> &graph)
{
	vector<vector<double>> pheromone(n, vector<double>(n, 1.0));
	int sameResult = 0;

	for (int i=0; i<MAX_ITERATIONS; i++)
	{
		vector<vector<int>> paths(NUM_ANTS);
		vector<double> lengths(NUM_ANTS);

		// Wektor watkow
		vector<thread> threads(0);

		// Symulacja dla kazdej mrowki na osobnym watku
		for (int ant=0; ant<NUM_ANTS; ant++)
		{
			//antTravel(n, ant, paths, lengths, graph, pheromone);
			threads.emplace_back(antTravel,
									ref(n), ant,
									ref(paths), ref(lengths),
									ref(graph), ref(pheromone));
		}

		// Oczekiwanie na zakonczenie watkow
		for (auto &thread : threads)
		{
			thread.join();
		}

		// Sprawdzanie, czy znaleziono lepsza trase
		for (int ant=0; ant<NUM_ANTS; ant++)
		{
			if (lengths[ant] < bestLength)
			{
				bestLength = lengths[ant];
				bestPath = paths[ant];
				sameResult = -1;
			}
		}

		// Aktualizacja feromonow
		updatePheromones(n, pheromone, paths, lengths);

		// Sprawdzenie, czy wynik sie½ poprawia
		sameResult++;
		if (sameResult > MAX_SAME_ITERATIONS && i > MIN_ITERATIONS)
			break;
	}
}

double tspAntColonyInstanceManager(const vector<vector<double>> &graph, const bool showPath)
{
	int n = graph.size();

	vector<vector<int>> paths(NUM_INSTANCES, vector<int>(0));
	vector<double> lengths(NUM_INSTANCES, numeric_limits<double>::max());
	vector<int> bestPath(0);
	double bestLength = numeric_limits<double>::max();

	// Wektor watkow
	vector<thread> threads(0);

	// Uruchamianie instancji
	for (int i=0; i<NUM_INSTANCES; i++)
	{
		//tspAntColony(n, paths[i], lengths[i], graph);
		threads.emplace_back(tspAntColony, ref(n), ref(paths[i]), ref(lengths[i]), ref(graph));
	}

	// Oczekiwanie na zakonczenie watkow
	for (auto &thread : threads)
	{
		thread.join();
	}

	// Wybor najlepszego wyniku
	for (int i=0; i<NUM_INSTANCES; i++)
	{
		if (lengths[i] < bestLength)
		{
			bestLength = lengths[i];
			bestPath = paths[i];
		}
	}

	if (showPath == true)
	{
		cout<<endl<<"Znaleziona trasa: ";
		for (int i=0; i<n; i++)
		{
			cout<<bestPath[i]+1<<", ";
		}
		cout<<bestPath[0]+1<<";"<<endl;
	}

	return bestLength;
}





int main()
{
	cout<<">> Legenda zrodel danych:"<<endl;
	cout<<"1 - Ranking: 'berlin52.txt'"<<endl;
	cout<<"2 - Ranking: 'bier127.txt'"<<endl;
	cout<<"3 - Ranking: 'tsp250.txt'"<<endl;
	cout<<"4 - Ranking: 'tsp500.txt'"<<endl;
	cout<<"5 - Ranking: 'tsp1000.txt'"<<endl;
	cout<<"6 - Plik 'Test.txt'"<<endl;
	cout<<"7 - Plik 'Input.txt'"<<endl;
	cout<<"8 - Generator"<<endl;

	while (true)
	{
		NUM_INSTANCES = 10; NUM_ANTS = 100;
		ALPHA = 1.0; BETA = 3.0; RHO = 0.50; Q = 100.0;

		vector<vector<int>> coordinates(0);
		vector<vector<double>> graph(0);
		long long time_1=0, time_2=0;
		double result=0.0;
		int n=0, m=0;

		while (n < 1 || n > 8)
		{
			cout<<endl<<"=========="<<endl<<endl;
			cout<<">> Wybierz zrodlo danych:"<<endl;
			cout<<">> ";
			cin>>n;
			cout<<endl;
		}

		if (n == 1)
		{
			coordinates = readCoordinates("Data/Ranking/berlin52.txt");
			NUM_INSTANCES = 10; NUM_ANTS = 500;
			ALPHA = 1.0; BETA = 3.0; RHO = 0.50; Q = 100.0;
		}
		else if (n == 2)
		{
			coordinates = readCoordinates("Data/Ranking/bier127.txt");
			NUM_INSTANCES = 10; NUM_ANTS = 400;
			ALPHA = 1.0; BETA = 10.0; RHO = 0.50; Q = 100.0;
		}
		else if (n == 3)
		{
			coordinates = readCoordinates("Data/Ranking/tsp250.txt");
			NUM_INSTANCES = 10; NUM_ANTS = 300;
			ALPHA = 1.5; BETA = 3.0; RHO = 0.50; Q = 100.0;
		}
		else if (n == 4)
		{
			coordinates = readCoordinates("Data/Ranking/tsp500.txt");
			NUM_INSTANCES = 10; NUM_ANTS = 200;
			ALPHA = 2.0; BETA = 6.0; RHO = 0.50; Q = 100.0;
		}
		else if (n == 5)
		{
			coordinates = readCoordinates("Data/Ranking/tsp1000.txt");
			NUM_INSTANCES = 5; NUM_ANTS = 100;
			ALPHA = 2.5; BETA = 2.5; RHO = 0.75; Q = 100.0;
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
			cout<<"Minimalny koszt znaleziony przez:"<<endl;
			cout<<fixed<<showpoint<<setprecision(2); // Wypisz dwie cyfry po przecinku

			time_1 = Get_time();
			{
				result = tspGreedy(graph, false); // false/true -> wyswietlanie znalezionej trasy
			}
			time_2 = Get_time() + 10;
			cout<<">> Algorytm zachlanny ["<<(time_2-time_1)/1000.0<<"s]: "<<result<<endl;

			time_1 = Get_time();
			{
				result = tspAntColonyInstanceManager(graph, false); // false/true -> wyswietlanie znalezionej trasy
			}
			time_2 = Get_time() + 10;
			cout<<">> Algorytm mrowkowy ["<<(time_2-time_1)/1000.0<<"s]: "<<result<<endl;
		}
		else
		{
			cout<<"Pusty graf !!!"<<endl;
		}
	}

	return 0;
}

