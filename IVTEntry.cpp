#include "KernelEv.h"
#include <dos.h>
#include <stdlib.h>
#include "IVTEntry.h"

IVTEntry* IVTEntries[256];

IVTEntry::IVTEntry(IVTNo ivtNo, pInterrupt interruptRoutine) : entryNum(ivtNo), kernelEvent(NULL) {
	IVTEntries[ivtNo] = this;
	oldRoutine = getvect(entryNum);
	setvect(entryNum, interruptRoutine);
}


void IVTEntry::signal() {
	if(kernelEvent != NULL) kernelEvent->signal();
}

void IVTEntry::callOldRoutine(){
	(*oldRoutine) ();
}

IVTEntry::~IVTEntry(){
	setvect(entryNum, oldRoutine);
}

IVTEntry* IVTEntry::getEntry(IVTNo ivtNo){
	return IVTEntries[ivtNo];
}

void IVTEntry::setKernelEv(KernelEv* event){
	kernelEvent = event;
}