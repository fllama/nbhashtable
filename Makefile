all: default

BUILD_DIR=build

build/.placeholder:
	mkdir -vp $(BUILD_DIR)
	touch $(BUILD_DIR)/.placeholder

default: build/.placeholder
	g++ -std=c++11 -I include/ -o build/nbhashtable main.cpp src/NBHashTable.cpp
	@echo Build complete. Placed executable into $(BUILD_DIR)/ directory.