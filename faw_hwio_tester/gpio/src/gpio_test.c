

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "gpio.h"
#include "gpio_test.h"

#define UTOFABUTEST 1

#define LED_ON 1
#define LED_OFF 0

// uto test led[416:419]
#if defined(UTOFABUTEST) && (UTOFABUTEST == 1)
#define LED0 416
#define LED1 417
#define LED2 418
#define LED3 419
#define KEY0 420
#define KEY1 421
#define KEY2 422
#define KEY3 423

#else
// for zcu104
#define LED0 508
#define LED1 509
#define LED2 510
#define LED3 511
#define KEY0 504
#define KEY1 505
#define KEY2 506
#define KEY3 507
#endif

static struct termios initial_settings, new_settings;
static int peek_character = -1;

void init_keyboard() {
    tcgetattr(0, &initial_settings);
    new_settings = initial_settings;
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_lflag &= ~ISIG;
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_settings);
}

void close_keyboard() { tcsetattr(0, TCSANOW, &initial_settings); }

int kbhit() {
    unsigned char ch;
    int nread;

    if (peek_character != -1) return 1;

    new_settings.c_cc[VMIN] = 0;
    tcsetattr(0, TCSANOW, &new_settings);
    nread = read(0, &ch, 1);
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &new_settings);

    if (nread == 1) {
        peek_character = ch;
        return 1;
    }
    return 0;
}

int readch() {
    char ch;

    if (peek_character != -1) {
        ch = peek_character;
        peek_character = -1;
        return ch;
    }

    read(0, &ch, 1);
    return ch;
}

int msleep(unsigned int times) { return usleep(times * 1000); }

void gpio_test() {
    /* dump menu */

    system("clear");

    init_keyboard();

    gpio_test_run();

    close_keyboard();

    printf("Input RETURN char to return\n");
    getchar();
}

void gpio_test_run() {
    char ch;
    printf("GPIO test running...\n");
    printf("when the button[0...3] is pressed, the led[0...3] is turned on.\n");

    if (gpio_open(LED0, DIRECTION_OUT) == -1) {
        printf("gpio open failed.\n");
        return;
    }
    if (gpio_open(LED1, DIRECTION_OUT) == -1) {
        printf("gpio open failed.\n");
        return;
    }

    if (gpio_open(LED2, DIRECTION_OUT) == -1) {
        printf("gpio open failed.\n");
        return;
    }

    if (gpio_open(LED3, DIRECTION_OUT) == -1) {
        printf("gpio open failed.\n");
        return;
    }

    if (gpio_open(KEY0, DIRECTION_IN) == -1) {
        printf("gpio open failed.\n");
        return;
    }

    if (gpio_open(KEY1, DIRECTION_IN) == -1) {
        printf("gpio open failed.\n");
        return;
    }

    if (gpio_open(KEY2, DIRECTION_IN) == -1) {
        printf("gpio open failed.\n");
        return;
    }

    if (gpio_open(KEY3, DIRECTION_IN) == -1) {
        printf("gpio open failed.\n");
        return;
    }

    printf("put 'q' to stop test.\n");

    while (1) {
        if (kbhit()) {
            ch = readch();
            if (ch == 'q') break;
        }

        if (gpio_read(KEY0))
            gpio_write(LED0, LED_ON);
        else
            gpio_write(LED0, LED_OFF);
        msleep(25);

        if (gpio_read(KEY1))
            gpio_write(LED1, LED_ON);
        else
            gpio_write(LED1, LED_OFF);
        msleep(25);

        if (gpio_read(KEY2))
            gpio_write(LED2, LED_ON);
        else
            gpio_write(LED2, LED_OFF);
        msleep(25);

        if (gpio_read(KEY3))
            gpio_write(LED3, LED_ON);
        else
            gpio_write(LED3, LED_OFF);
        msleep(25);
    }

    return;
}
