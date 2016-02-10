all: default
	
default:
	g++ -std=c++11 -I include/ -o nbhashtable main.cpp src/NBHashTable.cpp