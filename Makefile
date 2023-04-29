# Compiler and flags
CXX = g++
INCLUDE =   # add needed libs.
CXXFLAGS = -Wall -Wextra -O3 -D$(TEST) -D$(DEBUG) -fopenmp -Wunused-result $(INCLUDE)

# Directories
SRCDIR = src
BUILDDIR = build
TESTDIR = test

# google benchmark
BENCHMARK_CFLAGS := $(shell pkg-config --cflags benchmark)
BENCHMARK_LDFLAGS := $(shell pkg-config --libs benchmark)

# Targets
TARGET = $(BUILDDIR)/cyk
BENCHMARK_TARGET = $(BUILDDIR)/benchmark
CYKOMP = $(BUILDDIR)/cykomp.o

# ANSI color codes
RED = \033[0;31m
GREEN = \033[0;32m
NC = \033[0m # No color

# Rules
.PHONY: all serial clean test

all: serial

serial: $(TARGET)

benchmark: $(BENCHMARK_TARGET)

$(CYKOMP): $(SRCDIR)/cykomp.cpp  $(SRCDIR)/cykomp.h
	@mkdir -p $(BUILDDIR)
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(TARGET): $(SRCDIR)/cyk.cpp $(CYKOMP)
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -o $@ $< $(CYKOMP)

clean:
	@rm -rf $(BUILDDIR)

test: serial
	@echo "Running tests..."
	@for input_file in $(wildcard $(TESTDIR)/input*.txt); do \
	    if ! echo $$input_file | grep -q "\.ans"; then \
	        ans_file=$$(echo $$input_file | sed 's/\.txt/\.ans\.txt/'); \
	        echo "Testing with $$input_file and $$ans_file..."; \
	        ./$(TARGET) < $$input_file > output.txt; \
	        if cmp -s output.txt $$ans_file; then \
	            printf "$(GREEN)Test passed$(NC)\n"; \
	        else \
	            printf "$(RED)Test failed$(NC)\n"; \
	        fi; \
	    fi; \
	done; \
	rm -f output.txt

run: serial
	@echo "Running use test/input1.txt"
	@./$(TARGET) < "test/input1.txt"

$(BENCHMARK_TARGET): $(SRCDIR)/benchmark.cpp $(CYKOMP)
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(BENCHMARK_CFLAGS) -o $@ $< $(CYKOMP) $(BENCHMARK_LDFLAGS)

db:
	bear -- make
