#ifndef __NETWORK_UTIL_H
#define __NETWORK_UTIL_H

#include "network_test.h"
int apply_connect_thread(struct server_attr *server);
void periodic_report(struct network_test* test);
void summary_report(struct network_test* test);
ssize_t	NSend(int fd, const void *sendline, size_t n);
ssize_t NRead(int fd, const void *readline, size_t n);
int check_buff_8bytes(uint64_t *buff1, uint64_t *buff2, int size);
uint32_t random_data(void);

#endif
