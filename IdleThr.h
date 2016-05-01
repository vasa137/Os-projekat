#ifndef IDLETHREAD_H_
#define IDLETHREAD_H_

#include "thread.h"


class PCB;

class IdleThread : public Thread{
public:
	void start();
	virtual void run();
	PCB* getIdlePCB() const;
};

#endif