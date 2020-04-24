
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>

#include <sys/fcntl.h>
#include <sys/select.h>

#include "menu.h"
#include "uart_test.h"

#define MAXLEN 17

// static char parabuf[MAXLEN];

#define SERVER_MODE 0
#define CLINET_MODE 1

#define YES 1
#define NONE 0

#define DATA_BITS_5 5
#define DATA_BITS_6 6
#define DATA_BITS_7 7
#define DATA_BITS_8 8

#define STOP_BITS_1 1
#define STOP_BITS_2 2

#define PARITY_NONE NONE
#define PARITY_EVEN 1
#define PARITY_ODD 2
#define PARITY_MARK 3
#define PARITY_SPACE 4

#define DATA_MODE_NONE NONE
#define DATA_MODE_RAW 1
#define DATA_MODE_ASCII 2

#define DEFAULT_WRITE_SIZE 1024

static long long int _write_count = 0;
static long long int _read_count = 0;
static long long int _error_count = 0;

static char *_write_data;
static ssize_t _write_size;
static unsigned char _write_count_value = 0;
static unsigned char _read_count_value = 0;
static running = 1;

static void sigterm(int signo) { running = 0; }

void drawuarttestmenu(void) {
    // char c;
    printf("\033[0;0H");
    printf("+-----------------[Uart Test Menu]-----------------+\n");
    printf("| U -  Serial Device            : /dev/ttyPS1      |\n");
    printf("| M -  Server/ Client Mode      : Server           |\n");
    printf("| B -  Bitrate                  : 9600             |\n");
    printf("| D -  DataBits (5, 6, 7, 8)    : 8                |\n");
    printf("| S -  StopBits (1, 2)          : 1                |\n");
    printf("| P -  Parity                   : None             |\n");
    printf("| C -  Software Flow Control    : None             |\n");
    printf("| F -  Hardware Flow Control    : None             |\n");
    printf("| R -  Data Show Mode           : None             |\n");
    printf("|                                                  |\n");
    printf("+--------------------------------------------------+\n");
    printf("     Change which setting?\n");
}

// converts integer baud to Linux define
static int get_baud(int baud) {
    switch (baud) {
        case 9600:
            return B9600;
        case 19200:
            return B19200;
        case 38400:
            return B38400;
        case 57600:
            return B57600;
        case 115200:
            return B115200;
        case 230400:
            return B230400;
        default:
            return -1;
    }
}

int SetSerialPort(int fd, int iSerialPortSpeed, int iBits, int iParityCheck,
                  int iStop, int SoftwareFlowControl, int HardwareFlowControl) {
    struct termios TNewSerialPortParam;
    struct termios TOldSerialPortParam;

    if (0 != tcgetattr(fd, &TOldSerialPortParam)) {
        perror("SetupSerial 1");
        return (-1);
    }

    bzero(&TNewSerialPortParam, sizeof(TNewSerialPortParam));

    TNewSerialPortParam.c_cflag |= CLOCAL | CREAD;
    TNewSerialPortParam.c_cflag &= ~CSIZE;

    switch (iBits) {
        case 7:
            TNewSerialPortParam.c_cflag |= CS7;
            break;
        case 8:
            TNewSerialPortParam.c_cflag |= CS8;
            break;
        default:
            TNewSerialPortParam.c_cflag |= CS8;
            break;
    }

    cfsetispeed(&TNewSerialPortParam, iSerialPortSpeed);
    cfsetospeed(&TNewSerialPortParam, iSerialPortSpeed);

    switch (iParityCheck) {
        case PARITY_ODD:
            TNewSerialPortParam.c_cflag |= PARENB;
            TNewSerialPortParam.c_cflag |= PARODD;
            TNewSerialPortParam.c_iflag |= (INPCK | ISTRIP);
            break;
        case PARITY_EVEN:
            TNewSerialPortParam.c_iflag |= (INPCK | ISTRIP);
            TNewSerialPortParam.c_cflag |= PARENB;
            TNewSerialPortParam.c_cflag &= ~PARODD;
            break;
        case PARITY_NONE:
            TNewSerialPortParam.c_cflag &= ~PARENB;
            break;
        default:
            TNewSerialPortParam.c_cflag &= ~PARENB;
            break;
    }  // end of switch ( cParityCheck )

    switch (iStop) {
        case 2:
            TNewSerialPortParam.c_cflag |= CSTOPB;
            break;
        case 1:
            TNewSerialPortParam.c_cflag &= ~CSTOPB;
            break;
        default:
            TNewSerialPortParam.c_cflag &= ~CSTOPB;
            break;
    }

    if (SoftwareFlowControl == NONE && HardwareFlowControl == NONE) {
        TNewSerialPortParam.c_cflag &= ~CRTSCTS;
        TNewSerialPortParam.c_iflag &= ~(IXON | IXOFF | IXANY);
    } else if (HardwareFlowControl == YES) {
        TNewSerialPortParam.c_cflag |= CRTSCTS;
        TNewSerialPortParam.c_iflag &= ~(IXON | IXOFF | IXANY);
    } else if (SoftwareFlowControl == YES) {
        TNewSerialPortParam.c_cflag &= ~CRTSCTS;
        TNewSerialPortParam.c_iflag |= (IXON | IXOFF | IXANY);
    } else {
        TNewSerialPortParam.c_cflag &= ~CRTSCTS;
        TNewSerialPortParam.c_iflag &= ~(IXON | IXOFF | IXANY);
    }

    TNewSerialPortParam.c_cc[VTIME] = 0;
    TNewSerialPortParam.c_cc[VMIN] = 0;

    tcflush(fd, TCIFLUSH);

    if (0 != tcsetattr(fd, TCSANOW, &TNewSerialPortParam)) {
        perror("com set error");
        return (-1);
    }

    return 0;
}

