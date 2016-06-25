#include "PCB.h"
#include "SCHEDULE.h"
#include "Kernel.h"
#include "lock.h"
#include "IdleThr.h"
#include "List.h"
#include <dos.h>
#include <iostream.h>

/*stackSize limits*/
const StackSize minStackSize = 256; 
const StackSize maxStackSize = 65536; // 64KB

/*global variables for fork*/
unsigned int parentSP, parentBP, parentSS;
unsigned int childSP, childBP, childSS;
unsigned int currBP, currSP, currSS, currAX;
unsigned int parLAST, iterBP, tempBP;
unsigned int childCS, childIP;

/*static fields*/
ID PCB::threadNum = 0; 
List* PCB::allThreadList = new List(); // list of all PCBs
PCB* PCB::running = NULL;
volatile int PCB::numberOfActiveThreads = 0;


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
	
	stack[stackSize - 13] = FP_OFF(stack + stackSize - 1); // for fork, bp points to last position of stack
	
	
	/*stack pointer and stack segment initialization*/
	ss = FP_SEG(stack + stackSize - 2 - 11); 
	sp = FP_OFF(stack + stackSize - 2 - 11);
	
	/*base pointer initialization*/
	bp = sp;
}

PCB::PCB(Thread* thr, StackSize ssize, Time tslice) : threadId(threadNum++), thread(thr), stackSize(ssize), timeSlice(tslice), timeLeft(tslice), state(NEW),
  stack(NULL), ss(0), sp(0), bp(0), blockTimeLeft(0), unblockedBySignal(0), blockSem(NULL),
   parent(NULL), numOfChildren(0), childrenSemaphore(0) {
	
	/*don't put main and idle*/
	if(threadId > 1 ) {
		pendingList = new List();
		allThreadList->add(this);
	}
}

PCB::~PCB() {
	delete pendingList;
	delete [] stack;
	if(thread != NULL)thread->myPCB = NULL; //important
	if(this->numOfChildren > 0){
		allThreadList->removeParent(this);
	}
	allThreadList->remove(threadId);
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
	numberOfActiveThreads--;

	running->freePendingList();
	if(running->parent != NULL) running->parent->childrenSemaphore.signal();
	_unlock_
	dispatch();
}



// Fork methods

void PCB::remakeChildPCB(PCB* myparent){
	parent=myparent;
	asm{
		mov parentBP,bp
		mov parentSS,ss
		mov parentSP,sp
	}
	copyStack(myparent);
}

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
	
	
	unsigned int SegmentOfStack = FP_SEG(myByteStack + byteStackSize -  numberOfLocationsToCopy);
	childSS = SegmentOfStack;
	unsigned int OffsetOfLastLoc = FP_OFF(myByteStack + byteStackSize - numberOfLocationsToCopy);
	childSP = OffsetOfLastLoc;
	unsigned int OffsetBPSP = parentBP - parentSP;
	childBP = childSP + OffsetBPSP;
	this->bp = childBP;
	this->ss = childSS;
	
	parLAST = FP_OFF( parPCB->stack + stackSize - 1);
	
	currSS = _SS;
	currBP = _BP;
	currAX = _AX;
	
	for(;;){
		_SS = parentSS;
		_BP = parentBP;
		asm mov ax, [bp]
			
		iterBP = _AX;
			
		
		if( parLAST == iterBP ){
			_AX = currAX;
			_BP = currBP;
			_SS = currSS;
			currSP = _SP;
			_SP = childSP;
			_BP = childBP;
			_SS = childSS;
			break;
		} 
			
	    tempBP = childBP - (parentBP - iterBP);
	   
	    _SS = childSS;
	    _BP = childBP;
		_AX = tempBP;
		
		asm  mov [bp], ax
	   
	    parentBP=iterBP;
	    childBP=tempBP;		  
	}
	
	 
	
	asm{   
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
	}
	
	_SP = currSP;
	_SS = currSS;
	_BP = currBP;
	
	this->sp = childSP;
}


/*static list getter*/

List* PCB::getThreadList(){ return allThreadList; }


/* non-static getters*/

//regular PCB getters

State PCB::getState() const { return state; }

Thread* PCB::getThread() const { return thread; }

StackSize PCB::getStackSize() const { return stackSize; }

Time PCB::getTimeSlice() const { return timeSlice; }

ID PCB::getId() const { return threadId; }

List* PCB::getPendingList() const { return pendingList; }

//Semaphore getters

KernelSem* PCB::getBlockSem() const { return blockSem;}

Time PCB::getBlockTime() const { return blockTimeLeft;}

//Fork getters

int PCB::getNumOfChildren() const { return numOfChildren;} 

Semaphore& PCB::getChildrenSemaphore() { return childrenSemaphore; }

PCB* PCB::getParent() const { return parent; }



/*setters*/

//regular PCB setters

void PCB::setThread(Thread* thr){ thread = thr; }

void PCB::setState(State state){ this->state = state; }

//Semaphore setters
void PCB::setBlockSem(KernelSem* s) { blockSem = s;}

void PCB::setUnblockedBySignal(int signal) { unblockedBySignal = signal; }

//Fork setters

void PCB::setNumOfChildren(int n){ numOfChildren = n; }

void PCB::incNumOfChildren() { numOfChildren++;}

void PCB::setParent(PCB* par){ parent = par; }
