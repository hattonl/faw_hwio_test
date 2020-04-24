

#include "network_thread.h"
#include "network_test.h"
#include "network_time.h"
#include "network_util.h"

#include <pthread.h>
#include <stdio.h>

void *server_thread_func(void *arg) {
    int ret = 0;
    struct connect_thread_attr *connect_thread =
        (struct connect_thread_attr *)arg;
    int local_id = connect_thread->local_id;
    int buff_size = connect_thread->test->send_buff_size;
    char *buf = (char *)malloc(sizeof(char) * buff_size);

    if (buf == NULL) {
        printf("Thread: buff malloc error.\n");
        goto OUT;
    }

    while (1) {
        ret = recv(connect_thread->socket_fd, buf, buff_size, 0);
        if (ret <= 0) {
            perror("recv data error.\n");
            close(connect_thread->socket_fd);
            break;
        }

        if (connect_thread->test->test_mode == FUNCTION_MODE) {
            ret = send(connect_thread->socket_fd, buf, (size_t)ret, 0);
            if (ret == -1) {
                perror("send failed.\n");
            }
        }
    }

OUT:
    if (buf != NULL) {
        free(buf);
    }
    close(connect_thread->socket_fd);
    printf("thread exit.\n");
    connect_thread->test->server.used[local_id] = 0;

    return NULL;
}

void *statis_thread_func(void *arg) {
    struct network_test *test = (struct network_test *)arg;
    int sleep_time = test->interval;
    int finished = 0;

    /* TODO: change to debug printf */
    printf("Run on statis thread \n");

    /* time init */
    get_start_time(test);

    do {
        get_interval_start_time(test);
        sleep(sleep_time);
        get_elapsed_time(test);

        /* performance report */
        periodic_report(test);

        /* if the measurement end time has been reache? 0: No, 1: Yes */
        finished = transmit_time_finished(test);

    } while (finished == 0);

    /* sum up the last period */
    get_interval_start_time(test);

    /* finish all thread */
    /*
        for (int i = 0; i < test->pthread_num; ++i) {
            test->transmit_thread[i].finished = 1; // need mutex ?
        }
    */
    test->finished = 1;

    summary_report(test);

    return (void *) NULL;
}

void *transmit_thread_func(void *arg) {
    int ret = 0;
    struct transmit_thread_attr *args = (struct transmit_thread_attr *)arg;
    int finished = args->test_inst->finished;
    // int finished = args->finished;

    /* TODO: change to debug printf */
    printf("Run on transmit thread %ld \n", args->pthread_id);

    printf("finished == %d\n", finished);
    while (finished == 0) {
        ret = NSend(args->socket_fd, args->send_buff, args->send_buff_size);
        if (ret < 0) {
            perror("send mesage error.\n");
            return (void *)0;
        }

        /* sum up send num and read the finish flag */
        pthread_mutex_lock(&args->send_num_mutex);
        // printf("Thread[%d] lock mutex .\n", args->local_id);
        args->interval_send_num += ret;
        pthread_mutex_unlock(&args->send_num_mutex);
        // printf("Thread[%d] unlock mutex .\n", args->local_id);
        finished = args->test_inst->finished;
    }

    /* TODO: change to debug printf */
    printf("Thread[%d] exit.\n", args->local_id);
    return NULL;
}

void *function_transmit_thread_func(void *arg) {
    /* param init */
    struct transmit_thread_attr *args = (struct transmit_thread_attr *)arg;
    int ret = 0;
    int socketfd = args->socket_fd;
    int8_t *send_buff = args->send_buff;
    int8_t *recv_buff = args->recv_buff;
    uint32_t send_buff_size = args->send_buff_size;
    uint32_t size_in_8bytes = send_buff_size / 8;
    uint64_t *send_buff_8bytes = (uint64_t *)send_buff;
    uint64_t *recv_buff_8bytes = (uint64_t *)recv_buff;
    uint32_t transmit_times = args->test_inst->transmit_times;

    printf("in function_transmit_thread_func \n");

    for (int i = 0; i < transmit_times; ++i) {
        /* feed the send buff */
        uint32_t data = random_data();
        for (int j = 0; j < size_in_8bytes; ++j) {
            send_buff_8bytes[j] = data++;
        }

        // send this buff
        ret = NSend(socketfd, send_buff, send_buff_size);
        if (ret < 0) {
            perror("send mes error.\n");
            return NULL;
        }

        ret = NRead(socketfd, recv_buff, send_buff_size);
        if (ret < 0) {
            perror("recv mes error.\n");
            return NULL;
        }

        ret = check_buff_8bytes(send_buff_8bytes, recv_buff_8bytes,
                                size_in_8bytes);

        if (ret != size_in_8bytes) {
            /* error: log report */
            int start = ret % 10;
            printf("Part of Data We Send: \n");
            for (int i = start; i < start + 10 && i < size_in_8bytes; ++i) {
                printf("  %ld", send_buff_8bytes[i]);
            }
            printf("\n But Recv: \n");

            for (int i = start; i < start + 10 && i < size_in_8bytes; ++i) {
                printf("  %ld", recv_buff_8bytes[i]);
            }

            printf("\n[Thread %d][%3d][FAIL]: TEST FAIL\n", args->local_id,
                   i);
            return NULL;
        }

        /* TODO: change to debug printf */
        printf(
            "[Thread %d][%3d][SUCCESSFUL]: %d Bytes have beed send with the "
            "first data: %ld\n",
            args->local_id, i, send_buff_size, send_buff_8bytes[0]);
    }
    return NULL;
}
