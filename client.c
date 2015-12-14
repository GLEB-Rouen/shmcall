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


#define MAX_CHAR_USER 16
#define MAX_SIZE_FILENAME 255
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

#define TAILLE_SHM_2 (sizeof(struct shm_reponse))
#define TAILLE_SHM (sizeof(struct queue))

int main(int argc, char *argv[]) {
  int isUserUid, isUserName, isPid, isFile, opt;
  char arg[1024] ;
  isUserUid = 0;
  isUserName = 0;
  isPid = 0;
  isFile = 0;

  while ((opt = getopt(argc, argv, "hf:u:n:p:")) != -1) {
       switch (opt) {
       case 'u':
          isUserUid = 1;
          strcpy(arg,optarg);
          break;
       case 'n':
          isUserName = 1;
          strcpy(arg,optarg);
          break;
        case 'p':
          isPid = 1;
          strcpy(arg,optarg);
          break;
        case 'f':
          isFile = 1;
          strcpy(arg,optarg);
          break;
        case 'h':
          printf("BLALALALALALLALALLALALALALLALAAKLKLEWDHVKWXFJYKHYUGFUILKL\n");
          exit(EXIT_SUCCESS);
       default:
          printf("Usage : ./client -? arg, for more information type ./client -h\n");
          exit(EXIT_FAILURE);
       }
   }
   if(isUserUid + isUserName + isFile +isPid != 1) {
      printf("Erreur, un seul argument possible\n");
      exit(EXIT_FAILURE);
   }
   
  static char nomReponse[2048];
  sprintf(nomReponse, "%d", rand());






  struct question *my_question = malloc(sizeof(struct question));
  my_question->next = NULL;
  char *laCmd;
  if (isUserUid) {
    laCmd = "info_user";
  }
  else if (isUserName) {
    laCmd = "info_user";
  }
  else if (isPid) {
    laCmd = "info_proc";
  }
  else {
    laCmd = "info_file";
  }
  my_question->cmd = laCmd;
  my_question->arg = arg;
  my_question->nom_shm_rep = nomReponse;

  int shm_fd = shm_open(NOM_SHM, O_RDWR , S_IRUSR | S_IWUSR);
  if (shm_fd == -1) {
    perror("shm_open");
  }
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
  /*if (sem_wait(&my_var->vide) == -1) {
    perror("sem_wait");
    exit(EXIT_FAILURE);
  }
  if (sem_wait(&my_var->mutex) == -1) {
    perror("sem_wait");
    exit(EXIT_FAILURE);
  }*/
  if(my_var -> head == NULL) {
    my_var -> head = my_question;
  } else {
    my_var -> tail -> next = my_question;
  }
  my_var -> tail = my_question;

  /*if (sem_post(&my_var->mutex) == -1) {
      perror("sem_post");
      exit(EXIT_FAILURE);
  }
  if (sem_post(&my_var->plein) == -1) {
    perror("sem_post");
    exit(EXIT_FAILURE);
  }*/

  sleep(3);
  shm_fd = shm_open(nomReponse, 
      O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
  if (shm_fd == -1) {
    perror("shm_open or error in random");
  }

  if (shm_unlink(NOM_SHM) == -1) {
    perror("shm_unlink");
    exit(EXIT_FAILURE);
  }

  if (ftruncate(shm_fd, TAILLE_SHM_2) == -1) {
    perror("ftruncate");
    exit(EXIT_FAILURE);
  }
  printf("a\n");
  char *shm_ptr = mmap(NULL,  TAILLE_SHM_2, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shm_ptr == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  
  struct shm_reponse *maReponse = (struct shm_reponse*) shm_ptr;
  printf("%s\n", maReponse -> reponse );





 }
