#include <iostream>

//using namespace std;
//
//int main()
//{
//    cout << "Hello world!" << endl;
//    return 0;
//}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> //define O_WRONLY and O_RDONLY

//芯片复位引脚: P2_c4 p2:base1064 c4:20
#define SYSFS_GPIO_EXPORT           "/sys/class/gpio/export"
#define SYSFS_GPIO_RST_PIN_VAL      "1084"
#define SYSFS_GPIO_RST_DIR          "/sys/class/gpio/gpio1084/direction"
#define SYSFS_GPIO_RST_DIR_VAL      "OUT"
#define SYSFS_GPIO_RST_VAL          "/sys/class/gpio/gpio1084/value"
#define SYSFS_GPIO_RST_VAL_H        "1"
#define SYSFS_GPIO_RST_VAL_L        "0"

int main()
{
    int fd;

         //open gpio
         fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);
         if(fd == -1)
         {
                   printf("ERR: Radio hard reset pin open error.\n");
                   return EXIT_FAILURE;
         }
         write(fd, SYSFS_GPIO_RST_PIN_VAL ,sizeof(SYSFS_GPIO_RST_PIN_VAL));
         close(fd);

         //set direction
         fd = open(SYSFS_GPIO_RST_DIR, O_WRONLY);
         if(fd == -1)
         {
                   printf("ERR: Radio hard reset pin direction open error.\n");
                   return EXIT_FAILURE;
         }
         write(fd, SYSFS_GPIO_RST_DIR_VAL, sizeof(SYSFS_GPIO_RST_DIR_VAL));
         close(fd);

         //output reset sigal
         fd = open(SYSFS_GPIO_RST_VAL, O_RDWR);
         if(fd == -1)
         {
                   printf("ERR: Radio hard reset pin value open error.\n");
                   return EXIT_FAILURE;
         }
         while(1)
         {
                   write(fd, SYSFS_GPIO_RST_VAL_H, sizeof(SYSFS_GPIO_RST_VAL_H));
                   usleep(50000);
                   write(fd, SYSFS_GPIO_RST_VAL_L, sizeof(SYSFS_GPIO_RST_VAL_L));
                   usleep(50000);
         }
         close(fd);

         printf("INFO: Radio hard reset pin value open error.\n");
         return 0;

}
