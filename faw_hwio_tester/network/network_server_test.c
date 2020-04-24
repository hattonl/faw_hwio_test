#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>

#include "network_server.h"

int main(int argc, const char **argv)
{
		 int ret = 0;
		struct network_test *test = 
				(struct network_test*)malloc(sizeof(struct network_test));

		if (test == NULL) {
        perror("network test malloc error\n");
        return 0;
    }

		ret = network_test_init(argc, argv, test);
		if (ret == -1) {
				printf("network test init error.\n");
				goto OUT;
		}

		ret = network_test_run(test);
		if (ret == -1) {
				printf("network test run error.\n");
				goto OUT;
		}

		network_test_exit(test);

OUT:
		free(test);

		return 0;
}
