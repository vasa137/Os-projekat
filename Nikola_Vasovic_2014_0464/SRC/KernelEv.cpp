#include "KernelEv.h"
#include "PCB.h"
#include "lock.h"
#include "SCHEDULE.h"
#include "IVTEntry.h"
#include <iostream.h>

//global array with information about each entry (256 entries)
extern IVTEntry* IVTEntries[];

/*constructor, destructor*/
KernelEv::KernelEv(IVTNo ivtNo) : val(0), entryNum(ivtNo) {
	eventCreator = PCB::running;
	IVTEntries[ivtNo]->setKernelEv(this);
}

KernelEv::~KernelEv() {
	IVTEntries[entryNum]->setKernelEv(NULL);
}

/*wait*/
void KernelEv::block() {
	eventCreator->setState(BLOCKED);
	_unlock_
	dispatch();
}

void KernelEv::wait() {
	if(eventCreator == PCB::running){
		if(val-- == 0) {
			block();
		}
	}
}

/*signal, val can be in {-1, 0, 1}, -1,  if val == 1 don't increment*/
void KernelEv::deblock(){
	eventCreator->setState(READY);
	Scheduler::put(eventCreator);

}

void KernelEv::signal() {
	if(val < 1 && val++ < 0) deblock();
}





















