#include "PCB.h"

List::List() : first(NULL), last(NULL), numberOfElements(0) { }

List::Elem::Elem(PCB* prcb) : pcb(prcb), next(NULL) {}

List::~List(){
	while(first != NULL){
		Elem* temp = first;
		first = first->next;
		delete temp;
	}
	last = NULL;
}

void List::add(PCB* pcb){
	Elem* new_elem = new Elem(pcb);
	if(!first) first = new_elem;
	else last->next = new_elem;
	last = new_elem;
	numberOfElements ++;
}

PCB* List::get(ID threadId) const{
	Elem* curr = first;
	
	while(curr != NULL && curr->pcb->getId() != threadId){
		curr = curr->next;
	}
	
	return curr->pcb;
}

PCB* List::remove(ID threadId){
	Elem* curr = first,* prev = NULL;
	
	while(curr != NULL && curr->pcb->getId() != threadId){
		prev = curr;
		curr = curr->next;
	}
	
	if(curr == NULL) return NULL;
	
	if(prev == NULL) first = curr->next;
	else prev->next = curr->next;
	
	if( first == NULL ) last = NULL;
	
	PCB* pcb = curr->pcb;
	delete curr;
	numberOfElements --;
	return pcb;
}

PCB* List::getFirstPCB() const { return first->pcb; }

PCB* List::removeFirstPCB() {
	if(first == NULL) return NULL;
	return remove(first->pcb->getId());
}

int List::isEmpty() const {
	return first == NULL;
}

int List::getNumOfElements() const{ return numberOfElements;}

int List::onlyNewThreads() const {
	if(first == NULL) return -1; /*ako nema nijedne*/
	Elem* curr = first;
	while(curr!= NULL){
		if(curr->pcb->getState() != NEW ) return 0;
		curr = curr->next;
	}
	return 1;
}

void List::removeElem(Elem* prev, Elem*curr){
	if(curr == NULL) return;
	
	if(prev == NULL) first = curr->next;
	else prev->next = curr->next;
	
	if( first == NULL ) last = NULL;
}

List::Elem* List::getFirstElem() const { return first; }