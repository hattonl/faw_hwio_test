/*
 * network_test.c
 * switch_test run in server role
 *
 **/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "network_client.h"
#include "network_parse.h"
#include "network_server.h"
#include "network_test.h"
#include "network_thread.h"
#include "network_time.h"
#include "network_util.h"

#include "menu.h"
// #include "menu_utils.h"

#include "termios.h"

#define MAXLEN 15

char parabuf[MAXLEN];

int (*network_test_init)(struct network_test*);
int (*network_test_run)(struct network_test*);
int (*network_test_exit)(struct network_test*);

/*
    [terminal]
      +----------------[x direction] -------->
      |
      |
      |
      |
      |
    [y direction]
      |
      |
      |
      |
      \/
*/

void drawnetworktestmenu() {
    // char c;
    printf("\033[0;0H");
    printf(
        "+--------------------------[NetWork Test "
        "Menu]--------------------------+\n");
    printf(
        "| A -  Server/Client  Mode                            : Server        "
        "  |\n");
    printf(
        "| B -  Server port to listen on/connect to            : 60000         "
        "  |\n");
    printf(
        "| C -  Number of parallel client streams to run       : 1             "
        "  |\n");
    printf(
        "| D -  Window Size                                    :               "
        "  |\n");
    printf(
        "| E -  Package Size [KB]                              : 115200 8N1    "
        "  |\n");
    printf(
        "| F -  TCP/UDP                                        : TCP           "
        "  |\n");
    printf(
        "| G -  Time in seconds to transmit for                : 10            "
        "  |\n");
    printf(
        "| H -  Seconds between periodic throughput reports    : 1             "
        "  |\n");
    printf(
        "| I -  Function Mode/ Performance Mode                : Function Mode "
        "  |\n");
    printf(
        "| J -  Functional test mode transmit packet times     : 1000          "
        "  |\n");
    printf(
        "| K -  Host IP to connect                             : 1000          "
        "  |\n");
    printf(
        "|                                                                     "
        "  |\n");
    printf(
        "+---------------------------------------------------------------------"
        "--+\n");
    printf("     Change which setting?\n");
}

