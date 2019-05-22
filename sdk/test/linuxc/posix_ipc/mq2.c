#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <mqueue.h>
#include "include/debug.h"

#define MSG_QUEUE_NAME "/test_msg_queue"
#define MSG_QUIT "quit"
#define MAX_SIZE 512

int main(int argc, char **argv)
{
    mqd_t mq;

    if (argc < 2)
        err_exit("need an argument as message\n");

    /* open the mail queue */
    mq = mq_open(MSG_QUEUE_NAME, O_WRONLY);
    if ((mqd_t)-1 == mq) {
        err_exit("mq_open error, %s\n", strerror(errno));
    }

    pr_info("mq_open success\n");

    /* send the message */
    if (0 < mq_send(mq, argv[1], strlen(argv[1]), 0)) {
        err_exit("mq_send error,%s\n", strerror(errno));
    }

    /* cleanup */
#if 0
    if ((mqd_t)-1 == mq_close(mq)){
        err_exit("mq_close error, %s\n", strerror(errno));
    }
#else
    if (-1 == mq_unlink(MSG_QUEUE_NAME)){
        err_exit("mq_unlink error, %s\n", strerror(errno));
    }
    if (0 < mq_send(mq, argv[1], strlen(argv[1]), 0)) {
        err_exit("mq_send error,%s\n", strerror(errno));
    }
#endif

    return 0;
}
