#ifndef KERNELEV_H_
#define KERNELEV_H_

typedef unsigned char IVTNo;
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
	volatile int val;
	int entryNum;
};

#endif