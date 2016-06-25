#ifndef KERNELEV_H_
#define KERNELEV_H_

typedef unsigned char IVTNo;
class PCB;

class KernelEv{
public:
	KernelEv(IVTNo);
	~KernelEv();
	
	//event operations
	void wait();
	void signal();
	
private:
	volatile int val; // event value
	int entryNum; // number of entry linked with this event
	PCB* eventCreator; // thread that created object, only this thread can be blocked
	
	void block();
	void deblock();
};

#endif