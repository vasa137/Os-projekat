#ifndef IVTENTRY_H_
#define IVTENTRY_H_

class KernelEv;
typedef unsigned char IVTNo;
typedef void interrupt (*pInterrupt)(...);

class IVTEntry{
public:
	IVTEntry(IVTNo ivtNo, pInterrupt interruptRoutine);
	~IVTEntry();
	
	void signal();
	void callOldRoutine();

	//getter
	static IVTEntry* getEntry(IVTNo ivtNo);
	
	//setter
	void setKernelEv(KernelEv* event);
	
private:
	int entryNum;
	pInterrupt oldRoutine;
	KernelEv* kernelEvent;
};

#endif