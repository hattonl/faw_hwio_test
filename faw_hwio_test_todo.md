1、删除client、can: a.out、.gitkeep

3、printf   ？建议删除

4、menu_utils ？删除

5、格式
        case 'f':
            uart_test();
            break;

6、main.c, while(1) ?删除

7、menu选项大小写，case添加

8、Kb -> KB

9、结构体名和函数名区分开
network_test —> network_test_info

10、drawnetworktestmenu 比较puts_number -> format print？
move_cursor(57, 3); 抽象下
用变量名表达？

11、建议函数指针放进结构体network_test_info，区分client、server

12、server_thread_func  功能、性能拆开

13、资源回收问题，ctrl + c 全局

14、canfd支持，波特率，

15、can test 用法?
能不能做成库，init，send，recv，exit·
可读性、可移植性、可复用性较差

16、can_unit_test.c  用处？删除

17、格式？？
        if (gpio_read(KEY3)) gpio_write(LED3, LED_ON);
        else                 gpio_write(LED3, LED_OFF);

		
18、struct for
gpio_open(LED0, DIRECTION_OUT)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
退出q为何做的如此复杂？

19、格式：void uart_test(void)
		
20、串口拆成库？
可读性、可移植性、可复用性较差





