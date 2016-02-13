all: default

BUILD_DIR=build
CFLAGS=-std=c++11 -I include/ -pthread
MAIN_FILE=main.cpp

build/.placeholder:
	mkdir -vp $(BUILD_DIR)
	touch $(BUILD_DIR)/.placeholder

default: build/.placeholder
	g++ $(CFLAGS) -o build/nbhashtable $(MAIN_FILE) src/NBHashTable.cpp
	@echo Build complete. Placed executable into $(BUILD_DIR)/ directory.
