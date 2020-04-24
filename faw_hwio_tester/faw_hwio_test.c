#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#include "menu_utils.h"

#include "network_test.h"
#include "camera_test.h"
#include "can_test.h"
#include "gpio_test.h"
#include "pmic_test.h"
#include "uart_test.h"

void drawmainmenu()
{
    // char c;
    printf("\033[0;0H");
    printf("+--------[Main Menu]--------+\n");
    printf("| A:           NetWork Test |\n");
    printf("| B:           CAN     Test |\n");
    printf("| C:           PMIC    Test |\n");
    printf("| D:           GPIO    Test |\n");
    printf("| E:           CAMERA  Test |\n");
    printf("| F:           UART    Test |\n");
    printf("+---------------------------+\n");
    printf("Select one to test: \n");
}

int main()
{
    char c;
    // len = 22
    // printf("len = %d", len);
    // struct winsize size;
    // ioctl(STDIN_FILENO, TIOCGWINSZ, &size);

    // printf("%s\n", title);
	// system("clear");
    // drawmainmenu();

    while (1)  {
        system("clear");
        drawmainmenu();
        c = getch_noecho();
        // printf("get : %c \n", c);
        // printf("get : %c \n", c);
        switch (c) {
        case '\n':
          return 0;
          break;

        case 'a':
          network_test();
          // printf("break from network test\n");
          break;

        case 'b':
          can_test();
          break;

        case 'c':
          pmic_test();
          break;

        case 'd':
          gpio_test();
          break;

        case 'e':
          camera_test();
          break;

        case 'f':
          uart_test();
          break;
        default:
          break;
        }
        // getchar();
    }
}
