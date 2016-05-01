#ifndef KERNELEV_H_
#define KERNELEV_H_
#include "event.h"
class PCB;

class KernelEv{
public:
	KernelEv(IVTNo);
	void wait();
	void signal();
	~KernelEv();
private:
	void block();
	void deblock();
	PCB* eventCreator;
	int val;
	int entryNum;
};

#endif