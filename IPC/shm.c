//shm_2.c
#include<stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include<sys/syscall.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#define SHM_NAME "/desd_shm"
#define SIZE 4096
int main()
{
    int shm_fd=shm_open(SHM_NAME,O_RDONLY,0666);
    if(shm_fd==-1)
    {
        perror("shm_open.\n");
        exit(1);
    }
    char *shm_ptr=mmap(NULL,SIZE,PROT_READ,MAP_SHARED,shm_fd,0);
    if(shm_ptr==MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }
    printf("Message read: %s\n",shm_ptr);
    close(shm_fd);
    if(shm_unlink(SHM_NAME)==-1){
        perror("shm_unlink.");
        exit(1);
    }
}
/* if we compile first this process.eerror will occur.
o/p:eror:shm_open.
: No such file or directory
*/
/* sec ond compile proces.
o/p:Message read: HELLO DUDE.

*/