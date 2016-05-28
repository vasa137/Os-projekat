#include "KernelEv.h"
#include "PCB.h"
#include "lock.h"
#include "SCHEDULE.h"
#include "IVTEntry.h"
#include <iostream.h>
extern IVTEntry* IVTEntries[];

KernelEv::KernelEv(IVTNo ivtNo) : val(0), entryNum(ivtNo) {
	eventCreator = PCB::running;
	IVTEntries[ivtNo]->setKernelEv(this);
}

KernelEv::~KernelEv() {
	IVTEntries[entryNum]->setKernelEv(NULL);
}

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

void KernelEv::deblock(){
	eventCreator->setState(READY);
	Scheduler::put(eventCreator);

}

/* val can be in {-1, 0, 1}, -1 -> thread suspended */

void KernelEv::signal() {
	cout << "USAO SIGNAL" << endl;
	if(val < 1 && val++ < 0) deblock();
}
/* if val == 1 don't increment */




















