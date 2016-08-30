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
#include <fcntl.h> //define O_WRONLY and O_RDONLY
#include <errno.h>
#include <turbojpeg.h>
//#include <gmpxx.h>

#define EnableParseOutput true
#define GPSECHO false
#define Nano10_9 1000000000
extern Assemble AssembleDevice;

extern pthread_mutex_t GPS_DataFifoMutex;
extern sem_t GPS_DataFifoSem;
extern pthread_mutex_t pSaveGPS_DataMutex;
void * SaveGPS_DataFunc(void *);

extern pthread_mutex_t Write2EmmcMutex;
extern pthread_mutex_t Write2TerminalMutex;

extern pthread_mutex_t bIMU_Data_StableMutex;
extern pthread_cond_t bIMU_Data_StableCond;

extern pthread_mutex_t bCSV_PointerPreparedMutex;
extern pthread_cond_t bCSV_PointerPreparedCond;

extern pthread_mutex_t Camera_IMU_DataFifoMutex;
extern pthread_mutex_t IMU_RawDataFifoMutex;
extern pthread_mutex_t pSaveRawIMU_DataMutex;
extern pthread_mutex_t pSaveCamera_IMU_DataMutex;

extern pthread_mutex_t Device_TimerCounterMutex;
extern pthread_cond_t Device_TimeStampCond;
extern pthread_mutex_t Device_TimeStampMutex;
extern pthread_mutex_t RW_Device_TimeStampMutex;

extern timer_t Device_Timer;
extern long Device_TimerCounter;
extern struct itimerspec Device_Timer_trigger;
#define TimerDeviceFre 157
void InitTimerDevice();
void TimerDevice_Feedback(union sigval sv);
void * UpdateDeviceTimeStampFunc(void *);

extern pthread_mutex_t ReadIMU_Mutex;
extern pthread_mutex_t TimeStampBaseMutex;
extern pthread_cond_t TimeStampBaseCond;
void * UpdateTimeStampBaseFunc(void *);

//extern pthread_mutex_t IMU_TimerCounterMutex;
extern pthread_cond_t IMU_RawDataCond;
extern pthread_mutex_t IMU_RawDataMutex;
extern timer_t IMU_Timer;
extern sem_t IMU_RawDataFifoSem;
extern struct itimerspec IMU_Timer_trigger;
extern int IMU_TimerCounter;
void InitTimerIMU();//IMU data frequncy 50hz
void TimerIMU_Feedback(union sigval sv);
void * IMU_UpdateRawDataFunc(void *);
void UpdateIMU_RawData();
void OpenCSVfile();
void * SaveIMU_RawDataFunc(void *);
//void * SaveIMU_RawDataToFifoFunc(void *);
//extern pthread_mutex_t SaveIMU_RawDataMutex;
//extern pthread_cond_t SaveIMU_RawDataCond;

#define TimerIMUFre 30

/*cameras*/
void *SaveCamera_IMU_DataToFifoFunc(void *);
void * SaveCamera_IMU_DataFunc(void *);
//extern pthread_mutex_t Camera_TimerCounterMutex;
//extern pthread_cond_t Camera_TimeStampCond;
//extern pthread_mutex_t Camera_TimeStampMutex;
//extern int Camera_TimerCounter;
extern sem_t Camera_IMUDataFifoSem;

//extern sem_t PhotoSem;
//extern sem_t PhotoPositionSem;
extern pthread_mutex_t OnlySaveCamera_IMU_DataPthreadMutex;
extern pthread_mutex_t SaveCamera_IMU_DataMutex;
extern pthread_cond_t SaveCamera_IMU_DataCond;

extern pthread_mutex_t pNewFrameMutex;
extern FramePtr pNewFrame;

extern timer_t timerid_EXTERN_TRIGGER1;//Camera trigger pull up
extern timer_t timerid_EXTERN_TRIGGER2;//Camera trigger push down
extern int fd_GPIO_P2_c4;//GPIO file descriptor

//芯片复位引脚: P2_c4 p2:base1064 c4:20 SDO1
#define SYSFS_GPIO_EXPORT           "/sys/class/gpio/export"
#define SYSFS_GPIO_RST_PIN_VAL      "1084"
#define SYSFS_GPIO_RST_DIR          "/sys/class/gpio/gpio1084/direction"
#define SYSFS_GPIO_RST_DIR_VAL      "OUT"
#define SYSFS_GPIO_RST_VAL          "/sys/class/gpio/gpio1084/value"
#define SYSFS_GPIO_RST_VAL_H        "1"
#define SYSFS_GPIO_RST_VAL_L        "0"

//#define ExternTriggerFre 10
#define ExposureTime 50000000

//extern timer_t timerid_EXTERN_TRIGGER;
//extern int fd_GPIO_P2_c4;


//void InitCameraTimer();
//void TriggerPWM_pullup(union sigval sv);
//void TriggerPWM_pushdown(union sigval sv);

//void CloseTimerGPIO();
//int InitCameraTriggerGPIO();

/**/

void signal_handler_IO (int status);   /* definition of signal handler,
                                        used to process serial port interrupt */
void init_SerialPortInt();

#endif // TASKS_H_INCLUDED
