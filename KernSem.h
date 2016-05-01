#ifndef KERNSEM_H_
#define KERNSEM_H_

#include "Semaphore.h"

class SemList;
class Queue;

class KernelSem{
public:
	KernelSem(int);
	int getVal() const;
	int wait(Time);
	void signal();
	~KernelSem();
	static void updateTime();
	Queue* getWaitingSemaphoreList() const;
	static SemList* getSemList();
	static void setSemList(SemList*);
	void incVal();
private:
	static SemList* semList;
	Queue* waitingSemaphoreList;
	int block(Time);
	void deblock();
	int val;
};

#endif