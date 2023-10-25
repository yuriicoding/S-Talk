all: x

x: main.o list.o 
	g++ -Wall -o x main.o list.o -lpthread
	
main.o: main.c list.h
	g++ -Wall -o main.o -c main.c 
	
clean:
	rm -f x main.o main
   
