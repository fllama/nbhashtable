all: tests

BUILD_DIR=build
TEST_RESULT_DIR=results
SRC_DIR=src
INCL_DIR=include
TEST_DIR=tests

CC = g++
CFLAGS= -std=c++11 -pthread -I $(INCL_DIR)


tests: correctness executionTime atomictypes instruction

build/.placeholder:
	mkdir -vp $(BUILD_DIR)
	touch $(BUILD_DIR)/.placeholder

results/.placeholder:
	mkdir -vp $(TEST_RESULT_DIR)
	touch $(TEST_RESULT_DIR)/.placeholder

#
# Atomic test
#
atomictypes: build/.placeholder
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/atomict $(TEST_DIR)/atomictypes.cpp $(SRC_DIR)/NBHashTable.cpp $(SRC_DIR)/VersionState.cpp $(SRC_DIR)/ProbeBound.cpp
	@echo ProbeBound and VersionState binary "atomict" placed into $(BUILD_DIR)/
	
#
# Instructions test
#
instruction: build/.placeholder
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/instructions $(TEST_DIR)/instructions.cpp $(SRC_DIR)/NBHashTable.cpp $(SRC_DIR)/VersionState.cpp $(SRC_DIR)/ProbeBound.cpp
	@echo Instruction correctness tester "instructions" placed into $(BUILD_DIR)/

#
# Correctness test
#
correctness: build/.placeholder results/.placeholder
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/correctness1 $(TEST_DIR)/correctness1.cpp $(SRC_DIR)/NBHashTable.cpp $(SRC_DIR)/VersionState.cpp $(SRC_DIR)/ProbeBound.cpp
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/correctness2 $(TEST_DIR)/correctness2.cpp $(SRC_DIR)/NBHashTable.cpp $(SRC_DIR)/VersionState.cpp $(SRC_DIR)/ProbeBound.cpp
	@echo Correctness executable files built and placed into $(BUILD_DIR)/

#
# Execution time tests
#
executionTime: build/.placeholder results/.placeholder
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/executionTime $(TEST_DIR)/executiontime.cpp $(SRC_DIR)/NBHashTable.cpp $(SRC_DIR)/VersionState.cpp $(SRC_DIR)/ProbeBound.cpp
	@echo Execution time executable file built and placed into $(BUILD_DIR)/

executionTimeTest: executionTime time1 time2 time3 time4 time5 time6
	@echo Execution time testing complete. Placed results into $(TEST_RESULT_DIR)/

time1:
	@echo Testing trial 1
	$(BUILD_DIR)/executionTime 1 75 25 0 > $(TEST_RESULT_DIR)/test1-1.txt
	$(BUILD_DIR)/executionTime 2 75 25 0 > $(TEST_RESULT_DIR)/test1-2.txt
	$(BUILD_DIR)/executionTime 4 75 25 0 > $(TEST_RESULT_DIR)/test1-4.txt
	$(BUILD_DIR)/executionTime 8 75 25 0 > $(TEST_RESULT_DIR)/test1-8.txt
	
time2:
	@echo Testing trial 2
	$(BUILD_DIR)/executionTime 1 25 75 0 > $(TEST_RESULT_DIR)/test2-1.txt
	$(BUILD_DIR)/executionTime 2 25 75 0 > $(TEST_RESULT_DIR)/test2-2.txt
	$(BUILD_DIR)/executionTime 4 25 75 0 > $(TEST_RESULT_DIR)/test2-4.txt
	$(BUILD_DIR)/executionTime 8 25 75 0 > $(TEST_RESULT_DIR)/test2-8.txt
	
time3:
	@echo Testing trial 3
	$(BUILD_DIR)/executionTime 1 50 50 0 > $(TEST_RESULT_DIR)/test3-1.txt
	$(BUILD_DIR)/executionTime 2 50 50 0 > $(TEST_RESULT_DIR)/test3-2.txt
	$(BUILD_DIR)/executionTime 4 50 50 0 > $(TEST_RESULT_DIR)/test3-4.txt
	$(BUILD_DIR)/executionTime 8 50 50 0 > $(TEST_RESULT_DIR)/test3-8.txt

time4:
	@echo Testing trial 4
	$(BUILD_DIR)/executionTime 1 45 10 45 > $(TEST_RESULT_DIR)/test4-1.txt
	$(BUILD_DIR)/executionTime 2 45 10 45 > $(TEST_RESULT_DIR)/test4-2.txt
	$(BUILD_DIR)/executionTime 4 45 10 45 > $(TEST_RESULT_DIR)/test4-4.txt
	$(BUILD_DIR)/executionTime 8 45 10 45 > $(TEST_RESULT_DIR)/test4-8.txt

time5:
	@echo Testing trial 5
	$(BUILD_DIR)/executionTime 1 10 45 45 > $(TEST_RESULT_DIR)/test5-1.txt
	$(BUILD_DIR)/executionTime 2 10 45 45 > $(TEST_RESULT_DIR)/test5-2.txt
	$(BUILD_DIR)/executionTime 4 10 45 45 > $(TEST_RESULT_DIR)/test5-4.txt
	$(BUILD_DIR)/executionTime 8 10 45 45 > $(TEST_RESULT_DIR)/test5-8.txt

time6:
	@echo Testing trial 6
	$(BUILD_DIR)/executionTime 1 33 33 34 > $(TEST_RESULT_DIR)/test6-1.txt
	$(BUILD_DIR)/executionTime 2 33 33 34 > $(TEST_RESULT_DIR)/test6-2.txt
	$(BUILD_DIR)/executionTime 4 33 33 34 > $(TEST_RESULT_DIR)/test6-4.txt
	$(BUILD_DIR)/executionTime 8 33 33 34 > $(TEST_RESULT_DIR)/test6-8.txt
