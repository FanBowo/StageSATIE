
#include "CameraTimer.h"
#define ExternTriggerFre 25
#define Nanoseconds_0_5e10_9 1000000000
#define ExposureTime 5000000

timer_t timerid_EXTERN_TRIGGER1;//pull up
timer_t timerid_EXTERN_TRIGGER2;//push down
int fd_GPIO_P2_c4;

void InitTimer(){
    printf("Start initialize timer\n");
    struct sigevent sev1;//pull up
    struct itimerspec trigger1;//pull up
    memset(&sev1, 0, sizeof(struct sigevent));//pull up
    memset(&trigger1, 0, sizeof(struct itimerspec));//pull up

    sev1.sigev_notify=SIGEV_THREAD;//pull up
    sev1.sigev_notify_function=&TriggerPWM_pullup;//pull up

    timer_create(CLOCK_REALTIME,&sev1,&timerid_EXTERN_TRIGGER1);//pull up

    trigger1.it_interval.tv_sec=0;//pull up
    trigger1.it_interval.tv_nsec=Nanoseconds_0_5e10_9/ExternTriggerFre;//pull up
    //trigger.it_interval.tv_nsec=0;
    trigger1.it_value.tv_sec=0;//pull up
    trigger1.it_value.tv_nsec=1;//pull up

    struct sigevent sev2;//push down
    struct itimerspec trigger2;//push down
    memset(&sev2, 0, sizeof(struct sigevent));//push down
    memset(&trigger2, 0, sizeof(struct itimerspec));//push down

    sev2.sigev_notify=SIGEV_THREAD;//push down
    sev2.sigev_notify_function=&TriggerPWM_pushdown;//push down

    timer_create(CLOCK_REALTIME,&sev2,&timerid_EXTERN_TRIGGER2);//push down

    trigger2.it_interval.tv_sec=0;//push down
    trigger2.it_interval.tv_nsec=Nanoseconds_0_5e10_9/ExternTriggerFre;//push down
    //trigger.it_interval.tv_nsec=0;
    trigger2.it_value.tv_sec=0;//push down
    trigger2.it_value.tv_nsec=1+ExposureTime;//push down

    timer_settime(timerid_EXTERN_TRIGGER1,0,&trigger1,NULL);
    timer_settime(timerid_EXTERN_TRIGGER2,0,&trigger2,NULL);

    printf("Successfully initialize timer\n");
}


void TriggerPWM_pullup(union sigval sv){
    write(fd_GPIO_P2_c4, SYSFS_GPIO_RST_VAL_H, sizeof(SYSFS_GPIO_RST_VAL_H));
//printf("H\n");
//static bool FlipFlag=true;
//    if(!FlipFlag){
//        write(fd_GPIO_P2_c4, SYSFS_GPIO_RST_VAL_H, sizeof(SYSFS_GPIO_RST_VAL_H));
//        FlipFlag=!FlipFlag;
//        //printf("H\n");
//    }
//    else{
//        write(fd_GPIO_P2_c4, SYSFS_GPIO_RST_VAL_L, sizeof(SYSFS_GPIO_RST_VAL_L));
//        FlipFlag=!FlipFlag;
//        //printf("L\n");
//    }
}

void TriggerPWM_pushdown(union sigval sv){
    write(fd_GPIO_P2_c4, SYSFS_GPIO_RST_VAL_L, sizeof(SYSFS_GPIO_RST_VAL_L));
    //printf("L\n");
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
    timer_delete(timerid_EXTERN_TRIGGER1);
    close(fd_GPIO_P2_c4);
}
