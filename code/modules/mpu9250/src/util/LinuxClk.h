#ifndef _LINUXCLK_H_
#define _LINUXCLK_H_
#include <unistd.h>
#include <sys/time.h>

#if defined(__cplusplus)
extern "C" {
#endif

    int Linux_get_clock_ms(unsigned long *count);

    int Linux_delay_ms(unsigned long num_ms);
#if defined(__cplusplus)
}
#endif

#endif // _LINUXCLK_H_
