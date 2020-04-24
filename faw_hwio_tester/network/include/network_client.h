#ifndef __NETWORK_CLIENT_H
#define __NETWORK_CLIENT_H

#include "network_test.h"

int network_test_client_init(struct network_test* test);
int network_test_client_run(struct network_test* test);
int network_test_client_exit(struct network_test* test);

#endif
