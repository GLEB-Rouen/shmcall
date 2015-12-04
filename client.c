#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#define MAX_CHAR_USER 16

void main(int argc, char *argv[]) {
	int isUserUid, isUserName, isPid, uid, pid;
  char string[];
	isUserPid = 0;
	isUserName = 0;
	isPid = 0;

	while ((opt = getopt(argc, argv, "u:n:p:")) != -1) {
       switch (opt) {
       case 'u':
           isUserPid = 1;
           uid = atoi(optarg)
           break;
       case 'n':
           isUserName = 1;
           uid = atoi(optarg)
           break;
        case 'p':
        	startFrom = atoi(optarg);
        	break;
       	case 'e':
       		goTo = atoi(optarg);
       		if (!goTo) {
       			fprintf(stderr, "-e can't be set at 0\n");
       			exit(EXIT_FAILURE);
       		}
       		break;
       default: /* '?' */
           fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n",
                   argv[0]);
           exit(EXIT_FAILURE);
       }
   }