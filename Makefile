all:
	gcc -c -o WriteOutput.o WriteOutput.c
	g++ -o simulator *.cpp WriteOutput.o -lpthread -std=c++17
