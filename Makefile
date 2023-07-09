CC = gcc
CFLAGS = -Wall

all: dialogue prog

dialogue: dialogue.c
	$(CC) $(CFLAGS) dialogue.c -o dialogue

prog: main.c user.c
	$(CC) $(CFLAGS) main.c user.c -o prog

clean:
	rm -f dialogue prog pipe*

