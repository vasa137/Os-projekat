#ifndef SORTED_H_
#define SORTED_H_

class PCB;
class List;

class SortedList{
public:
	SortedList();
	
	~SortedList();
	
	void insert(PCB* pcb);
	
	int isEmpty() const;
	
	void remove(PCB* pcb);
	
	void decTime();
	
	PCB* getFirstPCB() const;
	
private:
	List* sortedList;
};

#endif