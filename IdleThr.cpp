#include "PCB.h"
#include "IdleThr.h"
#include "SCHEDULE.h"
#include <iostream.h>

void IdleThread::start(){
	myPCB->setState(READY);
	myPCB->createThread();
}

void IdleThread::run(){
	while(13);
}

PCB* IdleThread::getIdlePCB() const{
	return myPCB;
}