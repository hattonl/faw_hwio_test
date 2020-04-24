#include <stdio.h>

#include "network_test.h"
#include "network_util.h"

int apply_connect_thread(struct server_attr *server) {
    int i = 0;
    for (; i < server->max_connect; ++i) {
        if (server->used[i] == 0) break;
    }
    if (i == server->max_connect) return -1;
    memset(&server->connect_thread[i], 0, sizeof(struct connect_thread_attr));
    server->connect_thread[i].local_id = i;

    server->used[i] = 1;

    return i;
}

void periodic_report(struct network_test *test) {
    // cal speed
    // size / time
    double start_time = (double)test->interval_start_time.tv_sec +
                        (test->interval_start_time.tv_usec / 1000.0f / 1000.0f);
    double end_time = (double)test->current_time.tv_sec +
                      (test->current_time.tv_usec / 1000.0f / 1000.0f);

    start_time -= test->start_time_lf;
    end_time -= test->start_time_lf;
    // double all_start_time = (double)test->start_time.tv_sec
    //     + (test->start_time.tv_usec/1000.0f/1000.0f);
    // test->start_time.tv_sec
    // bytes / 1024 = k
    // k / 1024 = m
    // 1024 = G
    uint64_t interval_send_bytes = 0;
    double size_gbytes = 0.0;
    double speed_gbitsec = 0.0;

    printf(
        "------------------------------------------------------------------\n");
    for (int i = 0; i < test->pthread_num; ++i) {
        pthread_mutex_lock(&test->transmit_thread[i].send_num_mutex);
        interval_send_bytes = test->transmit_thread[i].interval_send_num;
        pthread_mutex_unlock(&test->transmit_thread[i].send_num_mutex);
        size_gbytes = (double)interval_send_bytes / 1024 / 1024 / 1024;
        speed_gbitsec = size_gbytes * 8 / (end_time - start_time);
        printf("Thread[%2d]: ", i);
        // printf("Time Interval [%.2f, %.2f] Sec", start_time - all_start_time,
        // end_time - all_start_time);
        printf("Time [%.2f, %.2f] Sec", start_time, end_time);
        printf("\tTransfer %.2f GBytes, %lu Bytes", size_gbytes,
               interval_send_bytes);
        printf("\tBitrate %.2f Gbits/sec\n", speed_gbitsec);
    }
}

void summary_report(struct network_test *test) {
    // test->send_num += test->interval_send_sum;
    double start_time = (double)test->start_time.tv_sec +
                        (test->start_time.tv_usec / 1000.0f / 1000.0f);
    double end_time = (double)test->end_time.tv_sec +
                      (test->end_time.tv_usec / 1000.0f / 1000.0f);

    double size_gbytes = (double)test->send_sum / 1024 / 1024 / 1024;
    double speed_gbitsec = size_gbytes * 8 / (end_time - start_time);

    printf("SUMMARY:\n");
    printf("Time Interval [%.2f, %.2f] Sec", start_time, end_time);
    printf("\tTransfer %.2f GBytes", size_gbytes);
    printf("\tBitrate %.2f Gbits/sec\n", speed_gbitsec);
}

// 发送40Gbits 数据测试
// 40 *
// (128 * 1024)(byte)  = 1024 * 1024 bits = 1024 Kbits = 1MBits
// 40 * 1024 * (128 * 1024)(byte) = 40 * Gbits
// 发送40 * 1024 次
// pthread_t statis_thread_id;
// pthread_mutex_t test_mutex;

ssize_t NSend(int fd, const void *sendline, size_t n) {
    int leftN = n;
    int sendN = 0;
    char *sendPoint = (char *)sendline;
    while (leftN > 0) {
        sendN = send(fd, sendPoint, leftN, 0);
        if (sendN < 0) return sendN;
        leftN -= sendN;
        sendPoint = sendPoint + sendN;
    }
    return n;
}

ssize_t NRead(int fd, const void *readline, size_t n) {
    int leftN = n;
    int readN = 0;
    char *readPoint = (char *)readline;
    while (leftN > 0) {
        readN = recv(fd, readPoint, leftN, 0);
        if (readN < 0) return readN;
        leftN -= readN;
        readPoint = readPoint + readN;
    }
    return n;
}

uint32_t random_data(void) {
    FILE *fs_p = NULL;
    unsigned int seed = 0;
    fs_p = fopen("/dev/urandom", "r");
    if (NULL == fs_p) {
        printf("Can not open /dev/urandom\n");
        return -1;
    } else {
        (void)fread(&seed, sizeof(int), 1,
                    fs_p);  // obtain one unsigned int data
        fclose(fs_p);
    }
    srand(seed);
    return rand();
}

// if i == size  success
// else false
int check_buff_8bytes(uint64_t *buff1, uint64_t *buff2, int size) {
    int i = 0;
    // int err = 0;
    for (; i < size; ++i) {
        if (buff1[i] != buff2[i]) break;
    }
    return i;
}
