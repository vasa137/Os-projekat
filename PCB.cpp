#include "PCB.h"
#include "SCHEDULE.h"
#include "Kernel.h"
#include "lock.h"
#include "IdleThr.h"
#include <dos.h>
#include <iostream.h>

ID PCB::threadNum; 
List* PCB::allThreadList = new List(); // list of all PCBs
PCB* PCB::running = NULL;

ID PCB::getId() const { return threadId; }

List* PCB::getThreadList(){ return allThreadList; }

Thread* PCB::getThread() const { return thread; }

List* PCB::getPendingList() const { return pendingList; }

void PCB::createThread(){
	/*correction if stackSize exceeds limits*/
	if(stackSize < minStackSize) stackSize = minStackSize;
	if(stackSize > maxStackSize) stackSize = maxStackSize;
	
	stack = new unsigned int[stackSize];
	
	// stack : PSWI, cs, ip, bp, di, si, ds, es, dx, cx, bx, ax
	
	stack[stackSize - 1] = 0x200; // initialize PSWI=1
	stack[stackSize - 2] = FP_SEG(&(PCB::wrapper)); // initialize segment of thread function wrapper
	stack[stackSize - 3] = FP_OFF(&(PCB::wrapper)); // initialize offset of thread function wrapper
	
	/*stack pointer and stack segment initialization*/
	ss = FP_SEG(stack + stackSize - 1 - 11); 
	sp = FP_OFF(stack + stackSize - 1 - 11);
	
	/*base pointer initialization*/
	bp = FP_OFF(stack + stackSize - 1 - 11);
}

PCB::PCB(Thread* thr, StackSize ssize, Time tslice) : threadId(threadNum++), thread(thr), stackSize(ssize), timeSlice(tslice), timeLeft(tslice), state(NEW),
  stack(NULL), ss(0), sp(0), bp(0), blockTimeLeft(0), unblockedBySignal(0), parent(NULL), numOfChildren(0), childrenSemaphore(0) {
	/* important, dont compare with threadId, because Threads can be global and initialize before system PCBs(threads)*/
	if(this != Kernel::getmainPCB() && this != Kernel::getIdleThread()->getIdlePCB() ) {
		pendingList = new List();
		pendingList->add(Kernel::getmainPCB());
		allThreadList->add(this);
	}
}

PCB::~PCB() {
	delete pendingList;
	delete [] stack;
	thread->myPCB = NULL; /*important*/
}

void PCB::freePendingList() {
	PCB* pcb;
	while((pcb = ( pendingList->removeFirstPCB() ) ) != NULL ){
		if(pcb->state != READY){
			pcb->state = READY;
			Scheduler::put(pcb);
		}
	}
}

void PCB::wrapper(){
	running->thread->run();
	
	_lock_
	running->setState(FINISHED);
	running->freePendingList();
	if(running->parent != NULL) running->parent->childrenSemaphore.signal();
	_unlock_
	dispatch();
}

void PCB::incNumOfChildren() { numOfChildren++;}

void PCB::setThread(Thread* thr){ thread = thr; }

void PCB::setState(State state){ this->state = state; }
State PCB::getState() const { return state; }
void PCB::setUnblockedBySignal(int signal){ unblockedBySignal = signal; }

void PCB::copyStack(const PCB& parPCB){	}
/*
	stack = new unsigned int[stackSize];
	
	unsigned int begss = FP_SEG(parentpcb.stack + stackSize - 1); 
	unsigned int begsp = FP_OFF(parentpcb.stack + stackSize - 1);
	
	unsigned int numloc=((long)begss*16+(long)begsp)-((long)parPCB.ss*16+(long)parPCB.sp);
	
	if(!(numloc % 2)){
		numloc++;
	}
	numloc/=2;
	
	sp = FP_OFF(stack + stackSize - numloc);
	
	
	
	for(int i = )
	
}
*/
int PCB::getNumOfChildren() const { return numOfChildren;} 
Semaphore& PCB::getChildrenSemaphore() { return childrenSemaphore; }
PCB* PCB::getParent()const { return parent; }

PCB::PCB(PCB& parPCB) : threadId(threadNum++), thread(NULL), stackSize(parPCB.stackSize), timeSlice(parPCB.timeSlice), timeLeft(parPCB.timeSlice), state(READY),
  stack(NULL), ss(0), sp(0), bp(0), blockTimeLeft(0), unblockedBySignal(0), parent(&parPCB), numOfChildren(0), childrenSemaphore(0) {
	pendingList = new List();
	pendingList->add(Kernel::getmainPCB());
	copyStack(parPCB);
	allThreadList->add(this);
	Scheduler::put(this);
}