#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <mqueue.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include "include/debug.h"

#define MSG_QUEUE_NAME "/test_msg_queue"
#define MSG_QUIT "quit"
#define MAX_SIZE 512

int mq_receive_timeout(mqd_t mq, char *buf, int len, int second)
{
    int bytes_read;
    struct timespec timeout;

    do {
        clock_gettime(CLOCK_REALTIME, &timeout);
        pr_info("tv_sec:%ld tv_nsec:%ld\n",
                    timeout.tv_sec,
                    timeout.tv_nsec);
        timeout.tv_sec += second;
        bytes_read = mq_timedreceive(mq, buf, len, NULL, &timeout);
        if(bytes_read < 0) {
            if (errno != ETIMEDOUT)
                err_exit("mq_receive error, %s\n", strerror(errno));
            else
                pr_info("mq_timedreceive timeout\n");
        } else if (bytes_read > 0)
            break;
    } while(1);

    return bytes_read;
}


int main(int argc, char **argv)
{
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_SIZE + 1];
    int must_stop = 0;

    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    /* create the message queue */
    mq = mq_open(MSG_QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    if ((mqd_t)-1 == mq) {
        err_exit("mq_open error, %s\n", strerror(errno));
	}

    pr_info("mq_open success\n");
    do {
        ssize_t bytes_read;

        pr_info("start received...\n");
        /* receive the message */
#if 0
        bytes_read = mq_receive_timeout(mq, buffer, MAX_SIZE, 3);
        if(bytes_read < 0) {
            err_exit("mq_receive error, %s\n", strerror(errno));
        }
#else
        bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
        if(bytes_read < 0) {
            err_exit("mq_receive error, %s\n", strerror(errno));
        }
#endif
        buffer[bytes_read] = '\0';
        if (!strncmp(buffer, MSG_QUIT, strlen(MSG_QUIT))) {
            must_stop = 1;
        } else {
            pr_info("Received: %s\n", buffer);
        }
    } while (!must_stop);

    /* cleanup */
    if ((mqd_t)-1 == mq_close(mq)) {
        err_exit("mq_close error\n");
    }
    if ((mqd_t)-1 == mq_unlink(MSG_QUEUE_NAME)) {
        err_exit("mq_unlink error\n");
    }


    return 0;
}
