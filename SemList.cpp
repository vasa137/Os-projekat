#include "SemList.h"
#include "KernSem.h"
#include "Queue.h"
#include <stdlib.h>

SemList::Elem::Elem(KernelSem* ksem) : kernelSem(ksem), next(NULL) { }

SemList::SemList() : first(NULL), last(NULL) {}

void SemList::add(KernelSem* kersem){
	Elem* new_elem = new Elem(kersem);
	if(!first) first = new_elem;
	else last->next = new_elem;
	last = new_elem;
}

void SemList::remove(KernelSem* kersem){
	Elem* curr = first,* prev = NULL;
	
	while(curr != NULL && curr->kernelSem != kersem){
		prev = curr;
		curr = curr->next;
	}
	
	if(curr == NULL) return;
	
	if(prev == NULL) first = curr->next;
	else prev->next = curr->next;
	
	if( first == NULL ) last = NULL;
	
	delete curr;
}

SemList::~SemList(){
	while(first != NULL){
		Elem* temp = first;
		first = first->next;
		delete temp;
	}
	last = NULL;
}

void SemList::updateSemaphorsTime(){
	Elem* curr = first;
	while(curr != NULL){
		curr->kernelSem->getWaitingSemaphoreList()->timeDecrement(curr->kernelSem); // We must send semaphore parameter to decrease val
		curr = curr->next;
	}
}