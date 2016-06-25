#include "event.h"
#include "lock.h"
#include "KernelEv.h"
#include <stdlib.h>

Event::Event (IVTNo ivtNo){
	asm{
		pushf
		cli
	}
	if(IVTEntry::getEntry(ivtNo) == NULL) myImpl = new KernelEv(ivtNo);
	else myImpl = NULL;
	asm popf
}

Event::~Event (){
	asm{
		pushf
		cli
	}
	delete myImpl;
	asm popf
}

//interrupt masked because in half of wait, signal can be called from interrupt and this is a critical section for event value
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
