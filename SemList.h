#ifndef SEMLIST_H_
#define SEMLIST_H_

class KernelSem;

class SemList{
	struct Elem{
		KernelSem* kernelSem;
		Elem* next;
		Elem(KernelSem* ksem);
	};
	Elem* first,*last;
public:
	SemList();
	void add(KernelSem*);
	void remove(KernelSem*);
	void updateSemaphorsTime();
	~SemList();
};


#endif