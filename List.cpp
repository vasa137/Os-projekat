#include "PCB.h"
#include "SCHEDULE.h"
#include "KernSem.h"

List::List() : first(NULL), last(NULL), number(0) {}

List::Elem::Elem(PCB* prcb) : pcb(prcb), next(NULL) {}

List::~List(){
	while(first != NULL){
		Elem* temp = first;
		first = first->next;
		delete temp;
	}
	number=0;
	last = NULL;
}

void List::add(PCB* pcb){
	Elem* newelem = new Elem(pcb);
	if(!first) first = newelem;
	else last->next = newelem;
	last = newelem;
	number++;
}

PCB* List::get(ID threadId) const{
	Elem* curr = first;
	while(curr != NULL && curr->pcb->getId()!= threadId){
		curr = curr->next;
	}
	if (curr==NULL) return NULL;
	return curr->pcb;
}

PCB* List::remove(ID threadId){
	Elem* curr = first,*prev = NULL;
	
	while(curr != NULL && curr->pcb->getId() != threadId){
		prev = curr;
		curr = curr->next;
	}
	
	if(curr == NULL) return NULL;
	
	if(prev == NULL) first = curr->next;
	else prev->next = curr->next;
	
	if(last == curr) last = prev;
	if( first == NULL ) last = NULL;
	
	PCB* pcb = curr->pcb;
	delete curr;
	number --;
	return pcb;
}

PCB* List::getFirstPCB() const{
	if(first==NULL) return NULL;
	return first->pcb;
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

int List::all_new() const{
   int num=0;
   Elem *fir=first;
   while(fir != NULL){
	    if(fir->pcb->getState() !=NEW) num++;
		fir = fir->next;
	}
   return (num==0);
}

void List::delete_all_new(){
   Elem *delPCB;
   while(first!= NULL){
	    delPCB=first;
		first= first->next;
		delete delPCB->pcb;
		delete delPCB;
	}
	last=NULL;
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
		
		while(first!=NULL && first->pcb->blockTimeLeft==0){
			
			PCB *elem=first->pcb;
			Elem* delElem=first;
			first=first->next;
			
			elem->blockSem->incVal();
			
			elem->setUnblockedBySignal(0);
	
			elem->state=READY;
		    Scheduler::put(elem);
			
			elem->blockSem->getWaitingSemaphoreList()->remove(elem->threadId);
			
			elem->blockSem=NULL;
			number--;
			delete delElem;
			
		}
		
		if(first==NULL) last=NULL;
	}
}

void List::remove_fromTimerList(PCB* DelPCB){
	
	Elem* curr = first;
	Elem*prev = NULL;
	
	while(curr != NULL && (curr->pcb!=DelPCB)){
		prev = curr;
		curr = curr->next;
	}
	if(curr == NULL) return;
	
	if (curr->next!=NULL) {
		curr->next->pcb->blockTimeLeft+=curr->pcb->blockTimeLeft;
	}
	
	if(prev == NULL) first = curr->next;
	else prev->next = curr->next;
	
	if(last == curr) last = prev;
	
	delete curr;
	number --;
	
}
