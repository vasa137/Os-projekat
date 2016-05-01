#include "PCB.h"
#include "SCHEDULE.h"
#include "Kernel.h"
#include "IdleThr.h"
#include "lock.h"
#include "Semaphor.h"
#include <dos.h>
#include <iostream.h>
extern volatile unsigned int contextChangeRequested;

unsigned int dispatchSeg, dispatchOff;

void Thread::start(){
	_lock_ //
	myPCB->setState(READY);
	myPCB->createThread();
	Scheduler::put(myPCB);
	_unlock_//
}

void Thread::waitToComplete(){
	_lock_
	if(myPCB == NULL ||  myPCB == PCB::running  ||  myPCB == Kernel::getInstance()->getIdleThread()->getIdlePCB() || myPCB->getState() == FINISHED || myPCB->getState() == NEW ){
		_unlock_
		return; 
	} 
	
	PCB::running->setState(BLOCKED);
	myPCB->getPendingList()->add(PCB::running);
	_unlock_//
	
	dispatch();
} 

Thread::~Thread(){
	_lock_
	waitToComplete();
	_lock_
	
	myPCB->setThread(NULL);
	
	if(myPCB == PCB::running){
		PCB::running->setState(FINISHED);
		PCB::running->freePendingList();
		if(PCB::running->getParent() != NULL) PCB::running->getParent()->getChildrenSemaphore().signal();
		dispatchSeg = FP_SEG(&dispatch);
		dispatchOff = FP_OFF(&dispatch);
		asm{
			/*POP DS,SI,BP*/
			pop ax
			pop ax
			pop ax
			/*POP IP,CS*/
			pop ax
			pop ax
			
			/*put dispatch*/
			mov ax,dispatchSeg
			push ax
			mov ax,dispatchOff
			push ax
			
			/*push random values for DS,SI,BP, we will never come back to this thread, so it isn't important*/
			push ax
			push ax
			push ax
		}
	}
	_unlock_
} 

ID Thread::getId() const {
	return myPCB->getId(); 
}

ID Thread::getRunningId(){ 
	return PCB::running->getId(); 
}

Thread* Thread::getThreadById(ID id){
	PCB* pcb = PCB::getThreadList()->get(id);
	if( pcb != NULL ) return pcb->getThread();
	else return NULL;
} 

Thread::Thread (StackSize stackSize, Time timeSlice){
	_lock_
	myPCB = new PCB(this, stackSize, timeSlice);
	_unlock_
} 

void dispatch (){
	asm{
		pushf
		cli
	}

	contextChangeRequested = 1;
	
	asm{
		int 08h
		popf
	}
}

Thread::Thread(const Thread& parent){
	myPCB = new PCB(*(parent.myPCB));
	myPCB->setThread(this);
	parent.myPCB->incNumOfChildren();
}

ID Thread::fork(){
	ID id = Thread::getRunningId();
	Thread* child = PCB::running->getThread()->clone();
	if(id == Thread::getRunningId()){
		return child->getId();
	}
	else if(Thread::getRunningId() == child->getId()){
		  return 0;
	}
	else return -1;
} 

void Thread::exit() {
	delete PCB::running->getThread();
}

void Thread::waitForForkChildren(){
	for(int i = 0; i< PCB::running->getNumOfChildren(); i++){
		PCB::running->getChildrenSemaphore().wait(0);
	}
}

Thread* Thread::clone() const {
	return new Thread(*this);
} 