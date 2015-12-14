CFLAGS=-std=c11 -Wall -Wextra -Werror -pedantic -D_XOPEN_SOURCE=500

LDLIBS = -lrt
LDFLAGS = -pthread

all: info_user info_proc info_file serveur reponse client

pwd: info_user.o info_proc.o info_file.o serveur.o reponse.o client.o

pwd.o: info_user.c info_proc.c info_file.c serveur.c reponse.c client.c


clean:
	$(RM) *.o info_user info_proc info_file serveur reponse s*~
