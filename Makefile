all: tests

BUILD_DIR=build
TEST_RESULT_DIR=test
CFLAGS=-std=c++11 -pthread
MAIN_FILE=executiontime.cpp

tests: correctness executionTime

build/.placeholder:
	mkdir -vp $(BUILD_DIR)
	touch $(BUILD_DIR)/.placeholder

test/.placeholder:
	mkdir -vp $(TEST_RESULT_DIR)
	touch $(TEST_RESULT_DIR)/.placeholder

default: build/.placeholder
	g++ $(CFLAGS) -o $(BUILD_DIR)/nbhashtable $(MAIN_FILE) NBHashTable.cpp
	@echo Build complete. Placed executable into $(BUILD_DIR)/ directory.

correctness: build/.placeholder test/.placeholder
	g++ $(CFLAGS) -o $(BUILD_DIR)/correctness1 correctness1.cpp NBHashTable.cpp
	g++ $(CFLAGS) -o $(BUILD_DIR)/correctness2 correctness2.cpp NBHashTable.cpp
	@echo Correctness executable file built and placed into $(BUILD_DIR)/

executionTime: build/.placeholder test/.placeholder
	g++ $(CFLAGS) -o $(BUILD_DIR)/executionTime executiontime.cpp NBHashTable.cpp
	@echo Execution time executable file built and placed into $(BUILD_DIR)/

executionTimeTest: executionTime time1 time2 time3 time4 time5 time6
	@echo Execution time testing complete. Placed results into $(TEST_RESULT_DIR)/

time1:
	@echo Testing trial 1
	$(BUILD_DIR)/nbhashtable 1 75 25 0 > $(TEST_RESULT_DIR)/test1-1.txt
	$(BUILD_DIR)/nbhashtable 2 75 25 0 > $(TEST_RESULT_DIR)/test1-2.txt
	$(BUILD_DIR)/nbhashtable 4 75 25 0 > $(TEST_RESULT_DIR)/test1-4.txt
	$(BUILD_DIR)/nbhashtable 8 75 25 0 > $(TEST_RESULT_DIR)/test1-8.txt
	
time2:
	@echo Testing trial 2
	$(BUILD_DIR)/nbhashtable 1 25 75 0 > $(TEST_RESULT_DIR)/test2-1.txt
	$(BUILD_DIR)/nbhashtable 2 25 75 0 > $(TEST_RESULT_DIR)/test2-2.txt
	$(BUILD_DIR)/nbhashtable 4 25 75 0 > $(TEST_RESULT_DIR)/test2-4.txt
	$(BUILD_DIR)/nbhashtable 8 25 75 0 > $(TEST_RESULT_DIR)/test2-8.txt
	
time3:
	@echo Testing trial 3
	$(BUILD_DIR)/nbhashtable 1 50 50 0 > $(TEST_RESULT_DIR)/test3-1.txt
	$(BUILD_DIR)/nbhashtable 2 50 50 0 > $(TEST_RESULT_DIR)/test3-2.txt
	$(BUILD_DIR)/nbhashtable 4 50 50 0 > $(TEST_RESULT_DIR)/test3-4.txt
	$(BUILD_DIR)/nbhashtable 8 50 50 0 > $(TEST_RESULT_DIR)/test3-8.txt

time4:
	@echo Testing trial 4
	$(BUILD_DIR)/nbhashtable 1 45 10 45 > $(TEST_RESULT_DIR)/test4-1.txt
	$(BUILD_DIR)/nbhashtable 2 45 10 45 > $(TEST_RESULT_DIR)/test4-2.txt
	$(BUILD_DIR)/nbhashtable 4 45 10 45 > $(TEST_RESULT_DIR)/test4-4.txt
	$(BUILD_DIR)/nbhashtable 8 45 10 45 > $(TEST_RESULT_DIR)/test4-8.txt

time5:
	@echo Testing trial 5
	$(BUILD_DIR)/nbhashtable 1 10 45 45 > $(TEST_RESULT_DIR)/test5-1.txt
	$(BUILD_DIR)/nbhashtable 2 10 45 45 > $(TEST_RESULT_DIR)/test5-2.txt
	$(BUILD_DIR)/nbhashtable 4 10 45 45 > $(TEST_RESULT_DIR)/test5-4.txt
	$(BUILD_DIR)/nbhashtable 8 10 45 45 > $(TEST_RESULT_DIR)/test5-8.txt

time6:
	@echo Testing trial 6
	$(BUILD_DIR)/nbhashtable 1 33 33 34 > $(TEST_RESULT_DIR)/test6-1.txt
	$(BUILD_DIR)/nbhashtable 2 33 33 34 > $(TEST_RESULT_DIR)/test6-2.txt
	$(BUILD_DIR)/nbhashtable 4 33 33 34 > $(TEST_RESULT_DIR)/test6-4.txt
	$(BUILD_DIR)/nbhashtable 8 33 33 34 > $(TEST_RESULT_DIR)/test6-8.txt
