
#include "CameraTimer.h"
#define ExternTriggerFre 20
#define Nanoseconds_0_5e10_9 500000000

timer_t timerid_EXTERN_TRIGGER;
int fd_GPIO_P2_c4;

void InitTimer(){
    printf("Start initialize timer\n");
    struct sigevent sev;
    struct itimerspec trigger;
    memset(&sev, 0, sizeof(struct sigevent));
    memset(&trigger, 0, sizeof(struct itimerspec));

    sev.sigev_notify=SIGEV_THREAD;
    sev.sigev_notify_function=&TriggerPWM;

    timer_create(CLOCK_REALTIME,&sev,&timerid_EXTERN_TRIGGER);

    trigger.it_interval.tv_sec=0;
    trigger.it_interval.tv_nsec=Nanoseconds_0_5e10_9/ExternTriggerFre;
    //trigger.it_interval.tv_nsec=0;
    trigger.it_value.tv_sec=0;
    trigger.it_value.tv_nsec=1;
    timer_settime(timerid_EXTERN_TRIGGER,0,&trigger,NULL);
    printf("Successfully initialize timer\n");
}


void TriggerPWM(union sigval sv){
static bool FlipFlag=true;
    if(!FlipFlag){
        write(fd_GPIO_P2_c4, SYSFS_GPIO_RST_VAL_H, sizeof(SYSFS_GPIO_RST_VAL_H));
        FlipFlag=!FlipFlag;
        //printf("H\n");
    }
    else{
        write(fd_GPIO_P2_c4, SYSFS_GPIO_RST_VAL_L, sizeof(SYSFS_GPIO_RST_VAL_L));
        FlipFlag=!FlipFlag;
        //printf("L\n");
    }
}


int InitGPIO(){

    //open gpio
    printf("Start initialize GPIO\n");
    fd_GPIO_P2_c4 = open(SYSFS_GPIO_EXPORT, O_WRONLY);
    if(fd_GPIO_P2_c4 == -1)
    {
              printf("ERR: Radio hard reset pin open error.\n");
              return EXIT_FAILURE;
    }
    write(fd_GPIO_P2_c4, SYSFS_GPIO_RST_PIN_VAL ,sizeof(SYSFS_GPIO_RST_PIN_VAL));
    close(fd_GPIO_P2_c4);

    //set direction
    fd_GPIO_P2_c4 = open(SYSFS_GPIO_RST_DIR, O_WRONLY);
    if(fd_GPIO_P2_c4 == -1)
    {
              printf("ERR: Radio hard reset pin direction open error.\n");
              return EXIT_FAILURE;
    }
    write(fd_GPIO_P2_c4, SYSFS_GPIO_RST_DIR_VAL, sizeof(SYSFS_GPIO_RST_DIR_VAL));
    close(fd_GPIO_P2_c4);

    //output reset sigal
    fd_GPIO_P2_c4 = open(SYSFS_GPIO_RST_VAL, O_RDWR);
    if(fd_GPIO_P2_c4 == -1)
    {
              printf("ERR: Radio hard reset pin value open error.\n");
              return EXIT_FAILURE;
    }
    printf("Successfully initialize GPIO\n");
    return 0;
}

void CloseTimerGPIO(){
    timer_delete(timerid_EXTERN_TRIGGER);
    close(fd_GPIO_P2_c4);
}
