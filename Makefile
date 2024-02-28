MODULES = io.c utils.c table.c tree.c
SRC = $(MODULES) main.c
OUTPUT_FILE = ./my_route_lookup

TESTS_FOLDER = ./tests
SRC_TESTS = $(MODULES) $(wildcard $(TESTS_FOLDER)/*.c) 
OUTPUT_FILE_TESTS = $(TESTS_FOLDER)/test

CFLAGS = -Wall -g -Ofast
VFLAGS = -s --leak-check=full --show-leak-kinds=all --track-origins=yes 

RESOURCES_FOLDER = ./resources
FIB_FILE = $(RESOURCES_FOLDER)/routing_table.txt
INPUT_FILE = $(RESOURCES_FOLDER)/prueba2.txt
RESULT_FILE = $(INPUT_FILE).out

MULTIBIT_FILE = ./$(RESOURCES_FOLDER)/multiBit
MULTIBIT_RESULT_FILE = $(RESULT_FILE).mb

LINEAR_SEARCH_FILE = ./$(RESOURCES_FOLDER)/linearSearch
LINEAR_SEARCH_RESULT_FILE = $(RESULT_FILE).ls

COMPARE_FILE = ./compare_algo.sh

COMMAND = $(OUTPUT_FILE) $(FIB_FILE) $(INPUT_FILE)
COMMAND_MULTIBIT= $(MULTIBIT_FILE) $(FIB_FILE) $(INPUT_FILE)
COMMAND_LINEAR_SEARCH = $(LINEAR_SEARCH_FILE) $(FIB_FILE) $(INPUT_FILE)
COMMAND_COMPARE = $(COMPARE_FILE) $(MULTIBIT_RESULT_FILE) $(LINEAR_SEARCH_RESULT_FILE) $(RESULT_FILE)

all: compile

compile: $(SRC)
	gcc $(CFLAGS) $(SRC) -o $(OUTPUT_FILE) -lm

run: compile
	$(COMMAND)

valgrind: compile
	valgrind $(VFLAGS) $(COMMAND)

gdb: compile
	gdb --args $(COMMAND)

.PHONY: clean
clean:
	rm -f $(OUTPUT_FILE)

compile_tests: $(SRC_TESTS) 
	gcc $(CFLAGS) $(SRC_TESTS) -o $(OUTPUT_FILE_TESTS) -lm

run_tests: compile_tests
	$(OUTPUT_FILE_TESTS)

test: compile
	$(COMMAND_MULTIBIT) &> /dev/null
	mv $(RESULT_FILE) $(MULTIBIT_RESULT_FILE) &> /dev/null
	$(COMMAND_LINEAR_SEARCH) &> /dev/null
	mv $(RESULT_FILE) $(LINEAR_SEARCH_RESULT_FILE) &> /dev/null
	$(COMMAND) &> /dev/null

	$(COMMAND_COMPARE)
