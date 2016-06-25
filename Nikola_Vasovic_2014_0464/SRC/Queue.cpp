#include "List.h"
#include "Queue.h"

Queue::Queue() : queueList(new List()) {}
Queue::~Queue(){ delete queueList; }

void Queue::insert_q(PCB* pcb){
	queueList->add(pcb);
}

PCB* Queue::delete_q(){
	return queueList->removeFirstPCB();
}

int Queue::isEmpty() const {
	return queueList->isEmpty();
}

PCB* Queue::remove(unsigned int id){
	return queueList->remove(id);
}

PCB* Queue::peek() const {
	return queueList->getFirstPCB();
}
