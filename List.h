#ifndef LIST_H_
#define LIST_H_

#include "thread.h"
class PCB;

class List{
	struct Elem{
		PCB* pcb;
		Elem* next;
		Elem(PCB*); 
	};
	
	
	Elem* first,*last;
	int numberOfElements;
public:
    void add(PCB*);
	PCB* get(ID) const;
	PCB* remove(ID);
	PCB* getFirstPCB() const;
	Elem* getFirstElem() const;
	PCB* removeFirstPCB();
	void removeElem(Elem* prev,Elem* curr);
	int isEmpty() const;
	int getNumOfElements() const;
	int onlyNewThreads()const ;
	List();
	~List();
	friend class Queue;
};

#endif