 #include <stdio.h>
 #include <unistd.h>
 #include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <assert.h>
 int main ()
 {
 int folder= open("/home/fanzhuzhi/Bureau/testPosix",O_RDONLY | O_NONBLOCK);
 fd_set readfd;
 int ret;
 if(folder<0){
    perror("Can't open directory");
    return EXIT_FAILURE;
 }
 struct timeval timeout;
FD_ZERO(&readfd);
FD_SET(folder,&readfd);
 while(1){
    timeout.tv_sec=50;
    timeout.tv_usec=0;

    ret=select(folder+1,&readfd,NULL,NULL,&timeout);
       //select error when ret = -1
       if (ret == -1)
           perror("select error");
       //data coming when ret>0
       else if (ret)
       {
//           if(FD_ISSET(keyboard,&readfd))
//           {
//               i=read(keyboard,&c,1);
//               if('\n'==c)
//                   continue;
//               printf("the input is %c\n",c);
//               if ('q'==c)
//               break;
//           }
        printf("ret value %d",ret);
        printf("contenent changed/n");
       }
       //time out when ret = 0
       else if (ret == 0)
           printf("time out/n");
 }

 }
// int ret,i;
// char c;
// fd_set readfd;
// struct timeval timeout;
// keyboard = open(".",O_RDONLY | O_NONBLOCK);
// assert(keyboard>0);
// while(1)
// {
//       timeout.tv_sec=5;
//       timeout.tv_usec=0;
//       FD_ZERO(&readfd);
//       FD_SET(keyboard,&readfd);
//       ret=select(keyboard+1,&readfd,NULL,NULL,&timeout);
//       //select error when ret = -1
//       if (ret == -1)
//           perror("select error");
//       //data coming when ret>0
//       else if (ret)
//       {
//           if(FD_ISSET(keyboard,&readfd))
//           {
//               i=read(keyboard,&c,1);
//               if('\n'==c)
//                   continue;
//               printf("the input is %c\n",c);
//               if ('q'==c)
//               break;
//           }
//       }
//       //time out when ret = 0
//       else if (ret == 0)
//           printf("time out/n");
// }
// }



// int main ()
// {
// int keyboard;
// int ret,i;
// char c;
// fd_set readfd;
// struct timeval timeout;
// keyboard = open("/dev/tty",O_RDONLY | O_NONBLOCK);
// assert(keyboard>0);
// while(1)
// {
//       timeout.tv_sec=5;
//       timeout.tv_usec=0;
//       FD_ZERO(&readfd);
//       FD_SET(keyboard,&readfd);
//       ret=select(keyboard+1,&readfd,NULL,NULL,&timeout);
//       //select error when ret = -1
//       if (ret == -1)
//           perror("select error");
//       //data coming when ret>0
//       else if (ret)
//       {
//           if(FD_ISSET(keyboard,&readfd))
//           {
//               i=read(keyboard,&c,1);
//               if('\n'==c)
//                   continue;
//               printf("the input is %c\n",c);
//               if ('q'==c)
//               break;
//           }
//       }
//       //time out when ret = 0
//       else if (ret == 0)
//           printf("time out/n");
// }
// }
//
