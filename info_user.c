#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>
#include <pwd.h>


void caracteristiques_uid(uid_t uid) {
  struct passwd *pass;
  int errno = 0;
  if ((pass = getpwuid(uid)) == NULL) {
    if (errno != 0) {
      perror("getpwuid");
    } else {
      fprintf(stderr, "l'utilisateur %d n'existe pas\n",
        uid);
    }
    exit(EXIT_FAILURE);
  }
  printf ("[%d] %s\n", uid, pass->pw_name);
  printf ("[%d] %d\n", uid, pass->pw_uid);
  printf ("[%d] %d\n", uid, pass->pw_gid);
  printf ("[%d] %s\n", uid, pass->pw_gecos);
  printf ("[%d] %s\n", uid, pass->pw_dir);
  printf ("[%d] %s\n", uid, pass->pw_shell);
  exit(EXIT_SUCCESS);
}

void caracteristiques_login(char *login) {
  struct passwd *pass;
  int errno = 0;
  if ((pass = getpwnam(login)) == NULL) {
    if (errno != 0) {
      perror("getpwnam");
    } else {
      fprintf(stderr, "l'utilisateur %s n'existe pas\n",
        login);
    }
    exit(EXIT_FAILURE);
  }
  printf ("[%s] %s\n", login, pass->pw_name);
  printf ("[%s] %d\n", login, pass->pw_uid);
  printf ("[%s] %d\n", login, pass->pw_gid);
  printf ("[%s] %s\n", login, pass->pw_gecos);
  printf ("[%s] %s\n", login, pass->pw_dir);
  printf ("[%s] %s\n", login, pass->pw_shell);
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s [uid] ou [login]\n",
        argv[0]);
    exit(EXIT_FAILURE);
  }
  char *s = argv[1];
  int is_int = 1;
  for (size_t i = 0; i < strlen(s); i++) {
    if (s[i] < '0' || s[i] > '9') {
      is_int = 0;
    }
  }
  if (is_int == 1) {
    caracteristiques_uid(atoi(argv[1]));
  } else {
    caracteristiques_login(argv[1]);
  }
  return EXIT_SUCCESS;
}
