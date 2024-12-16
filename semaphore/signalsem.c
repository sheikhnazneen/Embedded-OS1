#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
sem_t sem_in_proc,sem_proc_in;
int a,b;
void *proc_thread(void *data)
{
    int sum=0;
    while(1)
    {
        sem_wait(&sem_in_proc);
        sum=a+b;
        printf("proc_thread: sum=%d\n",sum);
        sem_post(&sem_proc_in);
    }
}
void *in_thread(void *data)
{
    while(1)
    {
        sem_wait(&sem_proc_in);
        printf("Enter a:");
        scanf("%d",&a);
        printf("Enter b:");
        scanf("%d",&b);
        printf("in_thread: a=%d b=%d\n",a,b);
        sem_post(&sem_in_proc);
    }
}
int main(){
    pthread_t in_id,proc_id;
    sem_init(&sem_in_proc,0,0);
    sem_init(&sem_proc_in,0,1);

    pthread_create(&in_id,NULL,in_thread,NULL);
    pthread_create(&proc_id,NULL,proc_thread,NULL);

    pthread_join(in_id,NULL);
    pthread_join(proc_id,NULL);

    sem_destroy(&sem_in_proc);
    sem_destroy(&sem_proc_in);
    //return 0;
    pthread_exit(0);
}
