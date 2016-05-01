#ifndef IVTENTRY_H_
#define IVTENTRY_H_

#include "event.h"

typedef void interrupt (*pInterrupt)(...);

class IVTEntry{
public:
	IVTEntry(IVTNo ivtNo, pInterrupt interruptRoutine);
	void signal();
	void callOld();
	~IVTEntry();
	void setKernelEv(KernelEv* event);
	static IVTEntry* getEntry(IVTNo ivtNo);
private:
	int entryNum;
	pInterrupt oldRoutine;
	KernelEv* kernelEvent;
};

#endif