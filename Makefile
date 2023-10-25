all: x

x: main.o list.o 
	g++ -Wall -o x main.o list.o -lpthread
	
main.o: main.c list.h
	g++ -Wall -o main.o -c main.c 
	
list.o: list.h list.c
	g++ -Wall -c list.c
	
clean:
	rm -f x main.o list.o