void network_test(void) {
    char c;
    int ret = 0;
    int breakwhile = 0;

    /* default para value */
    int isServer = 1;
    int port = DEFAULT_PORT;
    int threads = DEFAULT_THREAD;
    int windows_size = DEFAULT_WIN_SIZE;
    int pack_size = DEFAULT_PACK_SIZE;
    int isTcp = 1;
    int trans_time = DEFAULT_TIME;
    int interval = DEFAULT_INTERVAL;
    int isPerformanceMode = 1;
    int transmit_times = DEFAULT_TRANSMIT_TIMES;

    char host_ip[MAXLEN];

    struct network_test* test =
        (struct network_test*)malloc(sizeof(struct network_test));

    if (test == NULL) {
        perror("network test malloc error\n");
        return 0;
    }

    memcpy(host_ip, DEFAULT_HOST_IP, sizeof(DEFAULT_HOST_IP));

    system("clear");

    drawnetworktestmenu();

    printf("\033[s");
    /* draw para */
    if (isServer == 1) {
        puts_string(57, 2, "Server", MAXLEN);
    } else {
        puts_string(57, 2, "Client", MAXLEN);
    }

    puts_number(57, 3, port, MAXLEN);
    puts_number(57, 4, threads, MAXLEN);
    puts_number(57, 5, windows_size, MAXLEN);
    puts_number(57, 6, pack_size, MAXLEN);

    if (isTcp == 1) {
        puts_string(57, 7, "TCP", MAXLEN);
    } else {
        puts_string(57, 7, "UDP", MAXLEN);
    }

    puts_number(57, 8, trans_time, MAXLEN);
    puts_number(57, 9, interval, MAXLEN);

    if (isPerformanceMode == 1) {
        puts_string(57, 10, "Performance", MAXLEN);
    } else {
        puts_string(57, 10, "Function", MAXLEN);
    }

    puts_number(57, 11, transmit_times, MAXLEN);
    puts_string(57, 12, host_ip, MAXLEN);

    printf("\033[u");
    printf("\n");

    while (1) {
        if (breakwhile == 1) break;
        c = getch_noecho();
        // printf("get : %c \n", c);
        // printf("get : %c \n", c);
        switch (c) {
            case '\n':  //
                // return 0; // break to test
                breakwhile = 1;

                break;

            case 'a':  // " mode "

                isServer ^= 0x1;
                if (isServer == 1) {
                    puts_string(57, 2, "Server", MAXLEN);
                } else {
                    puts_string(57, 2, "Client", MAXLEN);
                }
                printf("\033[u");
                // printf("\n");
                break;

            case 'b':  // "port"
                       // can_test();
                move_cursor(57, 3);
                scanf("%d", &port);
                puts_number(57, 3, port, MAXLEN);
                printf("  |");
                printf("\033[K");
                printf("\033[u");

                while ((c = getchar()) != '\n' && c != EOF)
                    ;

                break;

            case 'c':  // "threads"

                move_cursor(57, 4);
                scanf("%d", &threads);
                puts_number(57, 4, threads, MAXLEN);
                printf("  |");
                printf("\033[K");
                printf("\033[u");

                while ((c = getchar()) != '\n' && c != EOF)
                    ;

                break;

            case 'd':  // "windows_size"

                move_cursor(57, 5);
                scanf("%d", &windows_size);
                puts_number(57, 5, windows_size, MAXLEN);
                printf("  |");
                printf("\033[K");
                printf("\033[u");

                while ((c = getchar()) != '\n' && c != EOF)
                    ;

                break;

            case 'e':  // "pack_size"

                move_cursor(57, 6);
                scanf("%d", &pack_size);
                puts_number(57, 6, pack_size, MAXLEN);
                printf("  |");
                printf("\033[K");
                printf("\033[u");

                while ((c = getchar()) != '\n' && c != EOF)
                    ;

                break;

            case 'f':
                isTcp ^= 0x1;
                if (isTcp == 1) {
                    puts_string(57, 7, "TCP", MAXLEN);
                } else {
                    puts_string(57, 7, "UDP", MAXLEN);
                }
                printf("\033[u");
                break;

            case 'g':  // "trans_time"

                move_cursor(57, 8);
                scanf("%d", &trans_time);
                puts_number(57, 8, trans_time, MAXLEN);
                printf("  |");
                printf("\033[K");
                printf("\033[u");

                while ((c = getchar()) != '\n' && c != EOF)
                    ;

                break;

            case 'h':  // "interval"

                move_cursor(57, 9);
                scanf("%d", &interval);
                puts_number(57, 9, interval, MAXLEN);
                printf("  |");
                printf("\033[K");
                printf("\033[u");

                while ((c = getchar()) != '\n' && c != EOF)
                    ;

                break;

            case 'i':
                isPerformanceMode ^= 0x1;
                if (isPerformanceMode == 1) {
                    puts_string(57, 10, "Performance", MAXLEN);
                } else {
                    puts_string(57, 10, "Function", MAXLEN);
                }
                printf("\033[u");
                break;

            case 'j':  // "transmit_times"

                move_cursor(57, 11);
                scanf("%d", &transmit_times);
                puts_number(57, 11, transmit_times, MAXLEN);
                printf("  |");
                printf("\033[K");
                printf("\033[u");

                while ((c = getchar()) != '\n' && c != EOF)
                    ;

                break;

            case 'k':  // host_ip

                move_cursor(57, 12);
                scanf("%s", &host_ip);
                puts_string(57, 12, host_ip, MAXLEN);
                printf("  |");
                printf("\033[K");
                printf("\033[u");

                while ((c = getchar()) != '\n' && c != EOF)
                    ;

                break;

            default:
                break;
        }

        // if (c == '\n') break;
        // getchar();
        // while((c=getchar()) != '\n' && c != EOF);
    }

    memset(test, 0, sizeof(struct network_test));

    test->isServer = isServer;
    test->interval = interval;
    test->pthread_num = threads;
    test->port = port;
    test->time = trans_time;
    test->udp = isTcp == 1 ? 0 : 1;
    test->windows_size = windows_size;
    test->package_size = pack_size;
    test->test_mode = isPerformanceMode;
    test->transmit_times = transmit_times;

    memcpy(test->ip, host_ip, sizeof(host_ip));

    if (test->isServer == 1) {
        printf("Runing on Server Mode: Ctrl + C to stop ...\n");
        network_test_init = network_test_server_init;
        network_test_run = network_test_server_run;
        network_test_exit = network_test_server_exit;
    } else {
        printf("Runing on Client Mode ...\n");
        network_test_init = network_test_client_init;
        network_test_run = network_test_client_run;
        network_test_exit = network_test_client_exit;
    }

    ret = network_test_init(test);
    if (ret == -1) {
        printf("network test init error.\n");
        goto OUT;
    }

    ret = network_test_run(test);
    if (ret == -1) {
        printf("network test run error.\n");
        goto OUT;
    }

    network_test_exit(test);

OUT:
    free(test);

    printf("NetWork Test Finished: Input RETURN to return\n");
    while ((c = getchar()) != '\n' && c != EOF)
        ;

    return;
}

int network_test_init_hook(struct network_test* test) { /*  */
    return 0;
}

/**
 * This function is used to check if the parameter is legal.
 */
int network_param_check(struct network_test* test) {
    /* TODO:  */
    return 0;
}
