#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <errno.h>
#include <termios.h>
using namespace std;

void signal_handler_IO (int status);   /* definition of signal handler */

int n;
int fd;
int connected;
struct termios termAttr;
struct sigaction saio;

int main(int argc, char *argv[])
{
    fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        perror("open_port: Unable to open /dev/ttyO1\n");
        exit(1);
    }

    saio.sa_handler = signal_handler_IO;
    saio.sa_flags = 0;
    saio.sa_restorer = NULL;
    sigaction(SIGIO,&saio,NULL);

    fcntl(fd, F_SETFL, FNDELAY|FASYNC);
    fcntl(fd, F_SETOWN, getpid());

    tcgetattr(fd,&termAttr);
    //baudRate = B115200;
    cfsetispeed(&termAttr,B115200);// input BaudRate
    cfsetospeed(&termAttr,B115200);//output BaudRate
	/* c_cflag: control mode flags */
    termAttr.c_cflag &= ~PARENB; //no parity checking 
    termAttr.c_cflag &= ~CSTOPB;//1 stop bit
    termAttr.c_cflag &= ~CSIZE;//clear current char size mask
    termAttr.c_cflag |= CS8;//no output processing, force 8 bit input
    termAttr.c_cflag |= (CLOCAL | CREAD);
	//-clocal: Disable modem control signals (no hardware flow control)
	
	/*c_lflag : local mode flags */
    termAttr.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); 
	
	/* canonical mode:
	Everything is stored into a buffer and can be edited until a carriage return or line feed is entered. 
	After the carriage return or line feed is pressed, the buffer is sent. 
	*/
	
	// canonical mode off,echo off, *,  signal chars off
	/*c_iflag : input mode flags */
    termAttr.c_iflag &= ~(IXON | IXOFF | IXANY);
	// no XON/XOFF software flow control *
	/* c_oflag : output mode flags */
    termAttr.c_oflag &= ~OPOST; // no local output processing
    tcsetattr(fd,TCSANOW,&termAttr);
	//the configuration is changed immediately
    printf("UART1 configured....\n");

    connected = 1;
    while(connected == 1){
               // some code
    }

    close(fd);
    exit(0);
}


void signal_handler_IO (int status)
{
    printf("received data from UART.\n");
}
