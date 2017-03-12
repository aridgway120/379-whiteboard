#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <ctype.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
void * thread1 (void * arg){
    int * num = (int *) arg;
    printf("Integer arg in thread is: %d\n", *num);
    *num = 10;
    return ((void *) 0);  // pthread_exit() also works
}
pthread_t tid;
int x = 5;
int main(){
    int err = pthread_create(&tid, NULL, thread1,(void*)&x);
    printf("Thread ID is %lu\n", (unsigned long) tid);
    pthread_join(tid, NULL);  // tid specifies the thread we are joining
    printf("x after joining thread is %d\n",x);

    
}