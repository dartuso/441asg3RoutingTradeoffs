//
// Created by Daniel on 2020-03-18.
//
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <list>
#include <climits>

#include "Event.h"
#include "Result.h"


#define SHORTEST_HOP_PATH_FIRST "SHPF"
#define SHORTEST_DELAY_PATH_FIRST "SDPF"
#define LEAST_LOADED_PATH "LLP"
#define MAXIMUM_FREE_CIRCUITS "MFC"

#define MAX_VERTEX 20
#define MAX_EVENTS 20000

/*Event Types*/
#define CALL_ARRIVAL 1
#define CALL_DEPARTURE 2

double propdelay[MAX_VERTEX][MAX_VERTEX];
int capacity[MAX_VERTEX][MAX_VERTEX];
int available[MAX_VERTEX][MAX_VERTEX];
int cost[MAX_VERTEX][MAX_VERTEX];

void ReleaseCall(Event &event, Result &result);
bool RouteCall(Event &event, Result &result);

using namespace std;

int main(){
	vector<Result> results;


#ifdef SHORTEST_DELAY_PATH_FIRST
	Result SDPF(SHORTEST_DELAY_PATH_FIRST);
	results.push_back(SDPF);
#endif

/*#ifdef SHORTEST_HOP_PATH_FIRST
	Result SHPF(SHORTEST_HOP_PATH_FIRST);
	results.push_back(SHPF);
#endif

#ifdef LEAST_LOADED_PATH
	Result LLP(LEAST_LOADED_PATH);
	results.push_back(LLP);
#endif

#ifdef MAXIMUM_FREE_CIRCUITS
	Result MFC(MAXIMUM_FREE_CIRCUITS);
	results.push_back(MFC);
#endif*/


	ifstream topology("topology.dat");
	if (!topology) {
		cerr << "Error reading topology file\n";
		exit(-1);
	}

	while (!topology.eof()) {
		char nodes[2];
		float delay = 0;
		int total = 0;
		topology >> nodes[0];
		topology >> nodes[1];
		topology >> delay;
		topology >> total;
		int row = nodes[0] - 'A';
		int col = nodes[1] - 'A';
		cout << "adding " << row << " "<<col <<" "<< delay << " " << total << endl;
		propdelay[row][col] = delay; propdelay[col][row] = delay;
		capacity[row][col] = total; capacity[col][row] = total;
		available[row][col] = total; available[col][row] = total;
	}
	topology.close();

	ifstream eventFile("callworkload.dat");
	if (!eventFile) {
		cerr << "Error reading event file\n";
		exit(-1);
	}

	list<Event> events;
	while (!eventFile.eof()) {
		float start = 0, duration = 0;
		char startN, endN;
		eventFile >> start;
		eventFile >> startN;
		eventFile >> endN;
		eventFile >> duration;
		int src = startN - 'A';
		int dst = endN - 'A';
		cout << "adding " << src << " "<<dst <<" "<< start << " " << duration << " " << events.size() << endl;

		Event startEvent(src, dst, start, duration, CALL_ARRIVAL,(int) events.size());
		events.push_back(startEvent);
	}
	eventFile.close();

//	std::sort(events.begin(),events.end());
/* Now simulate the call arrivals and departures */
//	FOR EACH ALGORITHM
//  FOR EACH EVENT
	for (auto &result : results) {
		for (auto &event : events) {
			result.incrTotalCalls();
			if (event.getEventType() == CALL_ARRIVAL) {
				if(RouteCall(event, result)) {
					result.incrSuccessCalls();
					Event endEvent(event,CALL_DEPARTURE);
					events.insert(std::lower_bound(events.begin(), events.end(), endEvent), endEvent);
				}else{
					result.incrBlockedCalls();
				}
			}else ReleaseCall(event, result);
		}
	}


	//Print Results
	cout << "Policy   Calls   Succ   Block (%)   Hops   Delay\n";
	cout << "-------------------------------------------------------------------" << endl;
	for (auto &result : results) {
		cout << result.getAlgorithm() << "\t" <<
		     result.getTotalCalls() << "\t" <<
		     result.getSuccessCalls() << "\t" <<
		     result.getBlockedCalls() << " (" <<
		     result.getBlocked() << ")\t" <<
		     result.getHops() << "\t" <<
		     result.getDelay() << endl;
	}
	return 0;
}

void printPath(int parent[], int j)
{

	// Base Case : If j is source
	if (parent[j] == - 1)
		return;

	printPath(parent, parent[j]);

	printf("%d ", j);
}

// A utility function to print
// the constructed distance
// array
int printSolution(double dist[], int n,
                  int parent[])
{
	int src = 0;
	printf("Vertex\t Distance\tPath");
	for (int i = 1; i < MAX_VERTEX; i++)
	{
		printf("\n%d -> %d \t\t %d\t\t%d ",
		       src, i, dist[i], src);
		printPath(parent, i);
	}
}

int minDistance(double dist[],
                bool sptSet[])
{

	// Initialize min value
	int min = INT_MAX, min_index = 0;

	for (int v = 0; v < MAX_VERTEX; v++)
		if (!sptSet[v] &&
		    dist[v] <= min)
			min = dist[v], min_index = v;

	return min_index;
}

bool RouteCall(Event &event, Result &result) {
	// The output array. dist[i]
	// will hold the shortest
	// distance from src to i
	double dist[MAX_VERTEX];

	// sptSet[i] will true if vertex
	// i is included / in shortest
	// path tree or shortest distance
	// from src to i is finalized
	bool sptSet[MAX_VERTEX];

	// Parent array to store
	// shortest path tree
	int parent[MAX_VERTEX];

	// Initialize all distances as
	// INFINITE and stpSet[] as false
	for (int i = 0; i < MAX_VERTEX; i++)
	{
		parent[0] = -1;
		dist[i] = INT_MAX;
		sptSet[i] = false;
	}

	// Distance of source vertex
	// from itself is always 0
	dist[event.getSource()] = 0;

	// Find shortest path
	// for all vertices
	for (int count = 0; count < MAX_VERTEX - 1; count++)
	{
		// Pick the minimum distance
		// vertex from the set of
		// vertices not yet processed.
		// u is always equal to src
		// in first iteration.
		int u =  minDistance(dist, sptSet);

		// Mark the picked vertex
		// as processed
		sptSet[u] = true;

		// Update dist value of the
		// adjacent vertices of the
		// picked vertex.
		for (int v = 0; v < MAX_VERTEX; v++)

			// Update dist[v] only if is
			// not in sptSet, there is
			// an edge from u to v, and
			// total weight of path from
			// src to v through u is smaller
			// than current value of
			// dist[v]
			if (!sptSet[v] && propdelay[u][v] &&
			    dist[u] + propdelay[u][v] < dist[v])
			{
				parent[v] = u;
				dist[v] = dist[u] + propdelay[u][v];
			}
	}

	// print the constructed
	// distance array
	printPath(parent, event.getDestination());
}




void ReleaseCall(Event &event, Result &result) {

}

