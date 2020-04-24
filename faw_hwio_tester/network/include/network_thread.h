#ifndef __NETWORK_THREAD_H
#define __NETWORK_THREAD_H
void *server_thread_func(void *arg);
void *statis_thread_func(void *arg);
void *transmit_thread_func(void *arg);
void *function_transmit_thread_func(void *arg);

#endif
