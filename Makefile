all: x

x: alter_main.o
	g++ -Wall -o x alter_main.o -lpthread
	
main.o: alter_main.c
	g++ -Wall -o alter_main.o -c alter_main.c
	
clean:
	rm -f x *.o 
   
