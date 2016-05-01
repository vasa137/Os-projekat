#include "thread.h"
#include "Kernel.h"
#include "lock.h"
#include "Semaphore.h"
#include <iostream.h> 

extern volatile int contextChangeRequested;

void tick(){}

void c();

Semaphore s(0), z(0);

void a(){	
	for( int i = 0; i < 50; i++){
		_lock_; //
		cout << "u a() i = " << i << endl;
		_unlock_; //
		if(contextChangeRequested){
			dispatch();
		}
		if(i == 25) {
			c();
		}
	}
}



void b(){
	for( int i = 0; i < 50; i++){
		_lock_;
		cout << "u b() i = " << i << endl;
		_unlock_;
		if(contextChangeRequested){
			dispatch();
		}
	}
}

class ThreadIzv : public Thread{
void (*f) ();

public:
	ThreadIzv(void (*ff) () ) : f(ff){}
	virtual void run(){
		(*f)();
	}
};

ThreadIzv* nita = new ThreadIzv(&a);
ThreadIzv nitc(&a);

void c(){
	delete nita;
}

int userMain (int argc, char**argv){
	ThreadIzv* nitb = new ThreadIzv(&b);
	nita->start();
	nitb->start();

	
	for(int i = 0; i < 250; i++){
		_lock_;
		cout << "u main() i = " << i << endl;
		_unlock_;
		if(contextChangeRequested){
			dispatch();
		}
	}
	//nita->waitToComplete();
	nitb->waitToComplete();
	delete nitb;
	return 0;
}