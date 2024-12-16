#include<stdio.h>
#include <fcntl.h>
#include<sys/syscall.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#define BUFF_SIZE 4096
int main()
{
    int fds;
    char buf1[BUFF_SIZE];
    mkfifo("desdfifo",0666);
    fds=open("desdfifo",O_WRONLY);
    if(-1==fds)
    {
        perror("Error\n");
        exit(EXIT_FAILURE);
    }
    write(fds,"HELLO\n",6);
    close(fds);
    fds=open("desdfifo", O_RDONLY);
    if(-1==fds)
    {
        perror("Error.\n");
        exit(EXIT_FAILURE);
    }
    read(fds,buf1,BUFF_SIZE);
    printf("Recieved from user2: %s",buf1);
    close(fds);
    return 0;
}
/*
o/p: Recieved from user2: WORLD.
*/