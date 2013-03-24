EXECUTABLE=xml-test
FLAGS=-Wall -Wextra -pedantic -std=c++11

all: do_exe

do_exe: main.o
	g++ main.o -o $(EXECUTABLE) 

main.o: main.cpp
	g++ -c $(FLAGS) main.cpp

clean:
	rm -rf *o
