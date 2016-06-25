#ifndef IDLETHREAD_H_
#define IDLETHREAD_H_

class PCB;

class IdleThread : public Thread{
public:
	IdleThread(); 
	~IdleThread();
	void start();
	virtual void run();
	
	//getter
	PCB* getIdlePCB() const;
};

#endif