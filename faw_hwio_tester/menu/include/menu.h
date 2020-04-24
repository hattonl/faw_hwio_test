#ifndef __MENU_H
#define __MENU_H

char getch_noecho();
void parapadding(char *parabuf, int len);
void move_cursor(int x, int y);
void puts_string(int x, int y, char *ch, int maxlen);
void puts_number(int x, int y, int number, int maxlen);

#endif
