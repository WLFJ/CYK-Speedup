# Compiler and flags
CXX = g++
INCLUDE =   # add needed libs.
CXXFLAGS = -Wall -Wextra -O3 -fopenmp -DOMP -Wunused-result $(INCLUDE)

# Directories
SRCDIR = src
BUILDDIR = build
TESTDIR = test

# Targets
TARGET = $(BUILDDIR)/omp

# ANSI color codes
RED = \033[0;31m
GREEN = \033[0;32m
NC = \033[0m # No color

# Rules
.PHONY: all serial clean test

all: serial

serial: $(TARGET)

$(TARGET): $(SRCDIR)/omp.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $< -o $@

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

db:
	bear -- make
