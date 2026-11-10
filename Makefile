CXX ?= g++
CPPFLAGS += -I.
CXXFLAGS ?= -Wall -Wextra -Wpedantic -g
CXXFLAGS += -std=c++20
LOAD_CXXFLAGS ?= -O2

BUILD_DIR := build

.PHONY: all test load leak-demo plot clean

all: $(BUILD_DIR)/functional_tests $(BUILD_DIR)/load_tests $(BUILD_DIR)/leak_demo

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/functional_tests: tests/main.cpp tests/tests.cpp tests/tests.h ShrdPtr.h ControlBlock.h ControlBlock.cpp Makefile | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/main.cpp tests/tests.cpp ControlBlock.cpp -o $@

$(BUILD_DIR)/load_tests: tests/load.cpp ShrdPtr.h ControlBlock.h ControlBlock.cpp Makefile | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LOAD_CXXFLAGS) tests/load.cpp ControlBlock.cpp -o $@

$(BUILD_DIR)/leak_demo: tests/with_leaks.cpp ShrdPtr.h ControlBlock.h ControlBlock.cpp Makefile | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/with_leaks.cpp ControlBlock.cpp -o $@

test: $(BUILD_DIR)/functional_tests
	./$(BUILD_DIR)/functional_tests

load: $(BUILD_DIR)/load_tests
	./$(BUILD_DIR)/load_tests

leak-demo: $(BUILD_DIR)/leak_demo
	./$(BUILD_DIR)/leak_demo

plot: $(BUILD_DIR)/load_tests tests/plot_load.py
	./$(BUILD_DIR)/load_tests > $(BUILD_DIR)/load-1.csv
	./$(BUILD_DIR)/load_tests > $(BUILD_DIR)/load-2.csv
	./$(BUILD_DIR)/load_tests > $(BUILD_DIR)/load-3.csv
	python3 tests/plot_load.py $(BUILD_DIR)/load-1.csv $(BUILD_DIR)/load-2.csv $(BUILD_DIR)/load-3.csv

clean:
	$(RM) -r $(BUILD_DIR)
