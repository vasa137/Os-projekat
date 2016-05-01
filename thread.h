#ifndef _THREAD_H_
#define _THREAD_H_
  
typedef unsigned long StackSize; 
const StackSize minStackSize = 1024; // 2 KB
const StackSize maxStackSize = 32768; // 64 KB
typedef unsigned int Time; // time x 55ms
const Time defaultTimeSlice = 1; // 110 ms
typedef int ID;

class PCB; // Kernel's implementation of a user's thread  

class Thread {
	
public:  
  void start(); 
  void waitToComplete(); 
  virtual ~Thread();  
  ID getId() const; 
  static ID getRunningId();  
  static Thread * getThreadById(ID id);  
  static ID fork();   
  static void exit(); 
  static void waitForForkChildren();  
  virtual Thread* clone() const; 
protected: 
  friend class PCB; 
  friend class IdleThread;
  friend class Kernel;
  Thread (StackSize stackSize = minStackSize, Time timeSlice = defaultTimeSlice);  
  virtual void run() {}  

private: 
  PCB* myPCB; 
  
  Thread(const Thread&); /*problem without const*/

}; 

void dispatch (); 

#endif