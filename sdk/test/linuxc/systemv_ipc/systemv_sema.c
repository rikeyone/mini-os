/*
 *
 *	id = sema_create(key, initval);	# create with initial value
 *	id = sema_open(key);		# open (must already exist)
 *	sema_down(id);			# P = down by 1
 *	sema_up(id);				# V = up by 1
 *	sema_close(id);			# close
 *
 *  Define two semaphores in the semaphore set:
 *
 *  sem[0]: this semaphore is for normal use, down and up is operated
 *  on this semaphore.
 *
 *  sem[1]: trace for the processes which using this semaphore set, so we
 *  can delete the semaphore when no one is using it. Why do it like this?
 *  Because we want to handle the semaphore set by the program itself, so
 *  we don't  need to clean it by "ipcrm" command or some way like it.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include "include/debug.h"

/*
 * system call:
 *  - semget
 *  - semctl
 *  - semop
 */

/* initial value of process counter */
#define	PROCESS_COUNT	10000
#define SEMA_KEY 0x65539

void sema_destory(int id)
{
	/* delete this semaphore set */
	if (semctl(id, 0, IPC_RMID, 0) < 0) {
		pr_err("can't IPC_RMID, %s\n", strerror(errno));
	}
	pr_info("sema destoryed!\n");
}

int sema_create(key_t key, int initval)
{
	int		id;
	union semun {
		int		val;
		struct semid_ds	*buf;
		unsigned short		*array;
	} semctl_arg;
	struct sembuf	op[1] = {
		{1, -1, SEM_UNDO}
	};

	if (key == IPC_PRIVATE)
		return -1;	/* not intended for private semaphores */

	else if (key == (key_t) -1)
		return -1;	/* probably an ftok() error by caller */

	if ((id = semget(key, 2, 0666 | IPC_CREAT)) < 0)
		return -1;	/* permission problem or tables full */


	/* init sem[0] to the init value */
	semctl_arg.val = initval;
	if (semctl(id, 0, SETVAL, semctl_arg) < 0) {
		pr_err("can SETVAL[0]\n");
		goto err_out;
	}

	/* set sem[1] to max process count */
	semctl_arg.val = PROCESS_COUNT;
	if (semctl(id, 1, SETVAL, semctl_arg) < 0) {
		pr_err("can SETVAL[1]\n");
		goto err_out;
	}

	/* TODO: SHOULD LOCK */
	/*
	 * should decrease sem[1], DOWN when open
	 */
	if (semop(id, op, 1) < 0) {
		pr_err("can't semop\n");
		goto err_out;
	}
	/* TODO: SHOULD UNLOCK */

	return id;

err_out:
	/* delete this semaphore set */
	sema_destory(id);
	return -1;
}

int sema_open(key_t key)
{
	int	id;
	struct sembuf	op[1] = {
		{1, -1, SEM_UNDO}
	};

	if (key == IPC_PRIVATE)
		return(-1);	/* not intended for private semaphores */

	else if (key == (key_t) -1)
		return(-1);	/* probably an ftok() error by caller */

	if ((id = semget(key, 2, 0)) < 0)
		return(-1);	/* doesn't exist, or tables full */

	/* TODO: SHOULD LOCK */

	/*
	 * should decrease sem[1], DOWN when open
	 */
	if (semop(id, op, 1) < 0) {
		pr_err("can't semop\n");
		return -1;
	}
	/* TODO: SHOULD UNLOCK */
	return(id);
}


void sema_close(int id)
{
	register int	semval;
	struct sembuf	op[1] = {
		{1, 1, SEM_UNDO}
	};

	/* TODO: SHOULD LOCK */

	/*
	 * should increase sem[1], UP when close
	 */
	if (semop(id, op, 1) < 0) {
		err_exit("can't semop\n");
	}

	if ((semval = semctl(id, 1, GETVAL, 0)) < 0)
		err_exit("can't GETVAL\n");

	if (semval > PROCESS_COUNT) 	/* exception: this should not happen */
		err_exit("sem[1] > 10000\n");
	else if (semval == PROCESS_COUNT) /* no process holding this semaphore set, so destory it */
		sema_destory(id);
	else
		pr_info("still have process hold the semaphore!!! Don't destory!\n");

	/* TODO: SHOULD UNLOCK */
}

static int sema_op(int id, int value)
{
	struct sembuf	op[1] = {
		{0, 99, SEM_UNDO}
	};

	if (value == 0) {
		pr_err("can't have value 0\n");
		return -1;
	}

	op[0].sem_op = value;

	if (semop(id, op, 1) < 0) {
		pr_err("sem_op error\n");
		return -1;
	}
	return 0;
}

/*
 *  DOWN operation.
 */

void sema_down(int id)
{
	sema_op(id, -1);
}

/*
 *  UP operation.
 */

void sema_up(int id)
{
	sema_op(id, 1);
}

int main(int argc, char argv[])
{
	pid_t pid;
	int id;

	if ((pid = fork()) < 0)
		err_exit("fork error\n");
	else if (pid > 0) {
		id = sema_create(SEMA_KEY, 1);
		if (id < 0)
			exit(1);
		pr_info("parent get semaphore\n");
		sema_down(id);
		sleep(5);
		pr_info("parent put semaphore\n");
		sema_up(id);
		sema_close(id);
		wait(NULL);
	} else {
		sleep(1);
		id = sema_open(SEMA_KEY);
		pr_info("child wait for sema\n");
		sema_down(id);
		pr_info("child get sema\n");
		sema_up(id);
		pr_info("child put sema\n");
		sema_close(id);
	}
	return 0;
}
