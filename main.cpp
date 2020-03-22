#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include <climits>
#include <algorithm>
#include <list>
#include <iomanip>
#include "Event.h"
#include "Result.h"
#include "Edge.h"

using namespace std;




#define SHORTEST_HOP_PATH_FIRST "SHPF"
#define SHORTEST_DELAY_PATH_FIRST "SDPF"
#define LEAST_LOADED_PATH "LLP"
#define MAXIMUM_FREE_CIRCUITS "MFC"
#define SHORTEST_HOP_PATH_ONLY "SHPO"
#define MY_PATH

#define MAX_EVENTS 20000
#define MAX_VERTEX 20

/*Event Types*/
#define CALL_ARRIVAL 1
#define CALL_DEPARTURE 2

/*Two types
 * Call arrival
 * Call completion (if not blocked)
 *
 * When accepted
 * Need to keep track of completed calls
 * Time of when things arrive and leave
 *
 * */
bool algorithmMinMax(Event &event, Result &result);

bool shortestHopPathFirst(Event &event, Result &result);

bool shortestDelayPath(Event &event, Result &result);

bool leastLoadedPath(Event &event, Result &result);

bool maximumFreeCircuits(Event &event, Result &result);

void ReleaseCall(Event &event, Result &result);

int minDistance(int pInt[20], bool pBoolean[20]);
int minDistanceD(double *pInt, bool *pBoolean);


int maxDistance(int dist[20], bool found[20]);

int main() {
	vector<Result> results;

#ifdef SHORTEST_HOP_PATH_FIRST
	Result SHPF(SHORTEST_HOP_PATH_FIRST);
	results.push_back(SHPF);
#endif

#ifdef SHORTEST_DELAY_PATH_FIRST
	Result SDPF(SHORTEST_DELAY_PATH_FIRST);
	results.push_back(SDPF);
#endif

#ifdef LEAST_LOADED_PATH
	Result LLP(LEAST_LOADED_PATH);
	results.push_back(LLP);
#endif

#ifdef MAXIMUM_FREE_CIRCUITS
	Result MFC(MAXIMUM_FREE_CIRCUITS);
	results.push_back(MFC);
#endif

	ifstream topology("st.dat");
	if (!topology) {
		cerr << "Error reading topology file\n";
		exit(-1);
	}
	vector<Edge> network;
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
//		cout << "adding " << row << " " << col << " " << delay << " " << total << endl;
		for (auto &result : results) {
			result.graph[row][col] = Edge(row, col, delay, total);
			result.graph[col][row] = Edge(row, col, delay, total);
		}
	}
	topology.close();

	ifstream eventFile("sc.dat");
	if (!eventFile) {
		cerr << "Error reading event file\n";
		exit(-1);
	}
	int counter = 0;
	while (!eventFile.eof()) {
		float start = 0, duration = 0;
		char startN, endN;
		eventFile >> start;
		eventFile >> startN;
		eventFile >> endN;
		eventFile >> duration;
		int src = startN - 'A';
		int dst = endN - 'A';
//		cout << "adding " << src << " " << dst << " " << start << " " << duration << " " << events.size() << endl;
		Event startEvent(src, dst, start, duration, counter, CALL_ARRIVAL);
		for (auto &result : results){
			result.events.push_back(startEvent);
		}
		counter++;
	}
	eventFile.close();

//	std::sort(events.begin(),events.end());
/* Now simulate the call arrivals and departures */
	cout << std::setprecision(5);
	for (auto &result : results) {
//		result.printCapacity();
		for (auto &event : result.events) {
			char src = event.getSource() + 'A';
			char dst = event.getDestination() + 'A';
//			cout << "Performing Event: " << event.getCallid() << " " << src << " " << dst << " type: " << event.getEventType() << " time: " << event.getEventTime() << endl;
			if (event.getEventType() == CALL_ARRIVAL) {
				result.incrTotalCalls();
				if (algorithmMinMax(event, result)) {
					result.incrSuccessCalls();
					Event endEvent(event, CALL_DEPARTURE);
					result.events.insert(std::lower_bound(result.events.begin(), result.events.end(), endEvent), endEvent);
				} else {
					result.incrBlockedCalls();
				}
			} else {
				ReleaseCall(event, result);
			}
//			result.printCapacity();
		}
	}


	//Print Results
	cout << "Policy   Calls   Succ   Block (%)   Hops   Delay\n";
	cout << "-------------------------------------------------------------------" << endl;
	for (auto &result : results) {
		cout << result.getAlgo() << " \t" <<
		     result.getTotalCalls() << "\t\t" <<
		     result.getSuccessCalls() << "\t\t" <<
		     result.getBlockedCalls() << " (" <<
		     result.getBlocked() << "%)\t" <<
		     result.getHopsAvg() << "\t" <<
		     result.getDelayAvg() << endl;
	}
	return 0;
}

