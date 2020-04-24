#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <net/if.h>

#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "can_test.h"

#include "menu.h"

#include "can_recv_test.h"
#include "can_send_test.h"

#define DEFAULT_TESTMODE  "Recv"
#define DEFAULT_CANDEVICE "can0"

#define MAXLEN 8

static char parabuf[MAXLEN];



void drawcantestmenu()
{
	system("clear");
	printf("\033[0;0H");
    printf("+----------[Can Test Menu]---------+\n");
    printf("| D -  Can Device        : can0    |\n"); // string
    printf("| M -  Can Test Mode     : Recv    |\n");
    printf("|                                  |\n");
    printf("+----------------------------------+\n");
    printf("     Change which setting?\n");
	printf("\033[s");
}


int findcan(char *buf)
{
	int s;
	// int ret = 0;
	struct ifreq ifr;

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		// perror("socket");
		return 1;
	}

	strcpy(ifr.ifr_name, buf);
	if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
		close(s);

		return 1;
	}

	close(s);
	return 0;
}


void can_test (void)
{
	char c;
	int isRecv = 1;
	int ret = 0;
	int breakwhile = 0;

	drawcantestmenu();

	// puts_string(28, 2, DEFAULT_CANDEVICE, MAXLEN);
	// puts_string(28, 3, DEFAULT_TESTMODE,  MAXLEN);
	
	memcpy(parabuf, DEFAULT_CANDEVICE, sizeof(DEFAULT_CANDEVICE));

	ret = findcan(parabuf);

	if (ret != 0) {
		parabuf[0] = 0;
	}

	puts_string(28, 2, parabuf, MAXLEN);

	printf(" |");
	printf("\033[K");
	printf("\033[u");

	if (ret != 0) {
		printf("can device not find\n");
	}


	while (1) {
		if (breakwhile) break;

		c = getch_noecho();
		switch (c)
		{
		case '\n':

			if (parabuf[0] != 0) breakwhile = 1;

			break;
		
		case 'd':
			move_cursor(28, 2);
            scanf("%s", &parabuf);


			// check
			ret = findcan(parabuf);

			if (ret != 0) {
				parabuf[0] = 0;
			}

			puts_string(28, 2, parabuf, MAXLEN);

            printf(" |");
            printf("\033[K");
            printf("\033[u");

			if (ret != 0) {
				printf("can device not find\n");
			}
			else {
				printf("\033[K");
			}
			
			while ((c = getchar() ) != '\n' && c != EOF);

			break;

		case 'm':
			isRecv ^= 0x01;
			if (isRecv == 1) {
				puts_string(28, 3, "Recv", MAXLEN);
			} else {
				puts_string(28, 3, "Send", MAXLEN);
			}
			printf("\033[u");

			break;

		default:
			break;
		}
		// getchar();

	}

	printf("CAN Test running ");


	if (isRecv) {
		printf("Recv Mode ... \n");
		can_recv_test(parabuf);
	} else {
		printf("Send Mode ... \n");
		can_send_test(parabuf);
	}

	printf("\nTest Finished: Input any char to return ... \n");
    getchar();
	// while(1);

	return;
}
