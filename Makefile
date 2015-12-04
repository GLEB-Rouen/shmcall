CFLAGS=-std=c11 -Wall -Wextra -Werror -pedantic -D_XOPEN_SOURCE

all: info_user info_proc info_file

pwd: info_user.o info_proc.o info_file.o

pwd.o: info_user.c info_proc.c info_file.c

clean:
	$(RM) *.o info_user info_proc info_file *~
