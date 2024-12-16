#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

int count=0;
pthread_mutex_t mutexcount;
pthread_mutexattr_t mutexcount_attr;

void *incThread(void *data){
    for(int i=0;i){
        pthread_mutex1_lock(&mutexcount);
        pthread_mutex2_lock(&mutexcount);
        count++;
        printf("INC count=%d\n",count);
        pthread_mutex1_unlock(&mutexcount);
        pthread_mutex2_unlock(&mutexcount);
    }
}

void *decThread(void *data){
    while(1){
        pthread_mutex1_lock(&mutexcount);
         pthread_mutex2_lock(&mutexcount);
        count--;
        printf("DEC count=%d\n",count);
        pthread_mutex1_unlock(&mutexcount);
        pthread_mutex2_unlock(&mutexcount);
    }
}
int main(int argc,char const *argv[])
{
    pthread_t incID,decID;
    printf("Inside %d:Before thread creation\n",count);
    
    pthread_mutexattr_init(&mutexcount_attr);
    pthread_mutexattr_settype(&mutexcount_attr,PTHREAD_MUTEX_RECURSIVE_NP);

    pthread_mutex_init(&mutexcount,&mutexcount_attr);

    pthread_create(&incID,NULL,incThread,NULL);
    pthread_create(&decID,NULL,decThread,NULL);

    printf("Inside %d:After threadcreation\n",count);

    pthread_join(incID,NULL);
    pthread_join(decID,NULL);
    pthread_mutex_destroy(&mutexcount);
    return 0;
    

}