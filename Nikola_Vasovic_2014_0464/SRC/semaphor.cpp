#include "Semaphor.h"
#include "KernSem.h"
#include "lock.h"


Semaphore::Semaphore (int init){
	asm{
		pushf
		cli
	}
	myImpl = new KernelSem(init);
	asm popf
}

Semaphore::~Semaphore (){
	asm{
		pushf
		cli
	}
	delete myImpl;
	asm popf
}

int Semaphore::wait(Time maxTimeToWait){
	asm{
		pushf
		cli
	}
	int waitResult = myImpl->wait(maxTimeToWait);
	asm popf
	return waitResult;
}

void Semaphore::signal(){
	asm{
		pushf
		cli
	}
	myImpl->signal();
	asm popf
} 

int Semaphore::val () const{ return myImpl->getVal();} 