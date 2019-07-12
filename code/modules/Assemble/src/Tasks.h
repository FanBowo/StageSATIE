#ifndef TASKS_H_INCLUDED
#define TASKS_H_INCLUDED
#include "Assemble.h"
#include <pthread.h>

#define EnableParseOutput true


extern Assemble AssembleDevice;

extern pthread_mutex_t TimeStampBaseMutex;
extern pthread_cond_t TimeStampBaseCond;

void * UpdateTimeStampBaseFunc(void *);

#endif // TASKS_H_INCLUDED
