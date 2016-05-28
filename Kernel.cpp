#include "Kernel.h"
#include "PCB.h"
#include "SCHEDULE.h"
#include "IdleThr.h"
#include "UserMain.h"
#include "lock.h"
#include "KernSem.h"
#include <dos.h>
#include <iostream.h>

unsigned int originalTimerOff, originalTimerSeg;
unsigned int newTimerOff, newTimerSeg;
unsigned int glss, glsp, glbp; // global variables for stack segment, stack pointer and base pointer
volatile unsigned int contextChangeRequested = 0, copyContextChangeRequested = 0;
volatile unsigned int locked = 1;

extern int returnValue;

void tick();

PCB* Kernel::mainPCB = new PCB(NULL, 0, 0);
IdleThread* Kernel::idleThread = new IdleThread();

Kernel* Kernel::instance = NULL;

Kernel* Kernel::getInstance(){
   if (instance == NULL){
        instance = new Kernel();
   }
   return instance;
}

UserMainThread::UserMainThread(int carg, char** varg) : argc(carg), argv(varg) { }

void UserMainThread::run(){ returnValue = userMain(argc,argv); }

UserMainThread* userMainThread = NULL;

void Kernel::initUserMainThread(int argc, char** argv){
	userMainThread = new UserMainThread(argc, argv);
	userMainThread->start();
}

void interrupt Kernel::newTimer(...){	
	copyContextChangeRequested = contextChangeRequested;
	
	if(!contextChangeRequested) KernelSem::updateTime();
	
	if(!contextChangeRequested && PCB::running->timeSlice != 0 && PCB::running->timeLeft > 0) PCB::running->timeLeft--;
	
	if( ( PCB::running->timeSlice != 0  && PCB::running->timeLeft == 0 ) || contextChangeRequested){
		if(!locked){	
			contextChangeRequested = 0;
			
		    asm{
				mov glss,ss
				mov glsp,sp
				mov glbp,bp
			}
			
			PCB::running->ss = glss;
			PCB::running->sp = glsp;
			PCB::running->bp = glbp;
			
			if( PCB::running->state != FINISHED && PCB::running->state != BLOCKED && PCB::running != getInstance()->idleThread->myPCB){
				PCB::running->state = READY;
				Scheduler::put(PCB::running);
			}
			if( ( PCB::running = Scheduler::get() ) == NULL ) PCB::running = getInstance()->idleThread->myPCB;
			
			PCB::running->state=RUNNING;
			
			glss = PCB::running->ss;
			glsp = PCB::running->sp;
			glbp = PCB::running->bp;
			
			if(PCB::running->timeLeft == 0) PCB::running->timeLeft = PCB::running->timeSlice;
			
			asm{
				mov sp, glsp
				mov ss, glss
				mov bp, glbp
			}
			
		}
		else{
			contextChangeRequested = 1;
		} 
	}
	
	
	if(!copyContextChangeRequested){
		tick();
		asm int 60h
	} 
	
}

void Kernel::initIVT(){	
	oldRoutine = getvect(8);
	asm{
		pushf
		cli
	}
	setvect(8, &newTimer);
	setvect(0x60, oldRoutine);
	asm popf
}

void Kernel::initSystemThreads(){
	mainPCB->state = RUNNING; //
	PCB::running = mainPCB;
	idleThread->start();
}

void Kernel::startUp(){
	initIVT();
	initSystemThreads();
}

PCB* Kernel::getmainPCB(){ return mainPCB; }

IdleThread* Kernel::getIdleThread() { return idleThread; }

void Kernel::restoreIVT() {
	asm{
		pushf
		cli
	}
	setvect(8,oldRoutine);
	asm popf
}

void Kernel::Mainblock(){
	mainPCB->setState(BLOCKED);
	_unlock_; //
	dispatch();
}

void Kernel::waitForUserThreads() {
	int counter,onlynewthreads;
	PCB* pcb;
	do{
		Mainblock();
		counter = PCB::allThreadList->getNum();
		while(counter != 0) {
			pcb = PCB::allThreadList->removeFirstPCB();
			if(pcb->getState() == FINISHED || pcb->getThread() == NULL) {
				delete pcb;
			} 
			else PCB::allThreadList->add(pcb);
			counter --;
		}
		onlynewthreads = PCB::getThreadList()->all_new();
	}while(!PCB::allThreadList->isEmpty() && !onlynewthreads);
	
	/*Delete passive threads*/
	if(onlynewthreads){
		counter = PCB::allThreadList->getNum();
		while(counter != 0) {
			pcb = PCB::allThreadList->removeFirstPCB();
			delete pcb;
			counter --;
		}
	}
}

void Kernel::removeSystemThreads(){
	delete idleThread;
	delete mainPCB;
}

void Kernel::removeSemaphoreList(){ }

void Kernel::terminate() {
	waitForUserThreads();
	removeSemaphoreList();
	removeSystemThreads();
	restoreIVT();
}