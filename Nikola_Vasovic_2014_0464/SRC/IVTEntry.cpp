#include "KernelEv.h"
#include <dos.h>
#include <stdlib.h>
#include "IVTEntry.h"

IVTEntry* IVTEntries[256]; // global array of IVT entries (256)

/*constructor, destructor*/
IVTEntry::IVTEntry(IVTNo ivtNo, pInterrupt interruptRoutine) : entryNum(ivtNo), kernelEvent(NULL) {
	IVTEntries[ivtNo] = this;
	oldRoutine = getvect(entryNum);
	setvect(entryNum, interruptRoutine);
}

IVTEntry::~IVTEntry(){
	setvect(entryNum, oldRoutine);
}

//dereferencing pointer to old routine
void IVTEntry::callOldRoutine(){
	(*oldRoutine) ();
}

//delegate signal to kernelEv
void IVTEntry::signal() {
	if(kernelEvent != NULL) kernelEvent->signal();
}

//static getter
IVTEntry* IVTEntry::getEntry(IVTNo ivtNo){
	return IVTEntries[ivtNo];
}

//setter
void IVTEntry::setKernelEv(KernelEv* event){
	kernelEvent = event;
}