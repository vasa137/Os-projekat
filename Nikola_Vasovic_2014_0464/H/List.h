#ifndef LIST_H_
#define LIST_H_
#include<stdlib.h>

class PCB;

class List{
	
	struct Elem{
		PCB* pcb;
		Elem* next;
		Elem(PCB*,Elem* nxt = NULL); 
	};
	
	Elem* first,*last;
	int number;
public:
	List();
	~List();
   
    void add(PCB*);
	void sort_insert(PCB*);
	
	PCB* getFirstPCB() const;
	PCB* get(unsigned int id) const;
	
	PCB* remove(unsigned int id);
	PCB* removeFirstPCB();
	void removePCB(PCB*);
	
	void decTime();
	void removeParent(PCB*);
	int isEmpty() const;
	int getNum() const;
};

#endif