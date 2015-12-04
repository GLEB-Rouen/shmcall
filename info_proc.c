#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

void caracteristique(pid_t pid) {
  static char s[1024];
  snprintf(s, 1024,"/proc/%d/status", pid);
  FILE *f;
  if ((f = fopen(s, "r")) == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }
  int n = 0;
  static char t[1024];
  while (n != 4 && fgets(t, 1024, f) != NULL) {
    if (strncmp(t, "Name", 4) == 0) {
      n++;
      printf("[%d] %s", pid, t + 6);
    }
    if (strncmp(t, "State", 5) == 0) {
      n++;
      printf("[%d] %s", pid, t + 7);
    }
    if (strncmp(t, "Tgid", 4) == 0) {
      n++;
      printf("[%d] TGID: %s", pid, t + 6);
    }
    if (strncmp(t, "PPid", 4) == 0) {
      n++;
      printf("[%d] PPID: %s", pid, t + 6);
    }
  }
  if (n != 4) {
    printf("attribut manquant\n");
    exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS);
}


  
int main (int argc, char *argv[]) {
  if (argc != 2) {
    printf("erreur d'argument\n");
  }
  pid_t pid = atoi(argv[1]);
  caracteristique(pid);
  return EXIT_SUCCESS;
}
