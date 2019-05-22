#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "watchdog.h"
#include "debug.h"

#define ONE_SECOND 1000000000
struct watchdog_timer watchdog;

int main()
{
	software_watchdog_init(&watchdog);
	software_watchdog_start(&watchdog, 10);
	while(1) {
		sleep(5);
		pr_info("feed watchdog!\n");
	}
	return 0;
}
