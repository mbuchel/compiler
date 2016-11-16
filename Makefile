debug:
	gcc -g3 *.c -o compiler -lpthread
all:
	gcc *.c -o compiler -lpthread
clean:
	rm compiler
