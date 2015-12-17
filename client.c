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
#include <time.h>

#define N 30
#define MAX_CHAR_USER 16
#define MAX_SIZE_FILENAME 255
#define NOM_SHM "/File_question"


struct shm_reponse {
  sem_t sem_p;
  char reponse[2048];
};

struct file {
  sem_t mutex;
  sem_t vide;
  sem_t plein;
  int tete;      // Position d'ajout dans le tampon
  int queue;     // Position de suppression dans le tampon
   char buffer[][50]; // Le tampon contenant les données
  
};

#define TAILLE_SHM (sizeof(struct file) + (N * 50))


#define TAILLE_SHM_2 (sizeof(struct shm_reponse))


int main(int argc, char *argv[]) {
  srand(time(NULL));
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
  sprintf(nomReponse, "/%d", rand());

  static char laQuest[2048];
  if (isUserUid) {
    strcpy(laQuest, "info_user");
  }
  else if (isUserName) {
    strcpy(laQuest, "info_user");
  }
  else if (isPid) {
    strcpy(laQuest, "info_proc");
  }
  else {
    strcpy(laQuest, "info_file");
  }
  strcat(laQuest, ":");
  strcat(laQuest, arg);
  strcat(laQuest, ":");
  strcat(laQuest, nomReponse);

  int shm_fd = shm_open(NOM_SHM, O_RDWR, S_IRUSR | S_IWUSR);
  if (shm_fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }
  if (ftruncate(shm_fd, TAILLE_SHM) == -1) {
    perror("ftruncate");
    exit(EXIT_FAILURE);
  }
  struct file *shm_ptr = mmap(NULL, TAILLE_SHM, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shm_ptr == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  if (sem_wait(&shm_ptr->vide) == -1) {
    perror("sem_wait");
    exit(EXIT_FAILURE);
  }
  if (sem_wait(&shm_ptr->mutex) == -1) {
    perror("sem_wait");
    exit(EXIT_FAILURE);
  }
  strcpy(shm_ptr->buffer[shm_ptr->queue], laQuest);
  shm_ptr->queue = (shm_ptr->queue + 1) % N; 
  if (sem_post(&shm_ptr->mutex) == -1) {
      perror("sem_post");
      exit(EXIT_FAILURE);
  }
  if (sem_post(&shm_ptr->plein) == -1) {
    perror("sem_post");
    exit(EXIT_FAILURE);
  }
  shm_fd = shm_open(nomReponse, 
      O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
  if (shm_fd == -1) {
    perror("shm_open or error in random");
    exit(EXIT_FAILURE);
  }

  if (ftruncate(shm_fd, TAILLE_SHM_2) == -1) {
    perror("ftruncate");
    exit(EXIT_FAILURE);
  }
  struct shm_reponse *shm_ptr2 = mmap(NULL,  TAILLE_SHM_2, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shm_ptr == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  if (sem_init(&shm_ptr2 -> sem_p, 1, 0) == -1) {
    perror("sem_init");
    exit(EXIT_FAILURE);
  }
  if (sem_wait(&shm_ptr2 -> sem_p) == -1) {
      perror("sem_wait");
      exit(EXIT_FAILURE);
    }
  printf("%s", shm_ptr2 -> reponse );
  if (shm_unlink(nomReponse) == -1) {
    perror("shm_unlink");
    exit(EXIT_FAILURE);
  }
 }
