
#include <termios.h>
#include <stdio.h>
#include <string.h>

#include "menu.h"


char getch_noecho()
{
	char input;
	struct termios save, current;
	tcgetattr(0, &save);
	current = save;
	current.c_lflag &= ~ICANON;
	current.c_lflag &= ~ECHO;
	current.c_cc[VMIN] = 1;
	current.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW,  &current);
	input = getchar();

	tcsetattr(0, TCSANOW, &save);
  	return input;
}


void parapadding(char *parabuf, int len)
{
    int local_len = strlen(parabuf);
    for (int i = local_len; i < len; ++i) {
        parabuf[i] = ' ';
    }
    parabuf[len-1] = 0;
}

void move_cursor(int x, int y)
{
    printf("\033[%d;%dH", y, x);
}



void puts_string(int x, int y, char *ch, int maxlen)
{
    char parabuf[50];
    int chlen = strlen(ch);
    int cpylen = chlen > maxlen? maxlen: chlen;

    // printf("chlen = %d, cpylen = %d\n", chlen, cpylen);
    memcpy(parabuf, ch, cpylen);
    parabuf[cpylen] = 0;
    parapadding(parabuf, maxlen);
    move_cursor(x, y);
    printf("%s", parabuf);
}

void puts_number(int x, int y, int number, int maxlen)
{
    char parabuf[50];
    snprintf(parabuf, maxlen, "%d", number);
    parapadding(parabuf, maxlen);
    move_cursor(x, y);
    printf("%s", parabuf);
}

