#include "KernSem.h"
#include "PCB.h"
#include "SCHEDULE.h"
#include "lock.h"
#include <iostream.h>

extern volatile unsigned int semflag;

//SemList* KernelSem::semList = new SemList();

List* KernelSem::TimeList = new List(); // OBRISI LISTU

KernelSem::KernelSem(int value) : val(value), waitingSemaphoreList(new List()) {} //semList->add(this);	

int KernelSem::getVal() const { return val; }

//SemList* KernelSem::getSemList() { }// return semList; 

List* KernelSem::getWaitingSemaphoreList() const volatile { return waitingSemaphoreList; }

int KernelSem::wait(Time maxTimeToWait){
	if(val-- <= 0) return block(maxTimeToWait);
	else return 2;
}

void KernelSem::signal(){
	if(val++ < 0) deblock();
}

int KernelSem::block(Time maxTimeToWait){
	if (maxTimeToWait!=0){
		PCB::running->blockTimeLeft = maxTimeToWait;
		TimeList->sort_insert(PCB::running);
		PCB::running->setBlockSem(this);
	}
	else PCB::running->setBlockSem(NULL);
	PCB::running->state = BLOCKED;
	waitingSemaphoreList->add(PCB::running);
	dispatch();
	return PCB::running->unblockedBySignal;
}

void KernelSem::deblock(){
	PCB* pcb = waitingSemaphoreList->removeFirstPCB();
	if (pcb->getBlockSem() != NULL){
		TimeList->remove_fromTimerList(pcb);
		pcb->setBlockSem(NULL);
	}

	pcb->state = READY;
	pcb->unblockedBySignal = 1;
	Scheduler::put(pcb);
}

void KernelSem::updateTime(){
	TimeList->decTime();
	//semList->updateSemaphorsTime(); 
}

KernelSem::~KernelSem(){
	delete waitingSemaphoreList;
	//semList->remove(this);
}

void KernelSem::incVal()volatile { val ++; }

