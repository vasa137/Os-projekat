#include "PCB.h"
#include "IdleThr.h"

//constructor-> stackSize 32 and timeSlice = 1
IdleThread::IdleThread() : Thread(32,1){}
IdleThread::~IdleThread() { delete myPCB; }

//start is different because Idle will never be in Scheduler
void IdleThread::start(){
	myPCB->setState(BLOCKED);
	myPCB->createThread();
}

void IdleThread::run(){
	while(13);
}

/*PCB getter*/
PCB* IdleThread::getIdlePCB() const{ return myPCB; }