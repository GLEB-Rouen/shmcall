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

#define NOM_SHM "File_question"

typedef void *(*start_routine_type)(void *);

struct queue {
  struct question *head;
  struct question *tail;
  int rdv;
};
struct question {
  struct question *next;
  char *cmd;
  char *arg;
  struct shm_reponse rep;
};

void *run(struct question quest) {
  int fd1[2];
  if (pipe(fd1) == -1) {
    perror("pipe()");
    exit(EXIT_FAILURE);
  }
  switch (fork()) {
    case -1:
      perror("fork()");
      exit(EXIT_FAILURE);
    case 0:
      if (close(fd[0]) == -1) {
        perror("close()");
        exit(EXIT_FAILURE);
      }
      if (dup2(fd[1], STDOUT_FILENO) == -1) {
        perror("dup2()");
        exit(EXIT_FAILURE);
      }
      if (close(fd[1]) == -1) {
        perror("close()");
        exit(EXIT_FAILURE);
      }
      char *s;
      strcat(s, "./");
      strcat(s, quest->cmd);
      execlp(s, s, quest->arg, NULL);
      perror("execlp");
      exit(EXIT_FAILURE);
    default:
      if (close(fd[1]) == -1) {
        perror("close()");
        exit(EXIT_FAILURE);
      }
      while ((n = read(tube[0], rep->reponse, 2048)) > 0) {
        printf("%s\n", s);
      }
      if (close(fd[0]) == -1) {
        perror("close()");
        exit(EXIT_FAILURE);
      }
  }
}


int main(void) {
    int shm_fd = shm_open(NOM_SHM, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
  if (shm_fd == -1) {
    perror("shm_open");
  }
  /*if (shm_unlink(NOM_SHM) == -1) {
    perror("shm_unlink");
    exit(EXIT_FAILURE);
  }*/
  struct queue *my_var;
  if (ftruncate(shm_fd, sizeof(my_var)) == -1) {
    perror("ftruncate");
    exit(EXIT_FAILURE);
  }
  my_var = mmap(NULL, sizeof(my_var), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (my_var == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  pthread_attr_t th_attr;
  if ((errnum = pthread_attr_init(&th_attr)) != 0) {
    fprintf(stderr, "pthread_attr_init(): %s\n", strerror(errnum));
    exit(EXIT_FAILURE);
  }
  if ((errnum = pthread_attr_setdetachstate(&th_attr, PTHREAD_CREATE_DETACHED)) != 0) {
    fprintf(stderr, "pthread_attr_setdetachstate(): %s\n", strerror(errnum));
    exit(EXIT_FAILURE);
  }
      
  pthread_t th;
  while (queue -> head != NULL) {
    if ((errnum = pthread_create(&th, &th_attr, (start_routine_type) run, my_var->head)) != 0) {
      fprintf(stderr, "pthread_create(): %s\n", strerror(errnum));
      exit(EXIT_FAILURE);
    }
    if ((errnum = pthread_attr_destroy(&th_attr)) != 0) {
      fprintf(stderr, "pthread_attr_destroy(): %s\n", strerror(errnum));
      exit(EXIT_FAILURE);
    }
    
    queue -> head = queue -> head -> next;
  }
}
  
