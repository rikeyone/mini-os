#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define pr_debug(fmt,...)   do{ printf("[%ld] DEBUG: "fmt,(long)getpid(),##__VA_ARGS__); fflush(stdout); }while(0)
#define pr_info(fmt,...)    do{ printf("[%ld] INFO:  "fmt,(long)getpid(),##__VA_ARGS__); fflush(stdout); }while(0)
#define pr_err(fmt,...)     do{ printf("[%ld] ERROR: "fmt,(long)getpid(),##__VA_ARGS__); fflush(stdout); }while(0)
#define err_exit(fmt,...)   do{ printf("[%ld] ERROR: "fmt,(long)getpid(),##__VA_ARGS__); exit(1); }while(0)

#define MSQ_MODE 666
#define MIN(a,b) (((a)<(b))?(a):(b))

struct msq_msg {
	long type;
	char data[512];
};

int msq_create(void)
{
	key_t key;
	int msqid;

	key = ftok("/home/xiehaocheng/work/qemu", 1);
	if (key == (key_t)-1) {
		pr_err("ftok error, %s\n", strerror(errno));
		return -1;
	}

	msqid = msgget(key, MSQ_MODE|IPC_CREAT|IPC_EXCL);
	if (msqid < 0) {
		pr_err("msgget error, %s\n", strerror(errno));
		return -1;
	}
	pr_info("msqid:%d\n", msqid);
	return msqid;
}

int msq_create_private(void)
{
	int msqid;

	msqid = msgget(IPC_PRIVATE, MSQ_MODE|IPC_CREAT|IPC_EXCL);
	if (msqid < 0) {
		pr_err("msgget error, %s\n", strerror(errno));
		return -1;
	}
	pr_info("msqid:%d\n", msqid);
	return msqid;
}


int msq_send(int msqid, int type, char *buf, int size)
{
	int ret;
	struct msq_msg msg = {0};
	int nbyte;

	nbyte = MIN(512, size);
	msg.type = type;
	strncpy(msg.data, buf, nbyte);

	do {
		ret = msgsnd(msqid, &msg, nbyte, 0);
		if (ret < 0) {
			if (errno == EINTR) {
				continue;
			} else if (errno == EIDRM) {
				pr_err("msg queue deleted\n");
				return -1;
			} else {
				pr_err("msgsnd error, %s\n", strerror(errno));
				return -1;
			}
		} else if (ret == 0)
			break;
	} while(1);

	return 0;
}

int msq_recv(int msqid, int type, char *buf, int size)
{
	int max = 512, nbyte;
	struct msq_msg msg = {0};

	nbyte = msgrcv(msqid, &msg, max, type, MSG_NOERROR);
	if (nbyte < 0) {
		pr_err("msgrcv error, %s\n", strerror(errno));
		return -1;
	}

	nbyte = MIN(nbyte, size);

	strncpy(buf, msg.data, nbyte);

	return nbyte;
}

int msq_delete(int msqid)
{
	int ret;
	struct msqid_ds msqds;

	ret = msgctl(msqid, IPC_RMID, &msqds);
	if (ret < 0) {
		pr_err("msgctl error\n");
		return -1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	pid_t pid;
	int msqid;
	char buf[512];

	msqid = msq_create_private();
	if (msqid < 0) {
		err_exit("msq_create error\n");
	}
	if ((pid = fork()) < 0) {
		err_exit("fork error\n");
	} else if (pid > 0) {
		strcpy(buf, "Hello, I'm your parent!\n");
		msq_send(msqid, 1, buf, strlen(buf) + 1);
		usleep(300000);
		msq_recv(msqid, 2, buf, 512);
		pr_info("parent:%s\n", buf);
		wait(NULL);
		msq_delete(msqid);
	} else {
		strcpy(buf, "Hello, I'm your child!\n");
		msq_send(msqid, 2, buf, strlen(buf) + 1);
		usleep(300000);
		msq_recv(msqid, 1, buf, 512);
		pr_info("child:%s\n", buf);
		exit(0);
	}
	return 0;
}
