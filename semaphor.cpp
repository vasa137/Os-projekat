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
	_lock_
	int waitResult = myImpl->wait(maxTimeToWait);
	_unlock_
	return waitResult;
}

void Semaphore::signal(){
	_lock_
	myImpl->signal();
	_unlock_
} 

int Semaphore::val () const{ return myImpl->getVal();} 