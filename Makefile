all: x

x: main.o
	g++ -Wall -o x main.o
	
main.o: main.c
	g++ -Wall -o main.o -c main.c
	
clean:
	rm -f x *.o 
   
