#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "gpio.h"

/* export gpio */
int gpio_open(int pin_num, int direction)
{
    int ret = 0;
    int exportfd = 0;
    int fd = 0;
    char pin[10];
    char gpio_path[50];

    sprintf(pin, "%d", pin_num);
    exportfd = open("/sys/class/gpio/export", O_WRONLY);
    if (exportfd < 0) {
        printf("Cannot open GPIO %d to export it\n", pin_num);
        return -1;
    }

    write(exportfd, pin, strlen(pin) + 1);
    close(exportfd);

    sprintf(gpio_path, "/sys/class/gpio/gpio%d/direction", pin_num);

    // printf("%s\n", gpio_path);

    fd = open(gpio_path, O_RDWR);
    if (fd < 0) {
        printf("Cannot open GPIO direction it\n");
        return -1;
    }

    if (direction == DIRECTION_IN) {
        write(fd, "in", 3);
    } else if (direction == DIRECTION_OUT) {
        write(fd, "out", 4);
    } else {
        printf("direction error\n");
        ret = -1;
    }
    close(fd);

    return ret;
}

int gpio_read(int pin_num)
{
    int fd;
    char gpio_path[50];
    char readline[2];
    sprintf(gpio_path, "/sys/class/gpio/gpio%d/value", pin_num);
    fd = open(gpio_path, O_RDWR);

    if (fd < 0) {
        printf("gpio read: cannot open GPIO val it\n");
        return -1;
    }

    read(fd, readline, 2);
    close(fd);
    if (readline[0] == '1') return 1;

    return 0;
}

int gpio_write(int pin_num, int val)
{
    int fd;
    char gpio_path[50];
    char readline[2];

    if (val == 0) {
        readline[0] = '0';
    } else {
        readline[0] = '1';
    }
    readline[1] = 0;

    sprintf(gpio_path, "/sys/class/gpio/gpio%d/value", pin_num);
    fd = open(gpio_path, O_RDWR);

    if (fd < 0) {
        printf("gpio write: cannot open GPIO val it\n");
        return -1;
    }

    write(fd, readline, 2);

    close(fd);

    return val;
}

