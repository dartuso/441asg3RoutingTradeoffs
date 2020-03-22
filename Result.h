//
// Created by Daniel on 2020-03-18.
//

#ifndef INC_441ASG3ROUTINGTRADEOFFS_RESULT_H
#define INC_441ASG3ROUTINGTRADEOFFS_RESULT_H

#include <string>
#include <iostream>
#include "Edge.h"
#include "Event.h"
#define MAX_VERTEX 20

enum typeAlgorithm{
	SHPF,
	SDPF,
	LLP,
	MFC
};

using namespace std;
class Result {
public:
	explicit Result(string algo) : algorithmStr(std::move(algo)) {
		totalCalls = 0;
		successCalls = 0;
		blockedCalls = 0;
		hops = 0;
		delay = 0;

		if(algorithmStr == "SHPF"){
			algorithmEnum = SHPF;
		} else if (algorithmStr == "SDPF"){
			algorithmEnum = SDPF;
		}else if(algorithmStr == "LLP"){
			algorithmEnum = LLP;
		} else if (algorithmStr == "MFC"){
			algorithmEnum = MFC;
		}
	}
	Edge graph[MAX_VERTEX][MAX_VERTEX];
	list<Event> events;

	typeAlgorithm getAlgorithmEnum() const {
		return algorithmEnum;
	}

	void printCapacity(){
		cout << flush;
		for (int i = 0; i < MAX_VERTEX; ++i) {
			for (int j = 0; j < MAX_VERTEX; ++j) {
				cout << graph[i][j].getCapacity() << " ";
			}
			cout << endl;
		}
	}


	const string &getAlgo() const {
		return algorithmStr;
	}

	int getTotalCalls() const {
		return totalCalls;
	}

	int getSuccessCalls() const {
		return successCalls;
	}

	int getBlockedCalls() const {
		return blockedCalls;
	}


	float getBlocked()  {
		return (float) blockedCalls / (float) totalCalls * 100;
	}

	double getHopsAvg() const {
		return (double) hops / (double) successCalls;
	}

	double getDelayAvg() const {
		return (double) delay / (double) successCalls ;
	}

	void addHops(int newHops){
		this->hops += newHops;
	}

	void addDelay(int newDelay){
		this->delay += newDelay;
	}

	void incrTotalCalls() {
		Result::totalCalls++;
	}
	void incrSuccessCalls(){
		Result::successCalls++;
	}
	void incrBlockedCalls(){
		Result::blockedCalls++;
	}
private:
	string algorithmStr;
	int totalCalls;
	int successCalls;
	int blockedCalls;
	int hops;
	int delay;
	typeAlgorithm algorithmEnum;
};


#endif //INC_441ASG3ROUTINGTRADEOFFS_RESULT_H
