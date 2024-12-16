#include<stdio.h>
#include<pthread.h>
 void *incThread(void*);
 void *decTHread(void*);

 int count;
 pthread_mutex_t countmutex;
 void *inc(void *data)
 {
    while(1)
    {
        pthread_mutex_lock(&countmutex);
        count++;
        printf("INC:%d\n",count);
        pthread_mutex_unlock(&countmutex);
    }
 }
 void *dec(void *data)
 {
    while(1)
    {
        pthread_mutex_lock(&countmutex);
        count--;
        printf("DEC:%d\n",count);
        pthread_mutex_unlock(&countmutex);
    }
 }
 int main()
 {
    pthread_t INCid,DECid;
    pthread_create(&INCid,NULL,inc,NULL);
    pthread_create(&DECid,NULL,dec,NULL);
    pthread_mutex_init(&countmutex,NULL);
    pthread_join(INCid,NULL);
    pthread_join(DECid,NULL);
    pthread_mutex_destroy(&countmutex);
    return 0;
    //sleep(5);


 }
