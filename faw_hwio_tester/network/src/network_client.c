/*
 * network_test.c
 * switch_test run in client role
 * 
**/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>  
#include <sys/socket.h>
#include <string.h>

#include "network_test.h"
#include "network_thread.h"
#include "network_time.h"
#include "network_parse.h"

int run_functional_test(struct network_test* test)
{
    int ret = 0;
    
    /* create each transmit thread */
    for (int i = 0; i < test->pthread_num; ++i) {
        ret = pthread_create(&test->transmit_thread[i].pthread_id, 
                             NULL, 
                             function_transmit_thread_func, 
                             &test->transmit_thread[i]);
        if  (ret != 0) {
            perror("create thread failed./r/n");
            return ret;
        }
    }

    /* wait all threads exit */
    for (int i = 0; i < test->pthread_num; ++i) {
        pthread_join(test->transmit_thread[i].pthread_id, NULL);
    }

    return ret;
}

/**
 * This function will create a statistics thread and some data transmit thread(s).
 * The transmit thread(s) is used ...
 * The statistics thread is used ...
 */
int run_performance_test(struct network_test* test)
{
    int ret = 0;

    /* time init */
    get_start_time(test);
    test->finished = 0;
    /**
     * create statistical threads 
     * that periodically count and report network performance 
     */
    ret = pthread_create(&test->statis_thread_id, 
                         NULL, 
                         statis_thread_func, 
                         test);
    if (ret != 0) {
        perror("create statistical thread thread error.\n");
        return ret;
    }
    printf("statistical thread create successful.\n");
    /* create a mutex protection shared variable*/
    ret = pthread_mutex_init(&test->test_mutex, NULL);
    if (ret != 0) {
        perror("create mutex error.\n");
        return ret;
    }

    /* create each transmit thread and mutex */
    for (int i = 0; i < test->pthread_num; ++i) {
        ret = pthread_create(&test->transmit_thread[i].pthread_id, 
                             NULL, 
                             transmit_thread_func, 
                             &test->transmit_thread[i]);
        if  (ret != 0) {
            printf("create thread failed erron= %d/n", errno);
            return ret;
        }
        ret = pthread_mutex_init(&test->transmit_thread[i].send_num_mutex, NULL);
        if (ret != 0) {
            printf("create mutex failed erron = %d/n", errno);
            return ret;
        }
        printf("transimit thread [%d] create successful.\n", i);
    }
    
    /* wait all threads exit */
    pthread_join(test->statis_thread_id, NULL);

    for (int i = 0; i < test->pthread_num; ++i) {
        pthread_join(test->transmit_thread[i].pthread_id, NULL);
    }

    printf("all thread exit. \n");

    return ret;
}

/**
 * This function creates connection to the server for each thread.
 */
int connect_server(struct network_test* test)
{
    int ret = 0;
    struct sockaddr_in sockaddr;
    int socket_type;
    /* sockaddr init */
    memset(&sockaddr, 0, sizeof(sockaddr));
    /* TODO: modified to variable parameters */
    sockaddr.sin_family = AF_INET;      
    sockaddr.sin_port   = htons(test->port);

    if ((ret = inet_pton(AF_INET, test->ip, &sockaddr.sin_addr)) < 0) {
        perror("inet_pton error.\n");
        return -1;
    }

    if (test->udp == 1) {
        socket_type = SOCK_DGRAM;
    } else {
        socket_type = SOCK_STREAM;
    }
    /* create socket fd and connect to server for each thread */
    for (int i=0; i < test->pthread_num; ++i) {
        int socketfd = socket(AF_INET, socket_type, 0);
        if (socketfd == -1) {
            perror("socket build faild\n");
            return -1;
        }
        if( (connect(socketfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr))) < 0 ) {
            printf("Thread [%d]: ", i);
            perror("connect to server error.\n");
            return -1;
        }
        test->transmit_thread[i].socket_fd = socketfd;
    }
    
    return 0;
}

int close_connection(struct network_test *test)
{
    for (int i = 0; i < test->pthread_num; ++i) {
        close(test->transmit_thread[i].socket_fd);
    }
    return 0;
}