void ReleaseCall(Event &event, Result &result) {
/*	cout << "Releasing: ";
	for (auto node : event.path) {
		cout << node;
	}
	cout << endl;*/
	while (event.path.size() != 1) {
		int id = event.path.back();
		event.path.pop_back();
		int id2 = event.path.back();
		result.graph[id][id2].removeEvent();
		result.graph[id2][id].removeEvent();
	}
}




bool algorithmMinMax(Event &event, Result &result) {
	switch (result.getAlgorithmEnum()){
		case SHPF:
			return shortestHopPathFirst(event,result);
			break;
		case SDPF:
			return shortestDelayPath(event,result);
			break;
		case LLP:
			return leastLoadedPath(event, result);
			break;
		case MFC:
			return maximumFreeCircuits(event,result);
			break;
		default:
			cerr << "Error in algorithm" << endl;
			break;
	}
}

int minDistance(int dist[20], bool found[20]) {
	int min = INT_MAX, min_index = 21;
	for (int v = 0; v < MAX_VERTEX; v++) {
		if (!found[v] && dist[v] <= min) {
			min = dist[v], min_index = v;
		}
	}
	return min_index;
}

int minDistanceD(double *pInt, bool *pBoolean) {
	double min = INT_MAX;
	int min_index = 21;
	for (int v = 0; v < MAX_VERTEX; v++) {
		if (!pBoolean[v] && pInt[v] <= min) {
			min = pInt[v], min_index = v;
		}
	}
	return min_index;
}

bool shortestHopPathFirst(Event &event, Result &result) {
	int dist[MAX_VERTEX];
	int prev[MAX_VERTEX];
	bool found[MAX_VERTEX] = {false};

	for (int i = 0; i < MAX_VERTEX; ++i) {
		dist[i] = INT_MAX;
		prev[i] = INT_MAX;
	}
	dist[event.getSource()] = 0;

	for (int j = 0; j < MAX_VERTEX; ++j) {

		int u = minDistance(dist, found);

		if (u == event.getDestination()) {
			break;
		}


		found[u] = true;


		for (int v = 0; v < MAX_VERTEX; ++v) {
			int temp;
			if (result.graph[u][v].getCapacity() != 0) {
				temp = 1;
			} else {
				temp = 0;
			}
			if (!found[v] &&
			    result.graph[u][v].getCapacity() &&
			    dist[u] != INT_MAX &&
			    dist[u] + temp <= dist[v]) {
				dist[v] = dist[u] + temp;
				prev[v] = u;
			}
		}
	}

	int target = event.getDestination();
	if (prev[target] != INT_MAX) {
//		cout << "Path:";
		while (target != event.getSource()) {
			int temp = prev[target];
			result.graph[temp][target].addEvent();
			result.graph[target][temp].addEvent();
			result.addDelay(result.graph[target][temp].getDelay());
//			cout << target;
			event.path.push_front(target);
			target = prev[target];
		}
		result.addHops(event.path.size());
		event.path.push_front(event.getSource());
//		cout << " Size:" << event.path.size() << endl;
		return true;
	}

	return false;
}

bool shortestDelayPath(Event &event, Result &result) {
	int dist[MAX_VERTEX];
	int prev[MAX_VERTEX];
	bool found[MAX_VERTEX] = {false};

	for (int i = 0; i < MAX_VERTEX; ++i) {
		dist[i] = INT_MAX;
		prev[i] = INT_MAX;
	}
	dist[event.getSource()] = 0;

	for (int j = 0; j < MAX_VERTEX; ++j) {

		int u = minDistance(dist, found);

		if (u == event.getDestination()) {
			break;
		}
		found[u] = true;

		for (int v = 0; v < MAX_VERTEX; ++v) {
			if (!found[v] &&
			    result.graph[u][v].getCapacity() &&
			    dist[u] != INT_MAX &&
			    dist[u] + result.graph[u][v].getDelay() < dist[v]) {
				dist[v] = dist[u] + result.graph[u][v].getDelay();
				prev[v] = u;
			}
		}
	}

	int target = event.getDestination();
	if (prev[target] != INT_MAX) {
//		cout << "Path:";
		while (target != event.getSource()) {
			int temp = prev[target];
			result.graph[temp][target].addEvent();
			result.graph[target][temp].addEvent();
			result.addDelay(result.graph[target][temp].getDelay());
//			cout << target;
			event.path.push_front(target);
			target = prev[target];
		}
		result.addHops(event.path.size());
		event.path.push_front(event.getSource());
//		cout << " Size:" << event.path.size() << endl;
		return true;
	}


	return !event.path.empty();
}

