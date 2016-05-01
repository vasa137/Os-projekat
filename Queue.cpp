#include "Queue.h"
#include "PCB.h"
#include "SCHEDULE.h"
#include "Semaphore.h"
#include "KernSem.h"

void Queue::timeDecrement(KernelSem* kersem){
	List::Elem* curr = queueList.getFirstElem(),*prev = NULL;
	while(curr != NULL){
		if(curr->pcb->blockTimeLeft > 0){
			if(--curr->pcb->blockTimeLeft == 0){
				PCB* pcb = curr->pcb;
				List::Elem* next = curr->next;
				queueList.removeElem(prev,curr);
				curr = next;
				pcb->setState(READY);
				pcb->setUnblockedBySignal(0);
				kersem->incVal();
				Scheduler::put(pcb);
				continue;
			}
		}
		prev = curr;
		curr = curr->next;
	}
}
