//
// Created by Daniel on 2020-03-18.
//

#ifndef INC_441ASG3ROUTINGTRADEOFFS_EVENT_H
#define INC_441ASG3ROUTINGTRADEOFFS_EVENT_H

#include <list>
#define CALL_ARRIVAL 1
#define CALL_DEPARTURE 2
class Event {
public:
	Event(int source, int destination, float startTime, float duration, int callid, int event_type) : source(source),
	                                                                                                  destination(destination),
	                                                                                                  startTime(startTime),
	                                                                                                  callID(callid),
	                                                                                                  duration(duration),
	                                                                                                  event_type(event_type)
	{
		endTime = startTime + duration;
		if (event_type == CALL_ARRIVAL){
			eventTime = startTime;
		}else{
			eventTime = endTime;
		}
	}
	Event(const Event& prev,  int event_type):
			source(prev.source),
			destination(prev.destination),
			startTime(prev.startTime),
			duration(prev.duration),
			endTime(prev.endTime),
			callID(prev.callID),
			event_type(event_type),
			path(prev.path)
			{
		if (event_type == CALL_ARRIVAL){
			eventTime = startTime;
		}else{
			eventTime = endTime;
		}

	}

	int getEventType() const {
		return event_type;
	}

	int getSource() const {
		return source;
	}

	int getDestination() const {
		return destination;
	}

	float getStartTime() const {
		return startTime;
	}

	float getEndTime() const {
		return endTime;
	}

	int getCallid() const {
		return callID;
	}

	float getDuration() const {
		return duration;
	}

	float getEventTime() const {
		return eventTime;
	}

	bool operator<( const Event &b){
		return this->eventTime < b.getEventTime();
	}
	std::list<int> path;

private:
	int source;
	int destination;
	float startTime;
	float duration;
	float endTime;
	float eventTime;
	int event_type;
	int callID;
	//Stack of path

};


#endif //INC_441ASG3ROUTINGTRADEOFFS_EVENT_H
