#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

pthread_rwlock_t rwl;
int count;
void *read1(void *data)
{
   while(1)
   {
    pthread_rwlock_rdlock(&rwl);
    printf("read 1 locked\n");
    sleep(1);
    pthread_rwlock_unlock(&rwl);
    printf("read 1 unlocked\n");
    sleep(2);
   }
}
void *read2(void *data)
{
    while(1)
    {
    pthread_rwlock_rdlock(&rwl);
    printf("read 2 locked\n");
    sleep(3);
    pthread_rwlock_unlock(&rwl);
    printf("read 2 unlocked\n");
    sleep(3);
    }
}
void *write1(void *d)
{
    while(1)
    {
        pthread_rwlock_wrlock(&rwl);
        printf("write 1 locked\n");
        printf("\ncount=%d\n",count);
        pthread_rwlock_unlock(&rwl);
        printf("write 1 unlocked\n");
        sleep(1);
    }
}
void *write2(void *d)
{
    while(1)
    {
        pthread_rwlock_wrlock(&rwl);
        printf("write 2 locked\n");
        printf("\ncount=%d\n",count);
        pthread_rwlock_unlock(&rwl);
        printf("write 2 unlocked\n");
        sleep(1);
    }
}
void main()
{
    pthread_t r1,r2,w1,w2;
    int a;

    pthread_rwlock_init(&rwl,NULL);
    pthread_create(&r1,NULL,read1,NULL);
    printf("Creating:read1\n");
    pthread_create(&r2,NULL,read2,NULL);
    printf("Creating:read2\n");
    pthread_create(&w1,NULL,write1,NULL);
    printf("Creating:write1\n");
    pthread_create(&w2,NULL,write2,NULL);
    printf("Creating:write2\n");

    pthread_join(r1,NULL);
    printf("joining:read1\n");
    pthread_join(r2,NULL);
    printf("joining:read2\n");
    pthread_join(w1,NULL);
    printf("joining:write1\n");
    pthread_join(w2,NULL);
    printf("joining:write2\n");

    pthread_rwlock_destroy(&rwl);
}