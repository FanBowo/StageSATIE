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

extern pthread_mutex_t MUTEX_FIFO_INFO_GPS;
extern sem_t SEM_FIFO_INFO_GPS;
extern pthread_mutex_t MUTEX_CSV_INFO_GPS;
void * SaveGPS_DataFunc(void *);

extern pthread_mutex_t MUTEX_CARTE_MEMOIRE;
extern pthread_mutex_t Write2TerminalMutex;

extern pthread_mutex_t bIMU_Data_StableMutex;
extern pthread_cond_t bIMU_Data_StableCond;

extern pthread_mutex_t bCSV_PointerPreparedMutex;
extern pthread_cond_t bCSV_PointerPreparedCond;

extern pthread_mutex_t MUTEX_FIFO_IMAGE_IMU;
extern pthread_mutex_t MUTEX_FIFO_BRUT_IMU;
extern pthread_mutex_t MUTEX_CSV_BRUT_IMU;
extern pthread_mutex_t MUTEX_CSV_POSE_IMU;

extern pthread_mutex_t MUTEX_HORODATAGE_COMPTEUR;
extern pthread_cond_t SIGNAL_RENOUVELER_HORODATAGE;
extern pthread_mutex_t MUTEX_HORODATAGE;
extern pthread_mutex_t RW_MUTEX_HORODATAGE;

extern timer_t Device_Timer;
extern long HORODATAGE_COMPTEUR;
extern struct itimerspec Device_Timer_trigger;
#define TimerDeviceFre 157
void InitTimerDevice();
void TimerDevice_Feedback(union sigval sv);
void * UpdateDeviceTimeStampFunc(void *);

extern pthread_mutex_t MUTEX_I2C;
extern pthread_mutex_t MUTEX_SIGNAL_RENOUVELER_BASE_HORODATAGE;
extern pthread_cond_t SIGNAL_RENOUVELER_BASE_HORODATAGE;
void * UpdateTimeStampBaseFunc(void *);

void * GPS_UpdateFIFOFunc(void *);
extern pthread_cond_t Update_GPS_FifoMutexCond;
extern pthread_mutex_t Update_GPS_FifoMutex;

extern pthread_cond_t SIGNAL_LIRE_IMU_BRUT;
extern pthread_mutex_t MUTEX_SIGNAL_LIRE_IMU_BRUT;
extern timer_t IMU_Timer;
extern sem_t SEM_FIFO_BRUT_IMU;
extern struct itimerspec IMU_Timer_trigger;
void InitTimerIMU();//IMU data frequncy 50hz
void TimerIMU_Feedback(union sigval sv);
void * IMU_UpdateRawDataFunc(void *);
void UpdateIMU_RawData();
void OpenCSVfile();
void * SaveIMU_RawDataFunc(void *);
//void * SaveIMU_RawDataToFifoFunc(void *);
//extern pthread_mutex_t SaveMUTEX_SIGNAL_LIRE_IMU_BRUT;
//extern pthread_cond_t SaveSIGNAL_LIRE_IMU_BRUT;

#define TimerIMUFre 80

/*cameras*/
void *SaveCamera_IMU_DataToFifoFunc(void *);
void * SaveCamera_IMU_DataFunc(void *);

extern sem_t SEM_FIFO_IMAGE_IMU;
extern pthread_mutex_t MUTEX_SIGNAL_REV_IMAGE;
extern pthread_cond_t SIGNAL_REV_IMAGE;

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
