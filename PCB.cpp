#include "PCB.h"
#include "SCHEDULE.h"
#include "Kernel.h"
#include "lock.h"
#include "IdleThr.h"
#include <dos.h>
#include <iostream.h>

const StackSize minStackSize = 256; 
const StackSize maxStackSize = 32768; // 64KB

unsigned int parentSP,parentBP,parentSS;
unsigned int childSP,childBP, childSS;
unsigned int currBP,currSP,currSS, currAX;
unsigned int parLAST,iterBP, tempBP;
unsigned int childCS,childIP;

ID PCB::threadNum; 
List* PCB::allThreadList = new List(); // list of all PCBs
PCB* PCB::running = NULL;

ID PCB::getId() const { return threadId; }

List* PCB::getThreadList(){ return allThreadList; }

Thread* PCB::getThread() const { return thread; }

List* PCB::getPendingList() const { return pendingList; }

void PCB::setBlockSem(KernelSem* s) { blockSem = s;}
KernelSem * PCB::getBlockSem() { return blockSem;}

void PCB::createThread(){
	/*correction if stackSize exceeds limits*/
	if(stackSize < minStackSize) stackSize = minStackSize;
	if(stackSize > maxStackSize) stackSize = maxStackSize;
	
	stackSize=stackSize/2;
	stack = new unsigned int[stackSize];
	
	// stack : PSWI, cs, ip, ax , bx , cx , dx , es ,ds , si , di , bp

	stack[stackSize - 2] = 0x200; // initialize PSWI=1
	stack[stackSize - 3] = FP_SEG(&(PCB::wrapper)); // initialize segment of thread function wrapper
	stack[stackSize - 4] = FP_OFF(&(PCB::wrapper)); // initialize offset of thread function wrapper
	
	stack[stackSize - 13] = FP_OFF(stack + stackSize - 1);
	/*stack pointer and stack segment initialization*/
	ss = FP_SEG(stack + stackSize - 2 - 11); 
	sp = FP_OFF(stack + stackSize - 2 - 11);
	
	/*base pointer initialization*/
	bp = FP_OFF(stack + stackSize - 2 - 11);
}

PCB::PCB(Thread* thr, StackSize ssize, Time tslice) : threadId(threadNum++), thread(thr), stackSize(ssize), timeSlice(tslice), timeLeft(tslice), state(NEW),
  stack(NULL), ss(0), sp(0), bp(0), blockTimeLeft(0), unblockedBySignal(0), blockSem(NULL),
   parent(NULL), numOfChildren(0), childrenSemaphore(0) 
  {
	if(threadId > 1 ) {
		pendingList = new List();
		pendingList->add(Kernel::getmainPCB());
		allThreadList->add(this);
	}
}

void PCB::setNumOfChildren(int n){
	numOfChildren = n;
}

PCB::~PCB() {
	delete pendingList;
	delete [] stack;
	thread->myPCB = NULL; /*important*/
}

void PCB::freePendingList() {
	PCB* pcb;
	while((pcb = ( pendingList->removeFirstPCB() ) ) != NULL ){
		if(pcb->state != READY){
			pcb->state = READY;
			Scheduler::put(pcb);
		}
	}
}

void PCB::wrapper(){
	running->thread->run();
	
	_lock_
	running->setState(FINISHED);
	running->freePendingList();
	if(running->parent != NULL) running->parent->childrenSemaphore.signal();
	_unlock_
	dispatch();
}

void PCB::incNumOfChildren() { numOfChildren++;}

void PCB::setThread(Thread* thr){ thread = thr; }

void PCB::setState(State state){ this->state = state; }
State PCB::getState() const { return state; }
void PCB::setUnblockedBySignal(int signal) { unblockedBySignal = signal; }

void PCB::remakeChildPCB(PCB* myparent){
	parent=myparent;
	asm{
		mov parentBP,bp
		mov parentSS,ss
		mov parentSP,sp
	}
	copyStack(myparent);
}

//fork

void PCB::copyStack(PCB* parPCB){
		asm{
		mov currAX, ax
	    mov ax, [bp+02]
		mov childIP, ax
		mov ax, [bp+04]
		mov childCS,ax
		mov ax,currAX
	}
	stack = new unsigned int[stackSize];
	unsigned char * parentByteStack = (unsigned char*) parPCB->stack;
	unsigned char * myByteStack = (unsigned char*) stack;
	unsigned int byteStackSize = parPCB->stackSize*2;
	unsigned int numberOfLocationsToCopy;
	//unsigned int OffsetOfFirstLoc = FP_OFF(myByteStack + byteStackSize -1);
	unsigned int ParOffsetOfFirstLoc = FP_OFF(parentByteStack + byteStackSize -1);
	
	
	
	if(ParOffsetOfFirstLoc > parentSP){
		numberOfLocationsToCopy = ParOffsetOfFirstLoc + 1 - parentSP;
	}
	else{
		numberOfLocationsToCopy = byteStackSize + ParOffsetOfFirstLoc + 1 - parentSP;
	}
		for(int i=0;i<numberOfLocationsToCopy;i++){
		myByteStack[byteStackSize-1-i]=parentByteStack[byteStackSize-1-i];
	}
	
	unsigned int OffsetOfLastLoc = FP_OFF(myByteStack + byteStackSize - numberOfLocationsToCopy);
	unsigned int SegmentOfStack = FP_SEG(myByteStack + byteStackSize -  numberOfLocationsToCopy);
	unsigned int OffsetBPSP = parentBP - parentSP;
	childSS = SegmentOfStack;
	childSP = OffsetOfLastLoc;
	childBP = childSP + OffsetBPSP;
	bp = childBP;
	parLAST = FP_OFF( parPCB->stack + stackSize - 1);
	
	asm{
		mov currAX,ax
		mov currBP, bp
		mov currSS, ss
	}
	
	while(1){
		asm{
		    mov bp, parentBP
			mov ss, parentSS
			mov ax, [bp]
			mov iterBP, ax
		}
		cout<< "iterBP: " << iterBP << endl;
		if (iterBP == parLAST) break;
			
	           tempBP = iterBP - parentBP + childBP;
				 cout<< "tempBP: " << tempBP << endl;
			   asm{
				  mov ss, childSS
				  mov bp, childBP
				  mov ax, tempBP
				  mov [bp], ax
			   }
               parentBP=iterBP;
			   childBP=tempBP;
			  cout<< "parentBP: " << parentBP << endl;
              cout<< "childBP: " << childBP << endl;			  
		}
	
	
	
	 asm {
		 mov ax, currAX
		 mov bp, currBP
		 mov ss, currSS
	}
	
	asm{   
		        mov currSP,sp
				mov currSS,ss
				mov currBP,bp
				mov ss,childSS
				mov sp,childSP
				mov bp,childBP
				sub sp,0008
				pushf
				mov currAX,ax
				mov ax,childCS
				push ax
				mov ax,childIP
				push ax
			    mov ax,currAX
				
				push ax
				push bx
				push cx
				push dx
				push es
				push ds
				push si
				push di
				push bp
			
                mov childSP, sp
				mov childSS, ss
				mov sp, currSP
				mov ss, currSS
				mov bp, currBP
		    }

			sp = childSP;
			ss = childSS;
}



int PCB::getNumOfChildren() const { return numOfChildren;} 
Semaphore& PCB::getChildrenSemaphore() { return childrenSemaphore; }
PCB* PCB::getParent()const { return parent; }

StackSize PCB::getStackSize(){
	return stackSize;
}
	Time PCB::getTimeSlice(){
		return timeSlice;
	}