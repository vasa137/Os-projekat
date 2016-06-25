#include "KernSem.h"
#include "PCB.h"
#include "SCHEDULE.h"
#include "lock.h"
#include "SortList.h"
#include "Queue.h"
#include <iostream.h>

/*initialization*/
SortedList* KernelSem::waitingList = new SortedList();

/*constructor,destructor*/
KernelSem::KernelSem(int value) : val(value), semaphoreQueue(new Queue()) {}

KernelSem::~KernelSem(){ delete semaphoreQueue; }

/*wait*/
int KernelSem::block(Time maxTimeToWait){
	PCB::running->blockTimeLeft = maxTimeToWait;
	
	if (maxTimeToWait!=0){
		waitingList->insert(PCB::running);
		PCB::running->setBlockSem(this);
	}
	else PCB::running->setBlockSem(NULL);
	
	PCB::running->state = BLOCKED;
	semaphoreQueue->insert_q(PCB::running);
	dispatch();
	
	return PCB::running->unblockedBySignal;
}

int KernelSem::wait(Time maxTimeToWait){
	if(val-- <= 0) return block(maxTimeToWait);
	else return 2;
}

/*signal*/
void KernelSem::deblock(){
	PCB* pcb = semaphoreQueue->delete_q();
	if (pcb->getBlockSem() != NULL){
		waitingList->remove(pcb);
		pcb->setBlockSem(NULL);
	}
	pcb->state = READY;
	pcb->unblockedBySignal = 1;
	Scheduler::put(pcb);
}

void KernelSem::signal(){
	if(val++ < 0) deblock();
}

/*time signal*/
void KernelSem::updateTime(){
	waitingList->decTime();
}

/*getters*/

int KernelSem::getVal() const { return val; }

Queue* KernelSem::getSemaphoreQueue() const { return semaphoreQueue; }

/*setters*/

void KernelSem::incVal() { val ++; }

