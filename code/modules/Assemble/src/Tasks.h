#ifndef TASKS_H_INCLUDED
#define TASKS_H_INCLUDED
#include "Assemble.h"
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>
#include <iomanip>
#include <sched.h>

#define EnableParseOutput true
#define GPSECHO false
#define Nano10_9 1000000000
extern Assemble AssembleDevice;


extern pthread_mutex_t TimeStampBaseMutex;
extern pthread_cond_t TimeStampBaseCond;
void * UpdateTimeStampBaseFunc(void *);

extern pthread_mutex_t IMU_TimerCounterMutex;
extern pthread_cond_t IMU_TimeStampCond;
extern pthread_mutex_t IMU_TimeStampMutex;
extern timer_t IMU_Timer;
extern sem_t IMU_RawDataFifoSem;
extern struct itimerspec IMU_Timer_trigger;
extern int IMU_TimerCounter;
void InitTimerIMU();//IMU data frequncy 50hz
void TimerIMU_Feedback(union sigval sv);
void * IMU_UpdateTimeStampFunc(void *);
void UpdateIMU_RawData();
void * SaveIMU_RawDataFunc(void *);
#define TimerIMUFre 50

void signal_handler_IO (int status);   /* definition of signal handler,
                                        used to process serial port interrupt */
void init_SerialPortInt();

#endif // TASKS_H_INCLUDED
