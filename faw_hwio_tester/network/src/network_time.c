#include "network_time.h"

#include <sys/time.h>

void get_elapsed_time(struct network_test* test)
{
    struct timeval *time1 = &test->interval_start_time;
    struct timeval *time2 = &test->current_time;
    struct timeval *timed = &test->elapsed_time;

    gettimeofday(time2, NULL);

    /* calculate elapsed time */
    if (time2->tv_usec < time1->tv_usec) {
        time2->tv_usec += 1000000;
        time2->tv_sec  -= 1;
    }
    timed->tv_usec = time2->tv_usec - time1->tv_usec;
    timed->tv_sec  = time2->tv_sec -  time1->tv_sec;
}

void get_start_time(struct network_test* test)
{
    gettimeofday(&test->start_time, NULL);

    test->start_time_lf = (double)test->start_time.tv_sec
        + (test->start_time.tv_usec/1000.0f/1000.0f);

    test->end_time.tv_usec = test->start_time.tv_usec;
    test->end_time.tv_sec  = test->start_time.tv_sec + test->time;
}

void get_interval_start_time(struct network_test* test)
{
    gettimeofday(&test->interval_start_time, NULL);
    test->elapsed_time.tv_sec  = 0;
    test->elapsed_time.tv_usec = 0;

    for (int i = 0; i < test->pthread_num; ++i) {
        pthread_mutex_lock(&test->transmit_thread[i].send_num_mutex);
        test->interval_send_sum += test->transmit_thread[i].interval_send_num;
        test->transmit_thread[i].interval_send_num = 0;
        pthread_mutex_unlock(&test->transmit_thread[i].send_num_mutex);
    }

    test->send_sum += test->interval_send_sum;
    test->interval_send_sum = 0;
}

int transmit_time_finished(struct network_test* test)
{
    int ret = 0;
    struct timeval *cur = &test->current_time;
    struct timeval *end = &test->end_time;
    gettimeofday(cur, NULL);

    if (end->tv_sec < cur->tv_sec || 
        (end->tv_sec == cur->tv_sec && end->tv_usec <= cur->tv_usec) )
    {
        // update end_time to cur_time
        end->tv_sec = cur->tv_sec;
        end->tv_usec = cur->tv_usec;
        ret = 1;
    }
    return ret;
}

int reached_time_interval(struct network_test* test)
{
    int ret = 0;
    if (test->elapsed_time.tv_sec >= test->interval)
        ret = 1;
    return ret;
}
