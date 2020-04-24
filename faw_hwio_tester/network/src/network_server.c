/*
 * network_server.c
 * switch_test run in server role
 * 
**/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>  
#include <sys/socket.h>
#include <string.h>
#include <sys/epoll.h>
#include <signal.h>

#include "network_test.h"
#include "network_thread.h"
#include "network_time.h"
#include "network_parse.h"
#include "network_server.h"
#include "network_util.h"

#define MAX_THREAD_NUM          16


static volatile int running = 1;

static void sigterm(int signo)
{
    running = 0;
}

/**
 * This function is responsible for parsing parameters, 
 * verifying the legality of the parameters 
 * and the memory space required by the application.
 */
int network_test_server_init(struct network_test* test)
{
    int ret = 0;

    /* zeros memory */
    // memset(test, 0, sizeof(struct network_test));

    // network_test_init_hook(test);
    // param_parse(argc, argv, test);

    /* test param check */
    // ret = network_param_check(test);
    // if (ret < 0) {
       // return ret;
    // }

    /* init buff size */
    test->send_buff_size = test->package_size*1024*(sizeof(char));


    /* TODO:  */
    test->server.max_connect = DEFAULT_MAX_CONNECT;

    test->server.connect_thread = 
        (struct connect_thread_attr *) \
        malloc(sizeof(struct connect_thread_attr) * \
        test->server.max_connect);
    
    if (test->server.connect_thread == NULL) {
        perror("connect thread attr malloc error.\n");
        return -1;
    }

    test->server.used = 
        (uint8_t *)malloc(sizeof(uint8_t)*test->server.max_connect);
    if (test->server.used == NULL) {
        perror("connect used flags malloc error.\n");
        return -1;
    }
    memset(test->server.used, 0, test->server.max_connect);

    return 0;
}

/**
 * This function is used to free up memory space
 */
int network_test_server_exit(struct network_test* test)
{
    /* free memory */
    if (test->server.connect_thread != NULL) {
        free(test->server.connect_thread);
    }
    if (test->server.used != NULL) {
        free(test->server.used);
    }

    return 0;
}



int network_test_server_run(struct network_test* test)
{
    int ret, i;
    int server_fd, client_fd, epoll_fd;
    int ready_count;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t addr_len;
    struct epoll_event event;
    struct epoll_event* event_array;
    int socket_type;

    running = 1;

    signal(SIGTERM, sigterm);
    signal(SIGHUP, sigterm);
    signal(SIGINT, sigterm);

    event_array = (struct epoll_event*)
        malloc(sizeof(struct epoll_event)*DEFAULT_MAX_EVENT);
    if (event_array == NULL) {
        perror("epoll event array malloc error.\n");
        return -1;
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(test->port);
    
    if (test->udp == 1) {
        socket_type = SOCK_DGRAM;
    } else {
        socket_type = SOCK_STREAM;
    }
    
    server_fd = socket(AF_INET, socket_type, 0);
    if(server_fd == -1) {
        perror("create socket failed.\n");
        return 1;
    }

    ret = bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(ret == -1) {
        perror("bind failed.\n");
        return 1;
    }

    epoll_fd = epoll_create(1);
    if(epoll_fd == -1) {
        perror("epoll_create failed.\n");
        return 1;
    }

    event.events  = EPOLLIN;
    event.data.fd = server_fd;
    ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);
    if(ret == -1) {
        perror("epoll_ctl failed.\n");
        return 1;
    }

    ret = listen(server_fd, 16);
    if(ret == -1) {
        perror("listen failed.\n");
        return 1;
    }
    printf("listen to client connect.\n");

    while(running) {
        ready_count = epoll_wait(epoll_fd, event_array, DEFAULT_MAX_EVENT, -1);
        if(ready_count == -1) {
            perror("epoll_wait failed.\n");
            return 1;
        }
        for(i = 0; i < ready_count; i++) {
            if(event_array[i].data.fd == server_fd) {
                // memset(&client_addr, 0, sizeof(struct sockaddr_in));
                struct connect_thread_attr *connect_thread;
                int ret = apply_connect_thread(&test->server);
                if (ret == -1) {
                    printf("apply connect thread error.\n");
                    break;
                }
                printf("thread %d will used.\n", ret);
                connect_thread = &test->server.connect_thread[ret];

                connect_thread->socket_fd = accept(server_fd,
                            (struct sockaddr*)&connect_thread->client_addr, 
                            &addr_len);

                if(connect_thread->socket_fd == -1) {
                    perror("accept failed.\n");
                    test->server.used[ret] = 0;
                    return 1;
                }
                // connect_thread->socket_fd = client_fd;
                printf("client from port: %d ", 
                    ntohs(connect_thread->client_addr.sin_port));
                
                printf("create thread. \n");

                connect_thread->test = test;
                ret = pthread_create(&connect_thread->connect_thread_id, 
                                     NULL, 
                                     server_thread_func,
                                     connect_thread);

                if (ret != 0) {
                    perror("pthread create failed.\n");
                    test->server.used[ret] = 0;
                    return 1;
                }
            }
        } // for each event
    } // while(1)

    close(epoll_fd);
    close(server_fd);
    free(event_array);

    return ret;
}
