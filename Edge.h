//
// Created by Daniel on 2020-03-18.
//


#ifndef INC_441ASG3ROUTINGTRADEOFFS_EDGE_H
#define INC_441ASG3ROUTINGTRADEOFFS_EDGE_H

#include <utility>


class Edge {
private:
	std::pair<int, int> nodes;
	int delay;
	int maxcircuit;
	int capacity;

public:
	int getDelay() {
		return delay;
	}

	int getMaxcircuit() const {
		return maxcircuit;
	}

	int getCapacity() const {
		return capacity;
	}

	bool addEvent() {
		if (this->capacity == 0){
			return false;
		} else {
			capacity--;
			return true;
		}
	}

	bool removeEvent(){
		if (this->capacity == this->getMaxcircuit()){
			return false;
		} else {
			capacity++;
			return true;
		}
	}


	Edge() {
		nodes.first = 0;
		nodes.second = 0;
		delay = 0;
		maxcircuit = 0;
		capacity = 0;
	}

	Edge(int n1, int n2, int delay, int maxcircuit) :
			delay(delay), maxcircuit(maxcircuit) {
		if (n1 > n2) {
			nodes.first = n1;
			nodes.second = n2;
		} else {
			nodes.first = n2;
			nodes.second = n1;
		}
		capacity = maxcircuit;
	}

};


#endif //INC_441ASG3ROUTINGTRADEOFFS_EDGE_H
