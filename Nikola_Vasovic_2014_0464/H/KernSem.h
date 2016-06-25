#ifndef KERNSEM_H_
#define KERNSEM_H_

#include "Semaphor.h"

class Queue;
class SortedList;

class KernelSem{
public:
	KernelSem(int);
	~KernelSem();
	
	//static fields
	static void updateTime();
	static SortedList* waitingList;
	
	//semaphore operations
	int wait(Time);
	void signal();
	
	//getters
	Queue* getSemaphoreQueue() const;
	int getVal() const;
	//setters
	void incVal();
	
private:
	volatile int val; // sempahore value
	Queue* semaphoreQueue; // blocked PCBs on this semaphore
	
	int block(Time); //wait
	void deblock(); //signal
};

#endif