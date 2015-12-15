#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>

#define NOM_SHM "/File_question"

#define N 30



typedef void *(*start_routine_type)(void *);

struct shm_reponse {
  sem_t mutex;
  sem_t vide;
  sem_t plein;
  char *reponse;
};

struct queue {
  char *test;
  sem_t mutex;
  sem_t vide;
  sem_t plein;
  struct question *head;
  struct question *tail;
  
};
struct question {
  struct question *next;
  char *cmd;
  char *arg;
  char *nom_shm_rep; 
};

#define TAILLE_SHM (sizeof(struct queue))

#define TAILLE_SHM_2 (sizeof(struct shm_reponse))

void *run(struct question *quest) {
  
  int fd[2];
  if (pipe(fd) == -1) {
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
      static char s[1024] = "./";
      strcat(s, quest->cmd);
      execlp(s, s, quest->arg, NULL);
      perror("execlp");
      exit(EXIT_FAILURE);
    default:
      wait(NULL);
      printf("run\n");
      if (close(fd[1]) == -1) {
        perror("close()");
        exit(EXIT_FAILURE);
      }
      
      int shm_fd = shm_open(quest -> nom_shm_rep, 
      O_RDWR , S_IRUSR | S_IWUSR);
      if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
      }
      if (ftruncate(shm_fd, TAILLE_SHM_2) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
      }
      char *shm_ptr = mmap(NULL,  TAILLE_SHM_2, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
      if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
      }
  
      struct shm_reponse *my_var = (struct shm_reponse *) shm_ptr;
      int n;
      static char res[2048];
      while ((n = read(fd[0], res, 2048)) > 0) {
        my_var -> reponse = strcat(my_var -> reponse, res);
      }
      if (n == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
      if (close(fd[0]) == -1) {
        perror("close()");
        exit(EXIT_FAILURE);
      }
  }
  return NULL;
}


int main(void) {
  int shm_fd = shm_open(NOM_SHM, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
  if (shm_fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }
  /*if (shm_unlink(NOM_SHM) == -1) {
    perror("shm_unlink");
    exit(EXIT_FAILURE);
  }*/
  if (ftruncate(shm_fd, TAILLE_SHM) == -1) {
    perror("ftruncate");
    exit(EXIT_FAILURE);
  }
  char *shm_ptr = mmap(NULL,  TAILLE_SHM, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shm_ptr == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  
  struct queue *my_var = (struct queue *) shm_ptr;
  my_var -> test = "test";
   if (sem_init(&my_var->mutex, 1, 1) == -1) {
    perror("sem_init");
    exit(EXIT_FAILURE);
  }

  if (sem_init(&my_var->vide, 1, N) == -1) {
    perror("sem_init");
    exit(EXIT_FAILURE);
  }

  if (sem_init(&my_var->plein, 1, 0) == -1) {
    perror("sem_init");
    exit(EXIT_FAILURE);
  }
  struct question *quest = malloc(sizeof(struct question));
  quest -> next = NULL;
  quest -> cmd = "info_user";
  quest -> arg = "naimigui";
  quest -> nom_shm_rep = "/1804289383";
  int errnum;
  pthread_attr_t th_attr;
  if ((errnum = pthread_attr_init(&th_attr)) != 0) {
    fprintf(stderr, "pthread_attr_init(): %s\n", strerror(errnum));
    exit(EXIT_FAILURE);
  }
  if ((errnum = pthread_attr_setdetachstate(&th_attr, PTHREAD_CREATE_DETACHED)) != 0) {
    fprintf(stderr, "pthread_attr_setdetachstate(): %s\n", strerror(errnum));
    exit(EXIT_FAILURE);
  }
  while (1) {
    printf("avant\n");
    if (sem_wait(&my_var->plein) == -1) {
      perror("sem_wait");
      exit(EXIT_FAILURE);
    }
    printf("aprés\n");
    if (sem_wait(&my_var->mutex) == -1) {
      perror("sem_wait");
      exit(EXIT_FAILURE);
    }
    
    pthread_t th;
    if ((errnum = pthread_create(&th, &th_attr, (start_routine_type) run, quest/*my_var->head*/)) != 0) {
      fprintf(stderr, "pthread_create(): %s\n", strerror(errnum));
      exit(EXIT_FAILURE);
    }
    /*if (my_var -> head -> next == NULL) {
      my_var -> tail = NULL;
    }
    my_var -> head = my_var -> head -> next;*/
    if (sem_post(&my_var->mutex) == -1) {
      perror("sem_post");
      exit(EXIT_FAILURE);
    }
    if (sem_post(&my_var->vide) == -1) {
      perror("sem_post");
      exit(EXIT_FAILURE);
    }
  }
  if ((errnum = pthread_attr_destroy(&th_attr)) != 0) {
      fprintf(stderr, "pthread_attr_destroy(): %s\n", strerror(errnum));
      exit(EXIT_FAILURE);
    }
  if (sem_destroy(&my_var->mutex) == -1) {
    perror("sem_destroy");
    exit(EXIT_FAILURE);
  }
  if (sem_destroy(&my_var->plein) == -1) {
    perror("sem_destroy");
    exit(EXIT_FAILURE);
  }
  if (sem_destroy(&my_var->vide) == -1) {
    perror("sem_destroy");
    exit(EXIT_FAILURE);
  }
  
  
}
  
