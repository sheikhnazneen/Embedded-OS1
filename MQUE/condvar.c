#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond= PTHREAD_COND_INITIALIZER;
int data_ready=0;

void* producer(void* arg)
{
    printf("Producer: Generating Code.\n");
    sleep(2);
    pthread_mutex_lock(&mutex);
    data_ready =1;
    printf("Producer: Data is ready.\n");
    sleep(3);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return NULL;
}
void* consumer(void* arg)
{
    pthread_mutex_lock(&mutex);
    if(data_ready==0){
        printf("Consumer:Waiting for data.\n");
        pthread_cond_wait(&cond,&mutex);
    }
    printf("Consumer:Data consumed.\n");
    pthread_mutex_unlock(&mutex);
    return NULL;
}
int main()
{
    pthread_t producer_thread_id,consumer_thread_id;

    pthread_create(&producer_thread_id,NULL,producer,NULL);
    pthread_create(&consumer_thread_id,NULL,consumer,NULL);
    pthread_join(producer_thread_id,NULL);
    pthread_join(consumer_thread_id,NULL);
    
    //cleanup
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&mutex);
    return 0;

}