#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#define MAX_CHAR_USER 16
#define MAX_SIZE_FILENAME 255

struct shm_reponse {
  int rdv;
  char reponse[2048];
};


struct queue {
  struct question *head;
  struct question *tail;

};
struct question {
  struct question *next;
  char *cmd;
  char *arg;
  struct shm_reponse reponse;
};


int main(int argc, char *argv[]) {
  int isUserUid, isUserName, isPid, isFile, opt;
  char userName[MAX_CHAR_USER], fileName[MAX_SIZE_FILENAME],uid[10], pid[10] ;
  isUserUid = 0;
  isUserName = 0;
  isPid = 0;

  while ((opt = getopt(argc, argv, "hf:u:n:p:")) != -1) {
       switch (opt) {
       case 'u':
          isUserUid = 1;
          strcpy(uid,optarg);
          break;
       case 'n':
          isUserName = 1;
          strcpy(userName,optarg);
          break;
        case 'p':
          isPid = 1;
          strcpy(pid,optarg);
          break;
        case 'f':
          isFile = 1;
          strcpy(fileName,optarg);
          break;
        case 'h':
          printf("BLALALALALALLALALLALALALALLALAAKLKLEWDHVKWXFJYKHYUGFUILKL\n");
          exit(EXIT_SUCCESS);
       default:
          printf("Usage : ./client -? arg, for more information type ./client -h\n");
          exit(EXIT_FAILURE);
       }
   }
   if(isUserUid + isUserName + isFile +isPid == 2) {
      printf("Erreur, un seul argument possible\n");
      exit(EXIT_FAILURE);
   }





 }