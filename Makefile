all: x

x: main.o
	g++ -Wall -o x main.o -lpthread
	
main.o: main.c
	g++ -Wall -o main.o -c main.c
	
clean:
	rm -f x *.o 
   