static unsigned char next_count_value(unsigned char c) {
    if (++c == 127) c = 32;
    return c;
}

static void process_write_data(int fd) {
    ssize_t count = 0;
    int repeat = 1;

    do {
        ssize_t i;
        for (i = 0; i < _write_size; i++) {
            _write_data[i] = _write_count_value;
            _write_count_value = next_count_value(_write_count_value);
        }

        ssize_t c = write(fd, _write_data, _write_size);

        if (c < 0) {
            if (errno != EAGAIN) {
                printf("write failed - errno=%d (%s)\n", errno,
                       strerror(errno));
            }
            c = 0;
        }

        count += c;

        if (c < _write_size) {
            _write_count_value = _write_data[c];
            repeat = 0;
        }
    } while (repeat);

    _write_count += count;

    printf("wrote %zd bytes\n", count);
}

static void dump_data(unsigned char *b, int count) {
    printf("%i bytes: ", count);
    int i;
    for (i = 0; i < count; i++) {
        printf("%02x ", b[i]);
    }

    printf("\n");
}

static void dump_data_ascii(unsigned char *b, int count) {
    int i;
    for (i = 0; i < count; i++) {
        printf("%c", b[i]);
    }
}

static void process_read_data(int fd, int datamode) {
    unsigned char rb[1024];
    int c = read(fd, &rb, sizeof(rb));
    if (c > 0) {
        if (datamode != DATA_MODE_NONE) {
            if (datamode == DATA_MODE_ASCII)
                dump_data_ascii(rb, c);
            else  // datamode == DATA_MODE_RAW
                dump_data(rb, c);
        }
        // verify read count is incrementing
        int i;
        for (i = 0; i < c; i++) {
            if (rb[i] != _read_count_value) {
                printf("Error, count: %lld, expected %02x, got %02x\n",
                       _read_count + i, _read_count_value, rb[i]);
                _error_count++;
                _read_count_value = rb[i];
            }
            _read_count_value = next_count_value(_read_count_value);
        }
        _read_count += c;
    }
}

