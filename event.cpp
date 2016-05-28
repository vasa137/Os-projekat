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
	asm{
		pushf
		cli
	}
	myImpl->wait();
	asm popf
}

void Event::signal(){
	myImpl->signal();
}
