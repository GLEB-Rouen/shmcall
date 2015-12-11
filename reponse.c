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


struct shm_reponse {
  sem_t mutex;
  sem_t vide;
  sem_t plein;
  char *reponse;
};

struct queue {
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

int main(void) {
  int shm_fd = shm_open(NOM_SHM, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
  if (shm_fd == -1) {
    perror("shm_open");
  }
  if (shm_unlink(NOM_SHM) == -1) {
    perror("shm_unlink");
    exit(EXIT_FAILURE);
  }
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
  
  shm_fd = shm_open(my_var -> head -> nom_shm_rep, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
  if (shm_fd == -1) {
    perror("shm_open");
  }
  if (shm_unlink(my_var -> head -> nom_shm_rep) == -1) {
    perror("shm_unlink");
    exit(EXIT_FAILURE);
  }
  if (ftruncate(shm_fd, TAILLE_SHM_2) == -1) {
    perror("ftruncate");
    exit(EXIT_FAILURE);
  }
  shm_ptr = mmap(NULL,  TAILLE_SHM_2, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shm_ptr == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  
  struct shm_reponse *my_var_rep = (struct shm_reponse *) shm_ptr;
  printf("%s\n", my_var_rep -> reponse);
  
}
