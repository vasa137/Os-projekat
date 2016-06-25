#include <iostream.h>
#include "Kernel.h"
#include "UserMain.h"

volatile int returnValue = 0;

int main (int argc, char* argv[]){
	Kernel* kernel = Kernel::getInstance();
	kernel->startUp();
	kernel->initUserMainThread(argc, argv);
	kernel->terminate();
	return returnValue;
}