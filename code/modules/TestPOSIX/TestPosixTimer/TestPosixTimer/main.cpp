#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

void thread_handler(union sigval sv){
    char *s=(char *)sv.sival_ptr;
    puts(s);
}

void OneSecondeTimer(union sigval sv){
    static int s=0;

    //int s=(int)(&(sv.sival_ptr));
    printf("Time now:%d\n",s);
    s++;
}
int main(){
    char info[]="5 secobds elapsed";
    timer_t timerid;
    timer_t timerid1;
    struct sigevent sev;
    struct sigevent sev1;
    struct itimerspec trigger;
    struct itimerspec trigger1;

    memset(&sev,0,sizeof(struct sigevent));
    memset(&trigger,0,sizeof(struct itimerspec));
    memset(&sev1,0,sizeof(struct sigevent));
    memset(&trigger1,0,sizeof(struct itimerspec));

    sev.sigev_notify=SIGEV_THREAD;
    sev1.sigev_notify=SIGEV_THREAD;
//    SIGEV_THREAD:
//              Upon timer expiration, invoke sigev_notify_function as if it
//              were the start function of a new thread
//    SIGEV_SIGNAL
//                  Upon timer expiration, generate the signal sigev_signo for the
//                  process.  See sigevent(7) for general details
    sev.sigev_notify_function=&thread_handler;
    sev.sigev_value.sival_ptr=&info;

    sev1.sigev_notify_function=&OneSecondeTimer;

    timer_create(CLOCK_REALTIME,&sev,&timerid);
    timer_create(CLOCK_REALTIME,&sev1,&timerid1);
//    int timer_create(clockid_t clockid, struct sigevent *sevp,
//                        timer_t *timerid);
    trigger.it_value.tv_sec=5;
    trigger1.it_interval.tv_sec=1;
    trigger1.it_value.tv_sec=0;
    trigger1.it_value.tv_nsec=1;
    timer_settime(timerid,0,&trigger,NULL);
    timer_settime(timerid1,0,&trigger1,NULL);
//    int timer_settime(timer_t timerid, int flags,
//                        const struct itimerspec *new_value,
//                        struct itimerspec *old_value);

    sleep(10);

    timer_delete(timerid);
    timer_delete(timerid1);
    return EXIT_SUCCESS;
}
