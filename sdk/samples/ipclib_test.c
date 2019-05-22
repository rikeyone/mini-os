#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include "siglib.h"
#include "debug.h"
#include "ipc.h"
#include "daemon.h"

static void signal_handler(int signo)
{
	switch (signo) {
	case SIGINT:
	case SIGTERM:
		pr_info("signal received:%d\n", signo);
		ipc_stop_loop();
		break;
	default:
		break;
	}
}

void app_msg_handler(void *data)
{
	struct ipc_msg *msg = (struct ipc_msg *)data;

	switch(msg->type) {
	case MSG_TYPE_WATCHDOG:
		pr_info("watchdog message received!\n");
		ipc_watchdog_feed();
		break;
	default:
		pr_info("unexpected message received! type:%d\n", msg->type);
		ipc_stop_loop();
		break;
	}
}

int main(int argc, char *argv[])
{
	int ret;

	daemon_init(argv[1]);

	/*
	 * set_signal_thread must be called
	 * before any new thread is created.
	 * */
	set_signal_thread(signal_handler);

	ret = ipc_init(argv[1], app_msg_handler);
	if (ret < 0)
		err_exit("ipc_init error\n");

	ipc_watchdog_init(20);
	ipc_main_loop();
	ipc_deinit();
	pr_info("main loop exit!\n");
	return 0;
}
