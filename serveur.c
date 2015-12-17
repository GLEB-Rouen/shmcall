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


char **decoupe_string(char *string);


void *run(char *quest) {
  
  int fd[2];
  if (pipe(fd) == -1) {
    perror("pipe()");
    exit(EXIT_FAILURE);
  }
  char **temp = decoupe_string(quest);
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
      strcat(s, temp[0]);
      execlp(s, s, temp[1], NULL);
      perror("execlp");
      exit(EXIT_FAILURE);
    default:
      wait(NULL);
      if (close(fd[1]) == -1) {
        perror("close()");
        exit(EXIT_FAILURE);
      }
      int shm_fd = shm_open(temp[2], 
      O_RDWR , S_IRUSR | S_IWUSR);
      if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
      }
      if (ftruncate(shm_fd, TAILLE_SHM_2) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
      }
      struct shm_reponse *shm_ptr = mmap(NULL,  TAILLE_SHM_2, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
      if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
      }
      int n;
      static char res[2048];
      shm_ptr -> reponse[0] = 0;
      while ((n = read(fd[0], res, 2048)) > 0) {
        strncat(shm_ptr -> reponse, res, n);
      }
      if (n == -1) {
        perror("read");
        exit(EXIT_FAILURE);
      }
      if (close(fd[0]) == -1) {
        perror("close()");
        exit(EXIT_FAILURE);
      }
      if (sem_post(&shm_ptr -> sem_p) == -1) {
      perror("sem_post");
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
  if (ftruncate(shm_fd, TAILLE_SHM) == -1) {
    perror("ftruncate");
    exit(EXIT_FAILURE);
  }
  struct file *shm_ptr = mmap(NULL,  TAILLE_SHM, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shm_ptr == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
   if (sem_init(&shm_ptr->mutex, 1, 1) == -1) {
    perror("sem_init");
    exit(EXIT_FAILURE);
  }

  if (sem_init(&shm_ptr->vide, 1, N) == -1) {
    perror("sem_init");
    exit(EXIT_FAILURE);
  }

  if (sem_init(&shm_ptr->plein, 1, 0) == -1) {
    perror("sem_init");
    exit(EXIT_FAILURE);
  }
  shm_ptr->tete = 0;
  shm_ptr->queue = 0;
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
    if (sem_wait(&shm_ptr->plein) == -1) {
      perror("sem_wait");
      exit(EXIT_FAILURE);
    }
    if (sem_wait(&shm_ptr->mutex) == -1) {
      perror("sem_wait");
      exit(EXIT_FAILURE);
    }
    
    pthread_t th;
    if ((errnum = pthread_create(&th, &th_attr, (start_routine_type) run, shm_ptr->buffer[shm_ptr->tete])) != 0) {
      fprintf(stderr, "pthread_create(): %s\n", strerror(errnum));
      exit(EXIT_FAILURE);
    }
    shm_ptr->tete = (shm_ptr->tete + 1) % N;
    if (sem_post(&shm_ptr->mutex) == -1) {
      perror("sem_post");
      exit(EXIT_FAILURE);
    }
    if (sem_post(&shm_ptr->vide) == -1) {
      perror("sem_post");
      exit(EXIT_FAILURE);
    }
  }
  if ((errnum = pthread_attr_destroy(&th_attr)) != 0) {
      fprintf(stderr, "pthread_attr_destroy(): %s\n", strerror(errnum));
      exit(EXIT_FAILURE);
  }
  if (shm_unlink(NOM_SHM) == -1) {
    perror("shm_unlink");
    exit(EXIT_FAILURE);
  }
  if (sem_destroy(&shm_ptr->mutex) == -1) {
    perror("sem_destroy");
    exit(EXIT_FAILURE);
  }
  if (sem_destroy(&shm_ptr->plein) == -1) {
    perror("sem_destroy");
    exit(EXIT_FAILURE);
  }
  if (sem_destroy(&shm_ptr->vide) == -1) {
    perror("sem_destroy");
    exit(EXIT_FAILURE);
  }
}

char **decoupe_string(char *string) {
    char c = ' ';
    static char *res[3];
    int j = 0;
    int deb = 0;
    int ind = 0;
    for (size_t i = 0; i <= strlen(string); i++) {
      c = string[i];
      if (c == ':' || c == '\0') {
        j = i;
        res[ind] = malloc(sizeof(char) * (j - deb));
        strncpy(res[ind], &string[deb], j - deb);
        ind ++;
        deb = i + 1;
      }
    }
    return res;
  }
  
