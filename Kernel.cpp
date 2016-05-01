#include "Kernel.h"
#include "PCB.h"
#include "SCHEDULE.h"
#include "IdleThr.h"
#include "UserMain.h"
#include "lock.h"
#include "KernSem.h"
#include "SemList.h"
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

void interrupt Kernel::newTimer(){	
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
	/*move timer routine on 60h and swap with our interrupt routine*/
	asm{
		/*save registers*/
		push es 
		push ax
		
		
		mov ax,0 
		mov es,ax
		
		/*get original timer segment and offset from entry no. 08h*/
		mov ax, word ptr es:0022h // segment of original timer
		mov word ptr originalTimerSeg, ax // save original timer segment
		mov ax, word ptr es:0020h // offset of original timer
		mov word ptr originalTimerOff, ax // save original timer offset
	}
	
	
	
	newTimerSeg = FP_SEG(&Kernel::newTimer);
	newTimerOff = FP_OFF(&Kernel::newTimer);
	
    asm{
		/*move new interrupt routine to position 08h*/
		mov ax,newTimerSeg
		mov word ptr es:0022h,ax
		mov ax,newTimerOff
		mov word ptr es:0020h,ax
		
		/*move original timer to 60h*/
		mov ax, originalTimerSeg 
		mov word ptr es:0182h, ax 
		mov ax, originalTimerOff
		mov word ptr es:0180h, ax
		
		/*restore registers*/
		pop ax
		pop es
	}



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
		/*save registers*/
		push es
		push ax
		
		mov ax,0
		mov es,ax
		
		/*restore original timer on int 08h*/
		mov ax, word ptr originalTimerSeg
		mov word ptr es:0022h, ax
		mov ax, word ptr originalTimerOff
		mov word ptr es:0020h, ax
		
		/*restore registers*/
		pop ax
		pop es
	}
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
		counter = PCB::allThreadList->getNumOfElements();
		while(counter != 0) {
			pcb = PCB::allThreadList->removeFirstPCB();
			if(pcb->getState() == FINISHED || pcb->getThread() == NULL) {
				delete pcb;
			} 
			else PCB::allThreadList->add(pcb);
			counter --;
		}
		onlynewthreads = PCB::getThreadList()->onlyNewThreads();
	}while(!PCB::allThreadList->isEmpty() && !onlynewthreads);
	
	/*Delete passive threads*/
	if(onlynewthreads){
		counter = PCB::allThreadList->getNumOfElements();
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

void Kernel::removeSemaphoreList(){ 
	delete KernelSem::getSemList();
}

void Kernel::terminate() {
	waitForUserThreads();
	removeSemaphoreList();
	removeSystemThreads();
	restoreIVT();
}