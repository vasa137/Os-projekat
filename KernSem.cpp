#include "KernSem.h"
#include "SemList.h"
#include "Queue.h"
#include "PCB.h"
#include "SCHEDULE.h"
#include "lock.h"
#include <iostream.h>
SemList* KernelSem::semList = new SemList();

KernelSem::KernelSem(int value) : val(value), waitingSemaphoreList(new Queue()) { semList->add(this); }	

int KernelSem::getVal() const { return val; }

SemList* KernelSem::getSemList() { return semList; }

Queue* KernelSem::getWaitingSemaphoreList() const{ return waitingSemaphoreList; }

int KernelSem::wait(Time maxTimeToWait){
	if(val-- <= 0) return block(maxTimeToWait);
	else return 0;
}

void KernelSem::signal(){
	if(val++ < 0) deblock();
}

int KernelSem::block(Time maxTimeToWait){
	PCB::running->blockTimeLeft = maxTimeToWait;
	PCB::running->state = BLOCKED;
	waitingSemaphoreList->insert_q(PCB::running);
	_unlock_
	dispatch();
	_lock_
	return PCB::running->unblockedBySignal;
}

void KernelSem::deblock(){
	PCB* pcb = waitingSemaphoreList->delete_q();
	pcb->state = READY;
	pcb->unblockedBySignal = 1;
	Scheduler::put(pcb);
}

void KernelSem::updateTime(){
	semList->updateSemaphorsTime(); 
}

KernelSem::~KernelSem(){
	delete waitingSemaphoreList;
	semList->remove(this);
}

void KernelSem::incVal(){ val ++; }

