#include "Kernel.h"
#include "PCB.h"
#include "SCHEDULE.h"
#include "IdleThr.h"
#include "UserMain.h"
#include "lock.h"
#include "KernSem.h"
#include "List.h"
#include "SortList.h"
#include <dos.h>
#include <iostream.h>

/*global variables*/

unsigned int originalTimerOff, originalTimerSeg; // old address of timer interrupt routine
unsigned int newTimerOff, newTimerSeg; // new address of timer routine

unsigned int glss, glsp, glbp; // global variables for stack segment, stack pointer and base pointer

volatile unsigned int contextChangeRequested = 0, copyContextChangeRequested = 0, contextChangeLocked = 0; // copy for easier manipulation in interrupt routine

volatile unsigned int locked = 1; // LOCKED at start

volatile PCB* removablePCB = NULL;

/*Kernel instance*/
Kernel* Kernel::instance = NULL;

/*instance getter*/
Kernel* Kernel::getInstance(){
   if (instance == NULL){
        instance = new Kernel();
   }
   return instance;
}

/*static fields initialization*/
PCB* Kernel::mainPCB = new PCB(NULL, 0, 0);
IdleThread* Kernel::idleThread = new IdleThread();
UserMainThread* userMainThread = NULL;

/* declaration only */
void tick(); 

//interrupt routine 08h
void interrupt Kernel::newTimer(...){	
	copyContextChangeRequested = contextChangeRequested;
	
	if(!contextChangeRequested ) KernelSem::updateTime();
	
	if(!contextChangeRequested && PCB::running->timeSlice != 0 && PCB::running->timeLeft > 0) PCB::running->timeLeft--;
	
	if( ( PCB::running->timeSlice != 0  && PCB::running->timeLeft == 0 ) || contextChangeRequested || contextChangeLocked){
		if(!locked){	
			contextChangeRequested = 0;
			contextChangeLocked = 0;
			
			if(PCB::running->state == FINISHED){ removablePCB = PCB::running; }
			else removablePCB = NULL;
			
			if(PCB::running != NULL){
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
			}
			
			if( ( PCB::running = Scheduler::get() ) == NULL ){
				if(PCB::numberOfActiveThreads > 0 )PCB::running = getInstance()->idleThread->myPCB;
				else PCB::running = getInstance()->mainPCB;	
			}
			
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
			contextChangeLocked = 1;
		} 
	}
		
	if(!copyContextChangeRequested){
		tick();
		asm int 60h
	} 
	
}

/*START UP*/

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
	mainPCB->state = RUNNING;
	PCB::running = mainPCB;
	idleThread->start();
}

void Kernel::startUp(){
	initIVT();
	initSystemThreads();
}

///////////////////////////////////////////

/*init UserMain and waitForAllUserThreads */

void Kernel::initUserMainThread(int argc, char** argv){
	userMainThread = new UserMainThread(argc, argv);
	userMainThread->start();
}

void Kernel::waitForUserThreads() {
	mainPCB->setState(BLOCKED);
	_unlock_; 
	dispatch();
	while(!PCB::allThreadList->isEmpty()) {
		delete PCB::allThreadList->removeFirstPCB();
	}
	delete PCB::allThreadList;
}

////////////////////////////////////////////////////////////

/* TERMINATE */

void Kernel::restoreIVT() {
	asm{
		pushf
		cli
	}
	setvect(8,oldRoutine);
	asm popf
}

void Kernel::removeSystemThreads(){
	delete idleThread->getIdlePCB();
	delete idleThread;
	delete mainPCB;
}

void Kernel::removeWaitList(){
	delete KernelSem::waitingList;
}

void Kernel::terminate() {
	waitForUserThreads();
	removeWaitList();
	removeSystemThreads();
	restoreIVT();
}

//////////////////////////////////////////

/*getters for static fields, fields must be static because they must initialized before creation of any thread*/

PCB* Kernel::getmainPCB() { return mainPCB; }

IdleThread* Kernel::getIdleThread() { return idleThread; }