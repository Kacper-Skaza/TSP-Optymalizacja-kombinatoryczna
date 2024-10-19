#include <iostream>
#include <vector>
#include <climits>
using namespace std;

//graph - odleglosci miedzy poszczegolnymi miatami
//visited - odwiedzone miasta
//currentCity - numer aktualnego miasta

int minDistance(vector<vector<int> > &graph, vector<bool> &visited, int currentCity) { // szukanie najbliższego miasta
	int n;
	int minDist;
	int nextCity;
	
	n = graph.size();
	minDist = INT_MAX;
	nextCity = -1;

	for (int i=0; i<n; i++) {
		if (visited[i] == false && graph[currentCity][i] < minDist) { //graph[currentCity][i] != 0
			minDist = graph[currentCity][i];
			nextCity = i;
		}
	}

	return nextCity;
}

int tspGreedy(vector<vector<int> > &graph, int currentCity) {
	int n;
	int totalCost;
	vector<bool> visited;
	
	n = graph.size();
	visited.assign(n, false);
	visited[currentCity] = true;
	totalCost = 0;

	for (int i=0; i<n-1; i++) {
		int nextCity = minDistance(graph, visited, currentCity);
		totalCost += graph[currentCity][nextCity];
		visited[nextCity] = true;
		currentCity = nextCity;
	}

	totalCost += graph[currentCity][0]; // Powrót do miasta poczatkowego
	return totalCost;
}

int main() {
	vector<vector<int> > graph; 
    	int result;
	
	graph = {  // co jesli graf nie jest pełny
		{0, 10, 15, 20},
		{10, 0, 35, 25},
		{15, 35, 0, 30},
		{20, 25, 30, 0}
	};

	result = tspGreedy(graph, 0);
	cout<<"Minimalny koszt znaleziony przez algorytm zachlanny: "<<result<<endl;

	return 0;
}
