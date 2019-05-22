#ifndef __DEBUG__
#define __DEBUG__

#include <stdio.h>
#include <stdlib.h>

#define pr_debug(fmt,...)   do{ printf("[%ld] DEBUG: "fmt,(long)getpid(),##__VA_ARGS__); fflush(stdout); }while(0)
#define pr_info(fmt,...)    do{ printf("[%ld] INFO:  "fmt,(long)getpid(),##__VA_ARGS__); fflush(stdout); }while(0)
#define pr_err(fmt,...)   	do{ printf("[%ld] ERROR: "fmt,(long)getpid(),##__VA_ARGS__); fflush(stdout); }while(0)
#define err_exit(fmt,...)   do{ printf("[%ld] ERROR: "fmt,(long)getpid(),##__VA_ARGS__); exit(1); }while(0)

#endif
