all:
	gcc -c -o WriteOutput.o WriteOutput.c
	g++ -o simulator *.cpp WriteOutput.o -std=c++17
