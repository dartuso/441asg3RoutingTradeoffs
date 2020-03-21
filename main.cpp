#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include <climits>
#include <algorithm>
#include <list>
#include "Event.h"
#include "Result.h"
#include "Edge.h"

using namespace std;




//#define SHORTEST_HOP_PATH_FIRST "SHPF"
#define SHORTEST_DELAY_PATH_FIRST "SDPF"
//#define LEAST_LOADED_PATH "LLP"
//#define MAXIMUM_FREE_CIRCUITS "MFC"

#define MAX_ROW 20
#define MAX_COL 20
#define MAX_EVENTS 20000

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


bool RouteCall(Event &event, Result &result);

void ReleaseCall(Event &event, Result &result);

int minDistance(int pInt[20], bool pBoolean[20]);

void * pass(bool (*call)(Event &, Result &));

int main() {
	vector<Result> results;


#ifdef SHORTEST_DELAY_PATH_FIRST
	Result SDPF(SHORTEST_DELAY_PATH_FIRST);
	results.push_back(SDPF);
#endif

#ifdef SHORTEST_HOP_PATH_FIRST
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
#endif


	Edge graph[MAX_ROW][MAX_COL];
	ifstream topology("t2.dat");
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
		cout << "adding " << row << " " << col << " " << delay << " " << total << endl;
		for (auto &result : results) {
			result.graph[row][col] = Edge(row, col, delay, total);
			result.graph[col][row] = Edge(row, col, delay, total);
		}
	}
	topology.close();

	ifstream eventFile("c2.dat");
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
//		cout << "adding " << src << " " << dst << " " << start << " " << duration << " " << events.size() << endl;

		Event startEvent(src, dst, start, duration, (int) events.size(), CALL_ARRIVAL);
		events.push_back(startEvent);
	}
	eventFile.close();

//	std::sort(events.begin(),events.end());
/* Now simulate the call arrivals and departures */
//	FOR EACH ALGORITHM
//  FOR EACH EVENT
	for (auto &result : results) {
		result.printCapacity();
		for (auto &event : events) {
			char src = event.getSource() + 'A';
			char dst = event.getDestination() + 'A';
			cout << "Performing Event: " << event.getCallid() << " " << src << " " << dst << endl;
			if (event.getEventType() == CALL_ARRIVAL) {
				result.incrTotalCalls();
				if (RouteCall(event, result)) {
					result.incrSuccessCalls();
					Event endEvent(event, CALL_DEPARTURE);
					events.insert(std::lower_bound(events.begin(), events.end(), endEvent), endEvent);
				} else {
					result.incrBlockedCalls();
				}
			} else {
				ReleaseCall(event, result);
			}
			result.printCapacity();
		}
	}


	//Print Results
	cout << "Policy   Calls   Succ   Block (%)   Hops   Delay\n";
	cout << "-------------------------------------------------------------------" << endl;
	for (auto &result : results) {
		cout << result.getAlgo() << " \t" <<
		     result.getTotalCalls() << "\t" <<
		     result.getSuccessCalls() << "\t" <<
		     result.getBlockedCalls() << " (" <<
		     result.getBlocked() << "%)\t" <<
		     result.getHopsAvg() << "\t" <<
		     result.getDelayAvg() << endl;
	}
	return 0;
}

//	  SHORTEST_HOP_PATH_FIRST
//
//		 SHORTEST_DELAY_PATH_FIRST
//
//		 LEAST_LOADED_PATH
//
//		 MAXIMUM_FREE_CIRCUITS
bool RouteCall(Event &event, Result &result) {
	int dist[MAX_VERTEX];
	int prev[MAX_VERTEX];
	bool found[MAX_VERTEX] = {false};

	for (int i = 0; i < MAX_VERTEX; ++i) {
		dist[i] = INT_MAX; prev[i] = INT_MAX;
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
		cerr << "Path:";
		while (target != event.getSource()) {
			int temp = prev[target];
			result.graph[temp][target].addEvent();
			result.graph[target][temp].addEvent();
			result.addDelay(result.graph[target][temp].getDelay());
			char n = target + 'A';
			cerr << n;
			event.path.push_front(target);
			target = prev[target];
		}
		result.addHops(event.path.size());
		cerr << " Size:" << event.path.size() << "Delay " << result.getDelayAvg() << endl;
		return true;
	}



	return !event.path.empty();
}

bool shortestDelayDijikstra(Event &event, Result &result){
	int dist[MAX_VERTEX];
	int prev[MAX_VERTEX];
	bool found[MAX_VERTEX] = {false};

	for (int i = 0; i < MAX_VERTEX; ++i) {
		dist[i] = INT_MAX; prev[i] = INT_MAX;
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
		cout << "Path:";
		while (target != event.getSource()) {
			int temp = prev[target];
			result.graph[temp][target].addEvent();
			result.graph[target][temp].addEvent();
			result.addDelay(result.graph[target][temp].getDelay());
			char n = target + 'A';
			cout << n;
			event.path.push_front(target);
			target = prev[target];
		}
		result.addHops(event.path.size());
		cout << "\n Size:" << event.path.size() << "Delay " << result.getDelayAvg();
		return true;
	}



	return !event.path.empty();
}


int minDistance(int dist[20], bool found[20]) {
	int min = INT_MAX, min_index = 21;
	for (int v = 0; v < MAX_VERTEX; v++) {
		if (!found[v] && dist[v] <= min) {
			min = dist[v], min_index = v;
		}
	}

	return  min_index;
}


void ReleaseCall(Event &event, Result &result) {
	while (event.path.size() != 1) {
		int id = event.path.back();
		event.path.pop_back();
		int id2 = event.path.back();
		cout << (char) id + 'A' <<
		result.graph[id][id2].removeEvent();
		result.graph[id2][id].removeEvent();
	}
}

bool algorithmMinMax(Event &event, Result &result) {
	//SHPF minimize hops

	//SDPF minimize delay
	//LLP go along max cap
	//MFC

}