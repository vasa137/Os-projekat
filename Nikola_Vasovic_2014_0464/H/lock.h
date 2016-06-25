#ifndef LOCK_H_
#define LOCK_H_

extern volatile unsigned int locked;

#define _lock_ locked=1;
#define _unlock_ locked=0;

#endif