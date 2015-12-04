CFLAGS=-std=c11 -Wall -Wextra -Werror -pedantic -D_XOPEN_SOURCE

all: info_user info_proc info_file serveur 

pwd: info_user.o info_proc.o info_file.o serveur.o

pwd.o: info_user.c info_proc.c info_file.c serveur.c

clean:
	$(RM) *.o info_user info_proc info_file serveur s*~
