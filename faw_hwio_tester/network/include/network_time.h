#ifndef __NETWORK_TIME_H
#define __NETWORK_TIME_H

#include "network_test.h"

void get_elapsed_time(struct network_test* test);
void get_start_time(struct network_test* test);
void get_interval_start_time(struct network_test* test);
int transmit_time_finished(struct network_test* test);
int reached_time_interval(struct network_test* test);


#endif
