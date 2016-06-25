#ifndef QUEUE_H_
#define QUEUE_H_

class PCB;
class List;

class Queue{
public:
	Queue();
	~Queue();
	
	void insert_q(PCB*);
	
	PCB* delete_q();
	
	int isEmpty() const;
	
	PCB* remove(unsigned int id);
	
	PCB* peek() const;

private:
	List* queueList;
};

#endif