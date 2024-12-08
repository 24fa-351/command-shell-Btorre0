make:
	gcc -o main main.c xsh.c exec.c pipe.c -Wall -Wextra -pedantic

run:
	./xsh

clean:
	rm -f xsh

