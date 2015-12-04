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


int affiche_attr(const char *file) {

  ssize_t d;
  if ((d = open(file, O_RDONLY)) == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  struct stat s; 
  if (fstat(d, &s) == -1) {
    perror("stat");
    exit(EXIT_FAILURE);
  }
  static char *type;
  if (S_ISREG(s.st_mode)) {
    type = "file";
  }
  if (S_ISDIR(s.st_mode)) {
    type = "directory";
  } 
  if (S_ISCHR(s.st_mode)) {
    type = "character device";
  } 
  if (S_ISBLK(s.st_mode)) {
    type = "block device";
  } 
  if (S_ISFIFO(s.st_mode)) {
    type = "FIFO";
  } 
  if (S_ISLNK(s.st_mode)) {
    type = "symbolic link";
  } 
  char *ru = "-";
  char *wu = "-";
  char *xu = "-";
  static char mode[9];
  if (s.st_mode & S_IRUSR) {
    ru = "r";
  }
  if (s.st_mode & S_IWUSR) {
    wu = "w";
  }
  if (s.st_mode & S_IXUSR) {
    xu = "x";
  }
  char *rg = "-";
  char *wg = "-";
  char *xg = "-";
  if (s.st_mode & S_IRGRP) {
    rg = "r";
  }
  if (s.st_mode & S_IWGRP) {
    wg = "w";
  }
  if (s.st_mode & S_IXGRP) {
    xg = "x";
  }
  char *ro = "-";
  char *wo = "-";
  char *xo = "-";
  if (s.st_mode & S_IROTH) {
    ro = "r";
  }
  if (s.st_mode & S_IWOTH) {
    wo = "w";
  }
  if (s.st_mode & S_IXOTH) {
    xo = "x";
  }
  sprintf(mode, "%s%s%s%s%s%s%s%s%s", ru,wu,xu,rg,wg,xg,ro,wo,xo);
  
  printf("%jd %s %s %d %s %s %d %s %s\n", (intmax_t) s.st_ino,
  type, mode, (int) s.st_nlink, getpwuid(s.st_uid)->pw_name, 
  getgrgid(s.st_gid)->gr_name, (int) s.st_size, file, ctime(&s.st_mtime));
  close(d);
  return 0;
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s [path]\n",
        argv[0]);
    exit(EXIT_FAILURE);
  }
  affiche_attr(argv[1]);
}
