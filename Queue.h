#ifndef QUEUE_H_
#define QUEUE_H_

#include "List.h"
class PCB;
class KernelSem;

class Queue{
	List queueList;
public:
	void insert_q(PCB* pcb) { queueList.add(pcb); }
	PCB* delete_q() { return queueList.removeFirstPCB(); }
	void timeDecrement(KernelSem*);
}

#endif