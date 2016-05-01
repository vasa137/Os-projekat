#include <iostream.h>
#include "Kernel.h"

int returnValue = 0;

int main (int argc, char* argv[]){
	Kernel* kernel = Kernel::getInstance();
	kernel->startUp();
	kernel->initUserMainThread(argc, argv);
	kernel->terminate();
	
	return returnValue;
}