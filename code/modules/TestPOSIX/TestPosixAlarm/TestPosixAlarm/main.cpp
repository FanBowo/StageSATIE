#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

volatile sig_atomic_t last_received_signal=0;

//           siginfo_t {
//               int      si_signo;     /* Signal number */
//               int      si_errno;     /* An errno value */
//               int      si_code;      /* Signal code */
//               int      si_trapno;    /* Trap number that caused
//                                         hardware-generated signal
//                                         (unused on most architectures) */
//               pid_t    si_pid;       /* Sending process ID */
//               uid_t    si_uid;       /* Real user ID of sending process */
//               int      si_status;    /* Exit value or signal */
//               clock_t  si_utime;     /* User time consumed */
//               clock_t  si_stime;     /* System time consumed */
//               sigval_t si_value;     /* Signal value */
//               int      si_int;       /* POSIX.1b signal */
//               void    *si_ptr;       /* POSIX.1b signal */
//               int      si_overrun;   /* Timer overrun count;
//                                         POSIX.1b timers */
//               int      si_timerid;   /* Timer ID; POSIX.1b timers */
//               void    *si_addr;      /* Memory location which caused fault */
//               long     si_band;      /* Band event (was int in
//                                         glibc 2.3.2 and earlier) */
//               int      si_fd;        /* File descriptor */
//               short    si_addr_lsb;  /* Least significant bit of address
//                                         (since Linux 2.6.32) */
//               void    *si_lower;     /* Lower bound when address violation
//                                         occurred (since Linux 3.19) */
//               void    *si_upper;     /* Upper bound when address violation
//                                         occurred (since Linux 3.19) */
//               int      si_pkey;      /* Protection key on PTE that caused
//                                         fault (since Linux 4.6) */
//               void    *si_call_addr; /* Address of system call instruction
//                                         (since Linux 3.5) */
//               int      si_syscall;   /* Number of attempted system call
//                                         (since Linux 3.5) */
//               unsigned int si_arch;  /* Architecture of attempted system call
//                                         (since Linux 3.5) */
//           }

void signal_catcher(int signo, siginfo_t * info, void *context){
    last_received_signal=info->si_signo;
    if(signo==SIGUSR1){
        printf("DIGUSER1 received, num signal %d\n",signo);
        //printf("Context Value %d \n",long(& context));
    }
    if(signo==SIGUSR2){
        printf("DIGUSER2 received, num signal %d\n",signo);
        //printf("Context Value %d \n",long(& context));
    }
}

int main(){
    //Setup a signal handler for SIGUSER1 and SIGUSER2
    struct sigaction act;
    /*
        struct sigaction {
        void     (*sa_handler) (int);
        void     (*sa_sigaction) (int, siginfo_t *, void *);
        sigset_t   sa_mask;
        int        sa_flags;
        void     (*sa_restorer) (void);
    };
    //sa_handler specifies the action to be associated with signum and may
       be SIG_DFL for the default action, SIG_IGN to ignore this signal, or
       a pointer to a signal handling function.  This function receives the
       signal number as its only argument
    */
    memset(&act,0,sizeof act);

    //sigact structre holding old configuration, will be filled by sigaction
    struct sigaction old_1;
    memset(&old_1,0,sizeof old_1);
    struct sigaction old_2;
    memset(&old_2,0,sizeof old_2);

    act.sa_sigaction=signal_catcher;

    act.sa_flags=SA_SIGINFO;
    /*
    int sigaction(int signum, const struct sigaction *act,
                     struct sigaction *oldact);
    //signum specifies the signal and can be any valid signal except
       SIGKILL and SIGSTOP.
    // If act is non-NULL, the new action for signal signum is installed
       from act.  If oldact is non-NULL, the previous action is saved in
       oldact.
    */
    if(0!=sigaction(SIGUSR1,&act,&old_1)){
        perror("sigaction () failed installing SIGUSER1 handler");
        return EXIT_FAILURE;
    }

    if(0!=sigaction(SIGUSR2,&act,&old_2)){
        perror("sigaction () failed installing SIGUSER2 handler");
        return EXIT_FAILURE;
    }

    for(int i=1;i<=60;i++){
        if(!(i%5)){
            if(0!=raise(SIGUSR1)){
                perror("Can't raise SIGUSR1");
                return EXIT_FAILURE;
            }
        }

        if(!(i%10)){
            if(0!=raise(SIGUSR2)){
                perror("Can't raise SIGUSR2");
                return EXIT_FAILURE;
            }
        }

        printf("Tick #%d,last caught signal: %d \n", i, last_received_signal);
        sleep(1);
    }

    if(0!=sigaction(SIGUSR1,&old_1,NULL)){
        perror("sigaction() failed restoring SIGUSER1 handler");
        return EXIT_FAILURE;
    }

    if(0!=sigaction(SIGUSR2,&old_2,NULL)){
        perror("sigaction() failed restoring SIGUSER2 handler");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
