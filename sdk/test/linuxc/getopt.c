#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
    int f_flag, opt;
    int t_flag, nsecs;

    nsecs = 0;
    f_flag = 0;
	t_flag = 0;
	opterr = 0; /*don't print getopt error message*/

    while ((opt = getopt(argc, argv, "ft:")) != -1) {
        switch (opt) {
        case 'f':
            f_flag = 1;
            break;
        case 't':
            t_flag = 1;
            nsecs = atoi(optarg);
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-t nsecs] [-f] name\n",
                    argv[0]);
            fprintf(stderr, "optopt:%c\n", optopt);
            exit(EXIT_FAILURE);
        }
    }

    printf("f_flag=%d; t_flag=%d; nsecs=%d; optind=%d\n", f_flag, t_flag, nsecs, optind);

    if (optind >= argc) {
        fprintf(stderr, "expected non-option argument\n");
        exit(EXIT_FAILURE);
    } else { /* optind < argc */
        printf("non-option arguments: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
    }

    exit(EXIT_SUCCESS);
}
