#ifndef __GPIO_H
#define __GPIO_H


#define DIRECTION_IN   0
#define DIRECTION_OUT  1

int gpio_open(int pin_num, int direction);

int gpio_read(int pin_num);

int gpio_write(int pin_num, int val);


#endif
