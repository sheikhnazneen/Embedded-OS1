#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
sem_t sem_count;
int count=0;

void *incthread(void *data)
{
    for(int i=0;i<=10;i++)
    {
        sem_wait(&sem_count);
        count++;
        printf("Inc:%d\n",count);
        sem_post(&sem_count);
    }
}

void *decthread(void *data)
{
    for(int i=0;i<=10;i++)
    {
        sem_wait(&sem_count);
        count--;
        printf("Dec:%d\n",count);
        sem_post(&sem_count);
    }
}
int main()
{
    pthread_t incid,decid;
    sem_init(&sem_count,0,1);
    pthread_create(&incid,NULL,incthread,NULL);
    pthread_create(&decid,NULL,decthread,NULL);
    pthread_join(incid,NULL);
    pthread_join(decid,NULL);
    sem_destroy(&sem_count);
    //return 0;
    pthread_exit(0);
}