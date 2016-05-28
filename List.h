#ifndef LIST_H_
#define LIST_H_
#include<stdlib.h>
class PCB;


class List{
	
	struct Elem{
		PCB* pcb;
		Elem* next;
		Elem(PCB*); 
	};
	
	Elem* first,*last;
	int number;
public:
   
    void add(PCB*);
	void sort_insert(PCB*);
	
	PCB* getFirstPCB() const;
	PCB* get(ID) const;
	
	PCB* remove(ID);
	PCB* removeFirstPCB();
	void remove_fromTimerList(PCB*);
	void delete_all_new();
	
	void decTime();
	
	int isEmpty() const;
	int getNum() const;
    int all_new() const;
	
	List();
	~List();
};

#endif