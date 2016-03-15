all: default

BUILD_DIR=build
CFLAGS=-std=c++11 -I include/ -pthread
MAIN_FILE=test2.cpp

build/.placeholder:
	mkdir -vp $(BUILD_DIR)
	touch $(BUILD_DIR)/.placeholder

default: build/.placeholder
	g++ $(CFLAGS) -o build/nbhashtable $(MAIN_FILE) src/ProbeBound.cpp src/VersionState.cpp src/NBHashTable.cpp
	@echo Build complete. Placed executable into $(BUILD_DIR)/ directory.
