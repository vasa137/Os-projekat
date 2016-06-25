#ifndef _PCB_H_
#define _PCB_H_
#include <stdlib.h>

#include "thread.h"
#include "Semaphor.h"

class List;

typedef enum {NEW, READY, RUNNING, BLOCKED, FINISHED} State;

class KernelSem;

class PCB {
public:
	PCB(Thread*, StackSize, Time);
	~PCB();

	//public static field
	static PCB* running;
	static volatile int numberOfActiveThreads;
	
	//regular PCB methods
	List* getPendingList() const;
	void freePendingList();
	void createThread();
	
	//fork methods
	void remakeChildPCB(PCB* myparent);
	
	//static list getter
	static List* getThreadList();
	
	//regular PCB getters
	StackSize getStackSize() const;
	Time getTimeSlice() const;
	State getState() const;
	Thread* getThread() const;
	ID getId() const;
	
	//Semaphore getters
	KernelSem* getBlockSem() const;
	Time getBlockTime() const;
	
	//Fork getters
	int getNumOfChildren() const;
	Semaphore& getChildrenSemaphore();
	PCB* getParent() const;

	//regular PCB setters
	void setState(State);
	void setThread(Thread*);
	
	//Semaphore setters
	void setBlockSem(KernelSem* s);
	void setUnblockedBySignal(int);
	
	//Fork setters
	void incNumOfChildren();
	void setNumOfChildren(int n);
	void setParent(PCB* par);
private:
	/*static method starter*/
	static void wrapper();
	
	/*fork methods*/
	void copyStack(PCB*);
	
	/*static fields*/
	static ID threadNum;
	static List* allThreadList;

	/*regular PCB fields*/
	ID threadId;
	Thread *thread;
	Time timeSlice;
	Time timeLeft;
	StackSize stackSize;
	unsigned int* stack;
	unsigned int sp, ss, bp;
    State state;
	List* pendingList;
	
	/*Semaphore fields*/
	Time blockTimeLeft;
	int unblockedBySignal;
	KernelSem* blockSem;
	
	/*fork fields*/
	PCB* parent;
	int numOfChildren;
	Semaphore childrenSemaphore;
		
	/*friends*/
	friend class Kernel;
	friend class List;
	friend class KernelSem;
	
};

#endif