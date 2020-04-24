#ifndef __NETWORK_SERVER_H
#define __NETWORK_SERVER_H

#include "network_test.h"

int network_test_server_init(struct network_test* test);
int network_test_server_run(struct network_test* test);
int network_test_server_exit(struct network_test* test);

#endif
