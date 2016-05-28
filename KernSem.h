#ifndef KERNSEM_H_
#define KERNSEM_H_

#include "Semaphore.h"
class List;

class KernelSem{
public:
	KernelSem(int);
	int getVal() const;
	int wait(Time);
	void signal();
	~KernelSem();
	static void updateTime();
	List* getWaitingSemaphoreList() const volatile ;
	static List* TimeList;
	void incVal() volatile;
private:
	//static SemList* semList;
	List* waitingSemaphoreList;
	int block(Time);
	void deblock();
	volatile int val;
};

#endif