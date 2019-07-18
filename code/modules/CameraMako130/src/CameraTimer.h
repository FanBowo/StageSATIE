#ifndef CAMERATIMER_H_INCLUDED
#define CAMERATIMER_H_INCLUDED

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h> //define O_WRONLY and O_RDONLY
//#include <linux/gpio.h>

//芯片复位引脚: P2_c4 p2:base1064 c4:20 SDO1
#define SYSFS_GPIO_EXPORT           "/sys/class/gpio/export"
#define SYSFS_GPIO_RST_PIN_VAL      "1084"
#define SYSFS_GPIO_RST_DIR          "/sys/class/gpio/gpio1084/direction"
#define SYSFS_GPIO_RST_DIR_VAL      "OUT"
#define SYSFS_GPIO_RST_VAL          "/sys/class/gpio/gpio1084/value"
#define SYSFS_GPIO_RST_VAL_H        "1"
#define SYSFS_GPIO_RST_VAL_L        "0"

//extern timer_t timerid_EXTERN_TRIGGER;
//extern int fd_GPIO_P2_c4;

void InitTimer();

void TriggerPWM_pullup(union sigval sv);
void TriggerPWM_pushdown(union sigval sv);

void CloseTimerGPIO();
int InitGPIO();
#endif // CAMERATIMER_H_INCLUDED
