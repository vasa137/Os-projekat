#include "PCB.h"
#include "SCHEDULE.h"
#include "KernSem.h"
#include "List.h"
#include "Queue.h"

List::List() : first(NULL), last(NULL), number(0) {}

List::Elem::Elem(PCB* newpcb,Elem* nxt) : pcb(newpcb), next(nxt) {}

List::~List(){
	while(first != NULL){
		Elem* old = first;
		first = first->next;
		delete old;
	}
	number = 0;
	last = NULL;
}

void List::add(PCB* pcb){
	Elem* elem = new Elem(pcb);
	if(first == NULL) first = elem;
	else last->next = elem;
	last = elem;
	number++;
}

PCB* List::get(unsigned int threadId) const{
	Elem* curr = first;
	while(curr != NULL && curr->pcb->getId() != threadId){
		curr = curr->next;
	}
	if (curr==NULL) return NULL;
	return curr->pcb;
}

PCB* List::remove(unsigned int thrID){
	Elem* curr = first,*prev = NULL;
	
	while(curr != NULL && curr->pcb->getId() != thrID){
		prev = curr;
		curr = curr->next;
	}
	
	if(curr == NULL) return NULL;
	
	PCB* removedPCB = curr->pcb;
	
	if(prev == NULL) first = curr->next;
	else prev->next = curr->next;
	
	if(last == curr) last = prev;
	
	delete curr;
	number --;
	return removedPCB;
}

void List::removeParent(PCB* pcb){
	Elem* curr = first;
	while(curr != NULL){
		if(curr->pcb->getParent() == pcb ) {
			curr->pcb->setParent(NULL);
		}
		curr = curr->next;
	}
}

PCB* List::getFirstPCB() const {
	if(first == NULL) return NULL;
	else return first->pcb;
}

PCB* List::removeFirstPCB() {
	if(first == NULL) return NULL;
	return remove(first->pcb->getId());
}

int List::isEmpty() const {
	return first == NULL;
}

int List::getNum() const{
	return number;
}

void List::sort_insert(PCB *pcbinfo){
	
	Elem* curr = first;
	Elem* prev = NULL;
	Elem* newElem = new Elem(pcbinfo);
	
	while(curr != NULL && pcbinfo->blockTimeLeft>curr->pcb->blockTimeLeft){
		prev = curr;
		pcbinfo->blockTimeLeft-=curr->pcb->blockTimeLeft;
		curr = curr->next;
	}
	
	newElem->next=curr;
	
	if(prev == NULL) first = newElem;
	else prev->next = newElem;
	
	if(curr != NULL) curr->pcb->blockTimeLeft-=pcbinfo->blockTimeLeft;
	else last=newElem;
	
	number ++;
}

void List::decTime(){
	if(first!=NULL && --first->pcb->blockTimeLeft==0){	
		while(first != NULL && first->pcb->blockTimeLeft == 0){
			
			PCB* unblockedPCB = first->pcb;
			Elem* oldElem=first;
			first=first->next;
			
			unblockedPCB->blockSem->incVal();
			unblockedPCB->setUnblockedBySignal(0);
			unblockedPCB->state=READY;
		    Scheduler::put(unblockedPCB);
			
			unblockedPCB->blockSem->getSemaphoreQueue()->remove(unblockedPCB->threadId);
			unblockedPCB->blockSem=NULL;
			
			delete oldElem;
			
			number--;	
		}
		if(first==NULL) last=NULL;
	}
}

void List::removePCB(PCB* toDelete){
	
	Elem* curr = first;
	Elem* prev = NULL;
	
	while(curr != NULL && (curr->pcb != toDelete)){
		prev = curr;
		curr = curr->next;
	}
	
	if(curr == NULL) return;
	
	if (curr->next!=NULL) {
		curr->next->pcb->blockTimeLeft += curr->pcb->blockTimeLeft;
	}
	
	if(prev == NULL) first = curr->next;
	else prev->next = curr->next;
	
	if(last == curr) last = prev;
		
	delete curr;
	
	number --;
}
