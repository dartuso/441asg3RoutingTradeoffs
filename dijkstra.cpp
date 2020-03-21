//
// Created by Daniel on 2020-03-20.
//
#include <stack>
#include <climits>
#include <iostream>

using namespace std;
int minDistance(int dist[], bool sptSet[])
{
	// Initialize min value
	int min = INT_MAX, min_index;

	for (int v = 0; v < V; v++)
		if (sptSet[v] == false && dist[v] <= min)
			min = dist[v], min_index = v;

	return min_index;
}

void djikstras(int graph[V][V], char src, char dst)
{
	int dist[V];
	bool setPath[V];
	int previous[V];

	for (int i = 0; i < V; i++)
	{
		dist[i] = INT_MAX;
		setPath[i] = false;
		previous[i] = INT_MAX;
	}
	dist[src - 'A'] = 0;

	//find the path from src to dst and update the availabilities
	for (int count = 0; count < V - 1; count++)
	{
		int u = minDistance(dist, setPath);

		setPath[u] = true;
		//if U is the DST then early exit
		if (u == dst - 'A')
		{
			break;
		}

		for (int v = 0; v < V; v++)
		{
			if (!setPath[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v])
			{
				dist[v] = dist[u] + graph[u][v];
				previous[v] = u;
			}
		}
	}

	stack<int> path;
	int u = dst - 'A';
	if (previous[u] != INT_MAX)
	{
		while (u != INT_MAX)
		{
			path.push(u);
			u = previous[u];
		}
	}

	while (!path.empty())
	{
		cout << path.top() << ' ';
		path.pop();
	}
	cout << endl;
}