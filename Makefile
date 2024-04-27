all:
	gcc -c -o WriteOutput.o WriteOutput.c
	gcc -c -o Helper.o helper.c
	g++ -o simulator *.cpp WriteOutput.o Helper.o -lpthread -std=c++17
