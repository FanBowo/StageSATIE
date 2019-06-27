#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

using namespace std;
int main(){
    cout << "Hello world!" << endl;//终端显示
    int fd = open("a.txt", O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (fd < 0)
    {
        printf("open a.txt failed, fd = %d\n", fd);
        return -1;
    }
    printf("open a.txt success, fd = %d\n", fd);//终端显示
    fflush(stdout);
    // 复制标准输出的文件描述符
    int nfd = dup(STDOUT_FILENO);//STDOUT_FILENO：Standard output
    int fileFd = dup2(fd, STDOUT_FILENO);
    if (fileFd < 0)
    {
        printf("dup2 stdout_fileno failed, fileFd = %d\n", fileFd);
        return -1;
    }
    printf("重定向标准输出到a.txt, fileFd = %d\n", fileFd);//写到文件
    // 将STDOUT_FILENO复制到fd后，fd并没有发生变化，依然可以通过它写入到a.txt中

    const char *pStr = "write string through fd\n";
    write(fd, pStr, strlen(pStr));//写到文件
    fileFd = dup2(nfd, fileFd);
    if (fileFd < 0)
    {
        printf("dup2 reback stdout_fileno failed\n");
        return -1;
    }
    printf("print back to stdout standard, fileFd = %d\n", fileFd);//终端显示
    // 上面使用dup2的时候，STDOUT_FILENO是存在并打开的，我们来测试下不存在的文件描述符的情况
    fileFd = dup2(fd, 20);
    if (fileFd < 0)
    {
        printf("dup2不存在的文件描述符失败, fileFd = %d\n", fileFd);
    }
    else
    {
        printf("dup2不存在的文件描述符成功, fileFd = %d\n", fileFd);//终端显示
        const char *pStr = "write string through fileFd\n";
        write(fileFd, pStr, strlen(pStr));//写到文件
    }
    close(nfd);
    close(fileFd);
    return 0;
}
