#ifndef KERNEL_H_
#define KERNEL_H_

class PCB;
class IdleThread;

typedef void interrupt (*pInterrupt)(...);

class Kernel{
   public:
		static Kernel* getInstance();
	
		static void interrupt newTimer(...); //interrupt routine
	
        void startUp();
		void initUserMainThread(int,char**);
	    void terminate();
		
		/*static fields getters*/
		static IdleThread* getIdleThread();
		static PCB* getmainPCB();
		
   private:
        static Kernel* instance;
		
		/*initialization*/
	    void initIVT();
		void initSystemThreads();
		
		/*restauration*/
		void restoreIVT();
		void removeUserThreads();
		void removeSystemThreads();
		void removeWaitList();
		
		/*Main wait*/
		void waitForUserThreads();

		/*static fields*/
		static IdleThread* idleThread;
		static PCB* mainPCB; 
		
		pInterrupt oldRoutine; // pointer to and old routine from entry 08h
};

#endif