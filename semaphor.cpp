#include "Semaphor.h"
#include "KernSem.h"
#include "lock.h"


Semaphore::Semaphore (int init){
	_lock_
	myImpl = new KernelSem(init);
	_unlock_
}

Semaphore::~Semaphore (){
	_lock_
	delete myImpl;
	_unlock_
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