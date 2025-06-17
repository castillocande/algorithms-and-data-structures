local:
	gcc -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror -o tp1 *.c -lm
	./tp1
	valgrind -s --error-exitcode=1 --leak-check=full --show-leak-kinds=all --track-origins=yes ./tp1
	rm tp1
