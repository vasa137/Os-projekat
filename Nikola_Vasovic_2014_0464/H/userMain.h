#ifndef USERMAIN_H_
#define USERMAIN_H_

#include "thread.h"

int userMain(int,char*[]);
extern volatile int returnValue;

class UserMainThread : public Thread{
	int argc;
	char** argv;
public:
	UserMainThread::UserMainThread(int carg, char** varg) : argc(carg), argv(varg) {}

	virtual void UserMainThread::run(){
		returnValue = userMain(argc,argv); 
	}

};

#endif