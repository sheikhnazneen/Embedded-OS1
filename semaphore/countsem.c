#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>

#define NUM_THREADS 5
sem_t semaphore;
void* worker(void* arg){
    int thread_id= *((int*)arg);

    printf("Thread %d waiting to enter critical section\n",thread_id);
    sem_wait(&semaphore);
    printf("Thread %d entered critical section\n",thread_id);
    sleep(1);
    printf("Thread %d leaving critical section\n",thread_id);
    sem_post(&semaphore);
    return NULL;
}
int main(){
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    sem_init(&semaphore,0,5);

    for(int i=0;i<NUM_THREADS;i++)
    {
        thread_ids[i]=i+1;
        pthread_create(&threads[i],NULL,worker,&thread_ids[i]);
    }
    for(int i=0;i<NUM_THREADS;i++)
    {
        pthread_join(threads[i],NULL);
    }
    sem_destroy(&semaphore);
    return 0;
}