bool leastLoadedPath(Event &event, Result &result) {
	double dist[MAX_VERTEX];
	double prev[MAX_VERTEX];
	bool found[MAX_VERTEX] = {false};
	double cost = 0;

	for (int i = 0; i < MAX_VERTEX; ++i) {
		dist[i] = INT_MAX;
		prev[i] = INT_MAX;
	}
	dist[event.getSource()] = 0;
/*
 * get a loads and sorted by lowest load
 * from lowest load try to get to dest
 *
 *
 * */


	for (int j = 0; j < MAX_VERTEX; ++j) {

		int u = minDistanceD(dist, found);

		found[u] = true;

		for (int v = 0; v < MAX_VERTEX; ++v) {
			double cost = 0;
			if (result.graph[u][v].getCapacity()) {
				cost = (1.0 - ((double) result.graph[u][v].getCapacity() / (double) result.graph[u][v].getMaxcircuit()));
			}
			cost = max(dist[u], cost);
			if (!found[v] &&
			    result.graph[u][v].getCapacity() &&
			    dist[u] != INT_MAX &&
			    cost < dist[v]) {
				dist[v] = cost;
				prev[v] = u;
			}
		}
	}

	int target = event.getDestination();
	if (prev[target] != INT_MAX) {
//		cout << "Path:";
		while (target != event.getSource()) {
			int temp = prev[target];
			result.graph[temp][target].addEvent();
			result.graph[target][temp].addEvent();
			result.addDelay(result.graph[target][temp].getDelay());
//			cout << target;
			event.path.push_front(target);
			target = prev[target];
		}
		result.addHops(event.path.size());
		event.path.push_front(event.getSource());
//		cout << " Size:" << event.path.size() << endl;
		return true;
	}

	return false;
}

bool maximumFreeCircuits(Event &event, Result &result) {
	int dist[MAX_VERTEX];
	int prev[MAX_VERTEX];
	bool found[MAX_VERTEX] = {false};

	for (int i = 0; i < MAX_VERTEX; ++i) {
		dist[i] = INT_MIN;
		prev[i] = INT_MIN;
	}
	dist[event.getSource()] = INT_MAX;

	for (int j = 0; j < MAX_VERTEX; ++j) {

		int u = maxDistance(dist, found);

/*		if (u == event.getDestination()) {
			break;
		}*/
		found[u] = true;
		for (int v = 0; v < MAX_VERTEX; ++v) {
			int load = INT_MAX;
			if (result.graph[u][v].getCapacity() > 0) {
				load = result.graph[u][v].getCapacity();
			}
			load = min(dist[u], load);
			if (!found[v] &&
			    result.graph[u][v].getCapacity() &&
			    dist[u] != INT_MIN &&
			    load > dist[v]
			    ) {
				dist[v] = load;
				prev[v] = u;
			}
		}
	}

	int target = event.getDestination();
	if (prev[target] != INT_MIN) {
//		cout << "Path:";
		while (target != event.getSource()) {
			int temp = prev[target];
			result.graph[temp][target].addEvent();
			result.graph[target][temp].addEvent();
			result.addDelay(result.graph[target][temp].getDelay());
//			cout << target;
			event.path.push_front(target);
			target = prev[target];
		}
		result.addHops(event.path.size());
		event.path.push_front(event.getSource());
//		cout << " Size:" << event.path.size() << endl;
		return true;
	}

	return false;
}

int maxDistance(int dist[20], bool found[20]) {
	int min = INT_MIN, min_index = 21;
	for (int v = 0; v < MAX_VERTEX; v++) {
		if (!found[v] && dist[v] >= min) {
			min = dist[v], min_index = v;
		}
	}

	return min_index;
}
