#ifndef __NETWORK_TEST_H
#define __NETWORK_TEST_H

#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#define FUNCTION_MODE    0
#define PERFORMANCE_MODE 1

#define DEFAULT_MAX_EVENT   16
#define DEFAULT_MAX_CONNECT 16

#define DEFAULT_UDP      0          /* 默认使用TCP通信 */
#define DEFAULT_INTERVAL 1          /* 默认时间间隔为1s进行一次统计 */
#define DEFAULT_TIME     10         /* 默认测试程序的总运行时间为10s */
#define DEFAULT_PORT     4006       /* 默认端口为4006 */
#define DEFAULT_THREAD   1          /* 默认单线程测试 */
#define DEFAULT_WIN_SIZE 128
#define DEFAULT_PACK_SIZE 128       /* 默认每包数据的大小为128KB */ 
#define DEFAULT_MODE     PERFORMANCE_MODE
#define DEFAULT_TRANSMIT_TIMES   10 /* 功能测试中默认发包数量 */
#define MAX_IP_SIZE 32
#define DEFAULT_HOST_IP "127.0.0.1"


typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

struct transmit_thread_attr {
    uint8_t   local_id;
    int32_t   socket_fd;
    pthread_t pthread_id;
    pthread_mutex_t send_num_mutex;
    int8_t  *send_buff;
    int8_t  *recv_buff;
    uint32_t send_buff_size;
    uint64_t interval_send_num;
    uint8_t finished;
    struct network_test *test_inst;
};

struct connect_thread_attr {
    uint8_t   local_id;
    pthread_t connect_thread_id;
    int32_t   socket_fd;
    struct sockaddr_in client_addr;
    struct network_test* test;
};

struct server_attr {
    struct connect_thread_attr* connect_thread;
    uint8_t *used;
    uint32_t max_connect;
};

struct network_test {
    uint8_t isServer;
    uint8_t  udp;
    uint8_t  interval;
    uint16_t time;
    uint16_t port;
    // uint16_t parallel;
    uint32_t windows_size;
    uint32_t package_size;
    uint32_t data_size;
    uint8_t  test_mode;
    int8_t   *send_buff;
    int8_t   *recv_buff;
    uint32_t send_buff_size;
    uint32_t transmit_times;

    char     ip[MAX_IP_SIZE];

    uint16_t    pthread_num;
    struct transmit_thread_attr *transmit_thread;
    pthread_t statis_thread_id;
    pthread_mutex_t test_mutex;
    struct server_attr server;
    struct sockaddr_in sockaddr;

    // for transmit_time
    double start_time_lf;
    struct timeval start_time;
    struct timeval end_time;

    struct timeval interval_start_time;
    struct timeval current_time;
    struct timeval elapsed_time;

    unsigned long long interval_send_sum;
    unsigned long long send_sum;

    int finished;
};

void network_test(void);

#endif
