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
    int fdr;
    char buf2[BUFF_SIZE];
    fdr=open("desdfifo",O_RDONLY);
    if(-1==fdr)
    {
        perror("Eror.\n");
        exit(EXIT_FAILURE);
    }
    read(fdr,buf2,BUFF_SIZE);
    printf("Recieved from User1: %s",buf2);
    close(fdr);
    fdr=open("desdfifo",O_WRONLY);
    if(-1==fdr)
    {
        perror("Eror.\n");
        exit(EXIT_FAILURE);
    }
    write(fdr,"WORLD.\n",6);
    close(fdr);
}
/*
o/p:Recieved from User1: HELLO
*/