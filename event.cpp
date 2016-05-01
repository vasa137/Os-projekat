#include "event.h"
#include "lock.h"
#include "KernelEv.h"

Event::Event (IVTNo ivtNo){
	_lock_
	myImpl = new KernelEv(ivtNo);
	_unlock_
}

Event::~Event (){
	_lock_
	delete myImpl;
	_unlock_
}

void Event::wait (){
	_lock_
	myImpl->wait();
	_unlock_
}

void Event::signal(){
	_lock_
	myImpl->signal();
	_unlock_
}
