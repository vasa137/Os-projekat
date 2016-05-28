#ifndef KERNEL_H_
#define KERNEL_H_

class PCB;
class IdleThread;
typedef void interrupt (*pInterrupt)(...);


class Kernel{
   public:
		static Kernel* getInstance();
		
        void startUp();
	    void terminate();
		void initUserMainThread(int,char**);
		static void interrupt newTimer(...);
		static IdleThread* getIdleThread();
		static PCB* getmainPCB();
		
   private:
        static Kernel* instance;
		
		pInterrupt oldRoutine;
		Kernel(){}
	    void initIVT();
		void restoreIVT();
		void initSystemThreads();
		void removeUserThreads();
		void removeSystemThreads();
		void waitForUserThreads();
		void Mainblock();
		void removeSemaphoreList();
		static IdleThread* idleThread;
		static PCB* mainPCB; 
};

#endif