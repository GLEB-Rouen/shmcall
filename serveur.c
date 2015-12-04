#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>


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

void *run(struct question quest) {
  switch (fork()) {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);
    case 0:
      execlp(./(question->cmd), ./(question->cmd), question->arg, NULL);
      perror("execlp");
      exit(EXIT_FAILURE);
    default:
      wait(NULL);
  }
}


int main(void) {
  
}
  
