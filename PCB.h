#ifndef _PCB_H_
#define _PCB_H_
#include <stdlib.h>
#include "thread.h"
#include "Semaphor.h"
#include "List.h"


typedef enum {NEW, READY, RUNNING, BLOCKED, FINISHED} State;
class KernSem;
class PCB {
public:
	PCB(Thread*, StackSize, Time);
	~PCB();

	ID getId() const;
	Thread* getThread() const;
	void setThread(Thread*);
	static List* getThreadList();
	void setState(State);
	State getState() const;
	List* getPendingList() const;
	void freePendingList();
	void createThread();
	void setUnblockedBySignal(int);
	static PCB* running;
	void setBlockSem(KernelSem* s);
	KernelSem* getBlockSem();
	Time getBlockTime() { return blockTimeLeft;}
	StackSize getStackSize();
	Time getTimeSlice();
	
	int getNumOfChildren() const;
	Semaphore& getChildrenSemaphore();
	PCB* getParent() const;
	void incNumOfChildren();
	void setNumOfChildren(int n);
	void remakeChildPCB(PCB* myparent);
private:
	void copyStack(PCB*);
	ID threadId;
	Thread *thread;
	Time timeSlice;
	Time timeLeft;
	StackSize stackSize;
	unsigned int* stack;
	unsigned int sp, ss, bp;
    State state;
	List* pendingList;
	
	static ID threadNum;
	static List* allThreadList;
	
	friend class Kernel;
	friend class List;
	friend class KernelSem;
	static void wrapper();
	
	/*For semaphore*/
	volatile Time blockTimeLeft;
	volatile int unblockedBySignal;
	KernelSem* blockSem;
	
	/*For fork*/
	
	PCB* parent;
	int numOfChildren;
	Semaphore childrenSemaphore;
	
};

#endif