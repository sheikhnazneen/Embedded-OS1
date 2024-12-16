//shm_1.c;
#include<stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include<sys/syscall.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#define SHM_NAME "/desd_shm"
#define SIZE 4096
int main()
{
    int shm_fd=shm_open(SHM_NAME,O_CREAT|O_RDWR,0666);
    if(shm_fd==-1){
        perror("shm_open");
        exit(1);
    }
     if(ftruncate(shm_fd,SIZE)==-1){
        perror("ftruncate");
        exit(1);
    }
    char *shm_ptr=mmap(NULL,SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,shm_fd,0);
    if(shm_ptr == MAP_FAILED){
        perror("mmap");
        exit(1);
    }

    char mesg[]="HELLO DUDE.\n";
    strncpy(shm_ptr,mesg,sizeof(mesg));
    printf("Message Written: %s\n",mesg);
    close(shm_fd);
    return 0;
}

/* first compile process. ---- gcc shm_1.c -o s -lrt
    secompiled process.   ----gcc sharedmem.c -o shm -lrt
o/p: Message Written: HELLO DUDE.
*/