#include <iostream>
#include <vector>
using namespace std;

int minDistance(vector<vector<int> > &graph, vector<bool> &visited, int currentCity) {
	int n = graph.size();
	int minDist = INT_MAX;
	int nextCity = -1;

	for (int i=0; i<n; i++) {
		if (!visited[i] && graph[currentCity][i] < minDist) {
			minDist = graph[currentCity][i];
			nextCity = i;
		}
	}

	return nextCity;
}

int tspGreedy(vector<vector<int> > &graph, int currentCity) {
	int n = graph.size();
	vector<bool> visited(n, false);
	visited[currentCity] = true;
	int totalCost = 0;

	for (int i=0; i<n-1; i++) {
		int nextCity = minDistance(graph, visited, currentCity);
		totalCost += graph[currentCity][nextCity];
		visited[nextCity] = true;
		currentCity = nextCity;
	}

	totalCost += graph[currentCity][0]; // Powrót do miasta pocz¹tkowego
	return totalCost;
}

int main() {
	vector<vector<int> > graph = {
		{0, 10, 15, 20},
		{10, 0, 35, 25},
		{15, 35, 0, 30},
		{20, 25, 30, 0}
	};

	int result = tspGreedy(graph, 0);
	cout<<"Minimalny koszt znaleziony przez algorytm zachlanny: "<<result<<endl;

	return 0;
}