/**
 * This function is responsible for parsing parameters, 
 * verifying the legality of the parameters 
 * and the memory space required by the application.
 */
int network_test_client_init(struct network_test* test) 
{
    int ret = 0;

    /* zeros memory */
    // memset(test, 0, sizeof(struct network_test));

    // network_test_init_hook(test);
    // param_parse(argc, argv, test);

    /* test param check */
    // ret = network_param_check(test);
    // if (ret < 0) {
    //     return ret;
    // }

    /* init buff size */
    test->send_buff_size = test->package_size*1024*(sizeof(char));


    /* malloc thread attruct memory */
    test->transmit_thread = (struct transmit_thread_attr *) \
        malloc(sizeof(struct transmit_thread_attr) * test->pthread_num);

    if (test->transmit_thread == NULL) {
        perror("thread attr malloc error.\n");
        return -1;
    }

    /* init thread attr */
    memset(test->transmit_thread, 0, \
        sizeof(struct transmit_thread_attr) * test->pthread_num);
    for (int i = 0; i < test->pthread_num; ++i) {
        test->transmit_thread[i].local_id = i;
        test->transmit_thread[i].test_inst = test;
    }

    /**
     * If the client is running in performance test mode, 
     * it only sends data and does not accept data, 
     * so all threads share one send buffer.
     */
    if (test->test_mode == PERFORMANCE_MODE) {
        test->send_buff = (int8_t *)malloc(test->send_buff_size);
        if (test->send_buff == NULL) {
            perror("send buff malloc error.\n");
            return -1;
        }
        for (int i = 0; i < test->pthread_num; ++i) {
            test->transmit_thread[i].send_buff = test->send_buff;
            test->transmit_thread[i].send_buff_size = test->send_buff_size;
        }
    }
    /**
     * If the client is running in functional test mode, 
     * each thread will apply its own send buffer and receive buffer. 
     */
    else if (test->test_mode == FUNCTION_MODE) {
        int buff_size = test->package_size*1024*(sizeof(char));
        for (int i = 0; i < test->pthread_num; ++i) {
            test->transmit_thread[i].send_buff_size = buff_size;
            /* malloc send buff */
            test->transmit_thread[i].send_buff = (int8_t *)malloc(buff_size);
            if (test->transmit_thread[i].send_buff == NULL) {
                perror("function mode send buff malloc error.\n");
                return -1;
            }
            /* malloc recv buff */
            test->transmit_thread[i].recv_buff = (int8_t *)malloc(buff_size);
            if (test->transmit_thread[i].recv_buff == NULL) {
                perror("function mode recv buff malloc error.\n");
                return -1;
            }
        }
    }

    return 0;
}

/**
 * This function is used to free up memory space
 */
int network_test_client_exit(struct network_test* test)
{
    /* free buff memory */
    if (test->test_mode == PERFORMANCE_MODE) {
        if (test->send_buff != NULL) {
            free(test->send_buff);
        }
    } else if (test->test_mode == FUNCTION_MODE) {
        for (int i = 0; i < test->pthread_num; ++i) {
            if (test->transmit_thread[i].send_buff != NULL) {
                free(test->transmit_thread[i].send_buff);
            }
            if (test->transmit_thread[i].recv_buff != NULL) {
                free(test->transmit_thread[i].recv_buff);
            }
        }
    }

    /* free thread memory */
    if (test->transmit_thread != NULL) {
        free(test->transmit_thread);
    }
    return 0;
}


int network_test_client_run(struct network_test* test)
{
    int ret = 0;
    /* socket, bind, and connect */
    if (connect_server(test) == -1) {
        printf("connect to server faild\n");
        return -1;
    }

    /* select test mode and test */
    if (test->test_mode == PERFORMANCE_MODE) {
        printf("Run in performance test mode\n");  /* TODO: change to debug printf */
        run_performance_test(test);
    } else if (test->test_mode == FUNCTION_MODE) {
        printf("Run in functional test mode\n");
        run_functional_test(test);
    } else {
        printf("ERROR: unknow test mode\n");
        ret = -1;
    }

    /* close */
    close_connection(test);

    return ret;
}
