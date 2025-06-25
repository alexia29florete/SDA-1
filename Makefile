CFLAGS = -Wall -Wextra -std=c99 -g

build: tema1

tema1: tema1.c
	$(CC) $(CFLAGS) tema1.c -o tema1

clean:
	rm -f tema1.c
