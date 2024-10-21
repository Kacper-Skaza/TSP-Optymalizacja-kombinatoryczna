#include <iostream>
#include <vector>
#include <climits>
using namespace std;

/*	TODO:
	Generator -> uwaga na powtorki punktow
	Wczytywanie z pliku -> po wspolrzednych
*/

// graph - odleglosci miedzy poszczegolnymi miastami
// visited - odwiedzone miasta
// currentCity - numer aktualnego miasta
int minDistance(vector<vector<int> > &graph, vector<bool> &visited, int currentCity) { // Szukanie najblizszego miasta
	int minDist;
	int nextCity;

	minDist = INT_MAX;
	nextCity = -1;

	for (int i=0; i<graph.size(); i++) {
		if (visited[i] == false && graph[currentCity][i] < minDist) { // graph[currentCity][i] != 0
			minDist = graph[currentCity][i];
			nextCity = i;
		}
	}

	return nextCity;
}

int tspGreedy(vector<vector<int> > &graph, int currentCity) {
	vector<bool> visited;
	int totalCost;

	visited.assign(graph.size(), false);
	visited[currentCity] = true;
	totalCost = 0;

	for (int i=0; i<graph.size()-1; i++) {
		int nextCity = minDistance(graph, visited, currentCity);
		totalCost += graph[currentCity][nextCity];
		visited[nextCity] = true;
		currentCity = nextCity;
		cout<<currentCity<<", ";
	}

	totalCost += graph[currentCity][0]; // Powrot do miasta poczatkowego
	return totalCost;
}



int main() {
	vector<vector<int> > graph; 
    int result=0, startCity=0;

	graph = { // Co jesli graf nie jest pelny???
		{0, 10, 15, 20},
		{10, 0, 35, 25},
		{15, 35, 0, 30},
		{20, 25, 30, 0}
	};

	cout<<"Sciezka: "<<startCity<<", ";
	result = tspGreedy(graph, startCity);
	cout<<startCity<<endl;
	cout<<"Minimalny koszt znaleziony przez algorytm zachlanny: "<<result;

	return 0;
}

