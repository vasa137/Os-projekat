#ifndef USERMAIN_H_
#define USERMAIN_H_

int userMain(int,char*[]);

class UserMainThread : public Thread{
	int argc;
	char** argv;
public:
	UserMainThread(int carg, char** varg);
	virtual void run();
};

#endif