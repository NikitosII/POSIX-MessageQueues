#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <mqueue.h>
#include <string.h>
#include <errno.h>

volatile int thread_finish = 0;
mqd_t mq;

void* thread_func(void* arg) {
    struct mq_attr attr, new_attr;
    
    // текущие атрибуты
    if (mq_getattr(mq, &attr)) {
        perror("mq_getattr");
        return NULL;
    }
    printf("Attributes: flags=%ld, maxmsg=%ld, msgsize=%ld, curmsgs=%ld\n",
           attr.mq_flags, attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);


    while (!thread_finish) {
        long host_id = gethostid();
        printf("Hostid: %ld\n", host_id);

        char buffer[64]; 
        snprintf(buffer, sizeof(buffer), "HostID: %ld", host_id);
        size_t len = strlen(buffer) + 1;

        if (mq_send(mq, buffer, len, 0) == -1) {
            if (errno == EAGAIN) {
                printf("Queue is full, retrying...\n");
            } else {
                perror("mq_send");
            }
        } 
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t thread;
    struct mq_attr attr = {
        .mq_flags = O_NONBLOCK,
        .mq_maxmsg = 10,
        .mq_msgsize = 32 
    };

    mq = mq_open("/myqueue", O_CREAT | O_WRONLY | O_NONBLOCK, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&thread, NULL, thread_func, NULL)) {
        perror("pthread_create");
        mq_close(mq);
        mq_unlink("/myqueue");
        exit(EXIT_FAILURE);
    }

    printf("Press Enter to stop...\n");
    getchar();

    thread_finish = 1;
    pthread_join(thread, NULL);

    mq_close(mq);
    mq_unlink("/myqueue");

    return EXIT_SUCCESS;
}