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
				push ax
				mov ax, dispatchSeg
				mov [bp+04], ax
				mov ax, dispatchOff
				mov [bp+02], ax
				pop ax
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

/*fork*/

ID Thread::fork(){
	_lock_
	ID id = Thread::getRunningId();
	Thread* child = PCB::running->getThread()->clone();
	_lock_
	child->myPCB->remakeChildPCB(PCB::running);
	_lock_
	if(id == Thread::getRunningId()){
		child->myPCB->setState(READY);
		Scheduler::put(child->myPCB);
		PCB::running->incNumOfChildren();
		_unlock_
		return child->getId();
	}
	else if(Thread::getRunningId() == child->getId()){
		_unlock_
		  return 0;
	}
	else{
		_unlock_
		return -1;
	} 
} 

void Thread::exit() {
	delete PCB::running->getThread();
}

void Thread::waitForForkChildren(){
	for(int i = 0; i < PCB::running->getNumOfChildren(); i++){
		PCB::running->getChildrenSemaphore().wait(0);
	}
	PCB::running->setNumOfChildren(0);
}

Thread* Thread::clone() const {
	return new Thread(myPCB->getStackSize(), myPCB->getTimeSlice());
} 
