#include "List.h"
#include "SortList.h"

SortedList::SortedList() { sortedList = new List(); }

SortedList::~SortedList(){ delete sortedList; }

void SortedList::insert(PCB* pcb){
	sortedList->sort_insert(pcb);
}

int SortedList::isEmpty() const{
	return sortedList->isEmpty();
}

void SortedList::remove(PCB* pcb){
	sortedList->removePCB(pcb);
}

void SortedList::decTime(){
	sortedList->decTime();
}

PCB* SortedList::getFirstPCB() const{
	return sortedList->getFirstPCB();
}
	