void uart_test(void) {
    int breakwhile = 0;
    char c;
    /* default var */
    char serialdevice[MAXLEN];
    int testmode = SERVER_MODE;
    int bitrate = 9600;
    int databits = DATA_BITS_8;  // select from 5, 6, 7, 8
    int stopbits = STOP_BITS_1;  // 1 or 2
    int parity = NONE;           // None Even Odd Mark Space
    int swflowctl = NONE;
    int hwfolwctl = NONE;
    int datamode = NONE;

    memcpy(serialdevice, "/dev/ttyPS1", sizeof("/dev/ttyPS1"));

    drawuarttestmenu();
    printf("\033[s");

    while (1) {
        if (breakwhile == 1) break;

        c = getch_noecho();

        switch (c) {
            case '\n':
                breakwhile = 1;
                break;
            // U M B D S P C F R
            case 'u':  // uart device
                move_cursor(35, 2);
                scanf("%s", serialdevice);
                puts_string(35, 2, serialdevice, MAXLEN);
                printf(" |");
                printf("\033[K");
                printf("\033[u");
                while ((c = getchar()) != '\n' && c != EOF)
                    ;

                break;
            case 'm':
                testmode ^= 0x01;
                if (testmode == SERVER_MODE) {
                    puts_string(35, 3, "Server", MAXLEN);
                } else {
                    puts_string(35, 3, "Client", MAXLEN);
                }
                printf("\033[u");
                break;
            case 'b':
                move_cursor(35, 4);
                scanf("%d", &bitrate);
                puts_number(35, 4, bitrate, MAXLEN);
                printf(" |");
                printf("\033[K");
                printf("\033[u");
                while ((c = getchar()) != '\n' && c != EOF) {}
                break;
            case 'd':
                if (databits == DATA_BITS_8) {
                    databits = DATA_BITS_5;
                } else {
                    databits++;
                }
                puts_number(35, 5, databits, MAXLEN);
                printf("\033[u");
                break;

            case 's':
                if (stopbits == STOP_BITS_2) {
                    stopbits--;
                } else {
                    stopbits++;
                }
                puts_number(35, 6, stopbits, MAXLEN);
                printf("\033[u");
                break;

            case 'p':
                if (parity == PARITY_SPACE) {
                    parity = PARITY_NONE;
                } else {
                    parity++;
                }

                if (parity == PARITY_NONE)
                    puts_string(35, 7, "None", MAXLEN);
                else if (parity == PARITY_EVEN)
                    puts_string(35, 7, "Even", MAXLEN);
                else if (parity == PARITY_ODD)
                    puts_string(35, 7, "Odd", MAXLEN);
                else if (parity == PARITY_MARK)
                    puts_string(35, 7, "Mark", MAXLEN);
                else if (parity == PARITY_SPACE)
                    puts_string(35, 7, "Space", MAXLEN);
                else
                    puts_string(35, 7, "????", MAXLEN);

                printf("\033[u");

                break;
            case 'c':  // software flow control
                swflowctl ^= 0x01;
                if (swflowctl == NONE) {
                    puts_string(35, 8, "None", MAXLEN);
                } else {
                    puts_string(35, 8, "Yes", MAXLEN);
                }
                printf("\033[u");
                break;

            case 'f':  // hardware flow control
                hwfolwctl ^= 0x01;
                if (hwfolwctl == NONE) {
                    puts_string(35, 9, "None", MAXLEN);
                } else {
                    puts_string(35, 9, "Yes", MAXLEN);
                }
                printf("\033[u");
                break;

            case 'r':
                if (datamode == DATA_MODE_ASCII) {
                    datamode = DATA_MODE_NONE;
                } else {
                    datamode++;
                }

                if (datamode == DATA_MODE_NONE) {
                    puts_string(35, 10, "None", MAXLEN);
                } else if (datamode == DATA_MODE_RAW) {
                    puts_string(35, 10, "RAW", MAXLEN);
                } else if (datamode == DATA_MODE_ASCII) {
                    puts_string(35, 10, "ASCII", MAXLEN);
                }
                printf("\033[u");

                break;

            default:
                break;
        }  // switch(c)
    }      // while (1)

    // debug dump
    // printf("serialdevice = %s\n", serialdevice);
    // printf("testmode     = %d\n", testmode);
    // printf("bitrate      = %d\n", bitrate);
    // printf("databits     = %d\n", databits);
    // printf("stopbits     = %d\n", stopbits);
    // printf("parity       = %d\n", parity);
    // printf("swflowctl    = %d\n", swflowctl);
    // printf("hwfolwctl    = %d\n", hwfolwctl);
    // printf("datamode     = %d\n", datamode);
    //

    running = 1;
    signal(SIGTERM, sigterm);
    signal(SIGHUP, sigterm);
    signal(SIGINT, sigterm);

    // dump some notice information

    int bound = get_baud(bitrate);
    if (bound == -1) {
        printf("The bitrate %d not support using B9600\n", bitrate);
        bound = B9600;
    }

    int comfd = open(serialdevice, O_RDWR | O_NOCTTY | O_NDELAY);
    if (comfd == -1) {
        printf("Can't Open Serial Port %s\n", serialdevice);
        goto END;
    }

    if (SetSerialPort(comfd, bound, databits, parity, stopbits, swflowctl,
                      hwfolwctl) == -1) {
        printf("Set serial port error.\n");
    }

    _write_size = DEFAULT_WRITE_SIZE;
    _write_data = (char *)malloc(_write_size);

    if (_write_data == NULL) {
        printf("write data buff malloc error.\n");
        goto END;
    }

    _write_count_value = 32;
    _read_count_value = 32;

    if (testmode == CLINET_MODE) {
        while (running) {
            process_write_data(comfd);
            sleep(2);
        }
    } else {  // testmode == SERVER_MODE
        printf("receiving string:\n");
        int rxlen = 0;
        int res = 0;
        while (running) {
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(comfd, &rfds);

            res = select(comfd + 1, &rfds, NULL, NULL, NULL);
            if (res < 0) {
                printf("error select,exit\n");
                break;
            } else {
                if (FD_ISSET(comfd, &rfds)) {
                    process_read_data(comfd, datamode);
                }
            }
        }
    }
END:

    if (comfd != -1) close(comfd);
    if (_write_data != NULL) free(_write_data);

    printf("%s: count for this session: rx=%lld, tx=%lld, rx err=%lld\n",
           serialdevice, _read_count, _write_count, _error_count);

    printf("Test Finished Input Any Char To Return.");
    getch_noecho();

    // while (1);
}