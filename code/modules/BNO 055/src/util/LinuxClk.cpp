#include "LinuxClk.h"

int Linux_get_clock_ms(int32_t *count)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	*count = (tv.tv_sec)*1000+(tv.tv_usec / 1000);
	return 0;
}

int Linux_delay_ms(unsigned long num_ms)
{
	//mdelay(num_ms);
	for(unsigned long i=0; i<num_ms;i++){
		usleep(1000);
	}
	return 0;
}
